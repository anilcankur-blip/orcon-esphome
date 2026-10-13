// CC1101 async-serial transport for Orcon RAMSES frames.
#include "cc1101_tx.h"
#include "config.h"
#include "orcon_compat.h"
#include <driver/spi_master.h>
#include <string.h>

#if defined(USE_CC1101_TX) || defined(RADIO_CC1101_ASYNC)

namespace cc1101tx {

static HardwareSerial ccUart(1);
static spi_device_handle_t ccDevice = nullptr;
static bool g_spiReady = false;

// CC1101 async serial register image based on evofw3 cc1101_param.c.
// GDO2 outputs demodulated RX bits; GDO0 accepts TX bits.
static const uint8_t CFG[] = {
  0x0D,0x2E,0x2E,0x07,0xD3,0x91,0xFF,0x04,0x31,0x00,0x00,0x0F,0x00,0x21,0x65,0x6A,
  0x6A,0x83,0x10,0x22,0xF8,0x50,0x07,0x30,0x18,0x16,0x6C,0x43,0x40,0x91,0x87,0x6B,
  0xF8,0x56,0x10,0xE9,0x21,0x00,0x1F,0x41,0x00,0x59,0x7F,0x3F,0x81,0x35,0x09
};
static const uint8_t PA = 0xC3;
static uint8_t g_ver = 0;

enum { SRES=0x30, SRX=0x34, STX=0x35, SIDLE=0x36, SFRX=0x3A, SFTX=0x3B };

static bool spiTransfer(const uint8_t *tx, uint8_t *rx, size_t len) {
    if (ccDevice == nullptr || len == 0) return false;
    spi_transaction_t t = {};
    t.length = len * 8;
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    return spi_device_transmit(ccDevice, &t) == ESP_OK;
}

static void strobe(uint8_t cmd) {
    (void) spiTransfer(&cmd, nullptr, 1);
}

static void writeBurst(uint8_t addr, const uint8_t *data, uint8_t len) {
    uint8_t tx[1 + sizeof(CFG)];
    if (len > sizeof(CFG)) return;
    tx[0] = addr | 0x40;
    memcpy(tx + 1, data, len);
    (void) spiTransfer(tx, nullptr, (size_t)len + 1);
}

static uint8_t readStatus(uint8_t addr) {
    uint8_t tx[2] = { (uint8_t)(addr | 0xC0), 0x00 };
    uint8_t rx[2] = { 0, 0 };
    if (!spiTransfer(tx, rx, sizeof(tx))) return 0xFF;
    return rx[1];
}

bool begin() {
    if (!g_spiReady) {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = PIN_CC_MOSI;
        buscfg.miso_io_num = PIN_CC_MISO;
        buscfg.sclk_io_num = PIN_CC_SCK;
        buscfg.quadwp_io_num = -1;
        buscfg.quadhd_io_num = -1;
        buscfg.max_transfer_sz = 64;

        esp_err_t busErr = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_DISABLED);
        if (busErr != ESP_OK && busErr != ESP_ERR_INVALID_STATE) {
            Serial.printf("[cc1101] SPI bus init failed: %d\n", (int)busErr);
            return false;
        }

        spi_device_interface_config_t devcfg = {};
        devcfg.clock_speed_hz = 4000000;
        devcfg.mode = 0;
        devcfg.spics_io_num = PIN_CC_NSS;
        devcfg.queue_size = 1;
        esp_err_t devErr = spi_bus_add_device(SPI3_HOST, &devcfg, &ccDevice);
        if (devErr != ESP_OK) {
            Serial.printf("[cc1101] SPI device init failed: %d\n", (int)devErr);
            return false;
        }
        g_spiReady = true;
    }

    delay(10);
    for (int attempt = 0; attempt < 6; attempt++) {
        strobe(SRES);
        delay(5);
        writeBurst(0x00, CFG, sizeof(CFG));
        writeBurst(0x3E, &PA, 1);
        strobe(SIDLE);
        g_ver = readStatus(0x31);
        if (g_ver != 0x00 && g_ver != 0xFF) break;
        delay(10);
    }

    Serial.printf("[cc1101] version=0x%02X (expect ~0x14)\n", g_ver);
    ccUart.setRxBufferSize(8192);
    ccUart.begin(38400, SERIAL_8N1, PIN_CC_GDO2, PIN_CC_GDO0);
    return (g_ver != 0x00 && g_ver != 0xFF);
}

bool send(const ramses::Frame& f) {
    uint8_t logical[ramses::MAX_LOGICAL];
    size_t ln = ramses::buildLogical(f, logical, sizeof(logical));
    if (!ln) return false;
    uint8_t wire[ramses::MAX_WIRE];
    size_t wn = ramses::wireFromLogical(logical, ln, wire, sizeof(wire));
    if (!wn) return false;

    strobe(SIDLE);
    strobe(SFTX);
    strobe(STX);
    delayMicroseconds(300);

    for (int i = 0; i < RF_PREAMBLE_BYTES + 2; i++) ccUart.write((uint8_t)0x55);
    ccUart.write(wire, wn);
    ccUart.flush();
    delayMicroseconds(200);
    strobe(SIDLE);
    return true;
}

uint8_t version() { return g_ver; }

static uint8_t g_rxbuf[320];
static size_t g_rxlen = 0;
static int g_rssi = -127;
static uint32_t g_uartBytes = 0;
uint32_t uartBytes() { return g_uartBytes; }

void startRx() {
    strobe(SIDLE);
    strobe(SFRX);
    strobe(SRX);
    while (ccUart.available()) ccUart.read();
    g_rxlen = 0;
}

int rssiDbm() {
    uint8_t r = readStatus(0x34);
    int v = (r >= 128) ? (r - 256) : r;
    return v / 2 - 74;
}

bool poll(ramses::Frame& out) {
    while (ccUart.available() && g_rxlen < sizeof(g_rxbuf)) {
        g_rxbuf[g_rxlen++] = (uint8_t)ccUart.read();
        g_uartBytes++;
    }
    if (g_rxlen < 8) return false;

    size_t consumed = 0;
    bool ok = ramses::extractWireFrame(g_rxbuf, g_rxlen, &consumed, out);
    if (ok) g_rssi = rssiDbm();
    if (consumed > 0 && consumed <= g_rxlen) {
        memmove(g_rxbuf, g_rxbuf + consumed, g_rxlen - consumed);
        g_rxlen -= consumed;
    } else if (g_rxlen >= sizeof(g_rxbuf)) {
        g_rxlen = 0;
    }
    return ok;
}

} // namespace cc1101tx
#endif // USE_CC1101_TX || RADIO_CC1101_ASYNC
