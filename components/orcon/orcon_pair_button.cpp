#include "orcon_pair_button.h"
#include "orcon.h"
#include "ramses_radio.h"
#include "orcon_compat.h"
#include "esp_mac.h"
#include "esphome/core/log.h"

namespace esphome::orcon_esphome {

static const char *TAG = "orcon";
static bool init_attempted = false;
static bool init_ok = false;
static uint32_t last_logged_tx = 0;
static uint32_t last_logged_rx = 0;
static orcon::PairState last_pair_state = orcon::PairState::Idle;

// Each Home Assistant button is assigned one of these callbacks by button.py.
void modeAway()  { ESP_LOGI(TAG, "Button command: Away"); orcon::setMode(orcon::Mode::Away); }
void mode1()     { ESP_LOGI(TAG, "Button command: mode 1 / Low"); orcon::setMode(orcon::Mode::Low); }
void mode2()     { ESP_LOGI(TAG, "Button command: mode 2 / Medium"); orcon::setMode(orcon::Mode::Medium); }
void mode3()     { ESP_LOGI(TAG, "Button command: mode 3 / High"); orcon::setMode(orcon::Mode::High); }
void modeAuto()  { ESP_LOGI(TAG, "Button command: Auto"); orcon::setMode(orcon::Mode::Auto); }
void modeBoost() { ESP_LOGI(TAG, "Button command: Boost"); orcon::setMode(orcon::Mode::Boost); }
void startPairing() { ESP_LOGI(TAG, "Button command: start pairing"); orcon::startPairing(); }

void OrconPairButton::setup() {
  if (!init_attempted) {
    init_attempted = true;
    this->radio_owner_ = true;
    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read ESP32 MAC address");
      this->mark_failed();
      return;
    }

    uint32_t us_serial = ((((uint32_t) mac[3]) << 16) |
                          (((uint32_t) mac[4]) << 8) |
                          mac[5]) & 0x3FFFFUL;

    if (!radio::begin()) {
      ESP_LOGE(TAG, "CC1101 initialization failed");
      this->mark_failed();
      return;
    }

    orcon::begin(0x25, us_serial, 0x20, 0);
    radio::startReceive();
    ESP_LOGI(TAG, "Orcon transport ready (%s), controller serial=%06lX",
             radio::name(), (unsigned long) us_serial);
    init_ok = true;
  }

  if (!init_ok) this->mark_failed();
}

void OrconPairButton::press_action() {
  if (!init_ok) {
    ESP_LOGE(TAG, "Command ignored because Orcon radio is not initialized");
    return;
  }
  if (this->action_ == nullptr) {
    ESP_LOGE(TAG, "No command callback configured for this button");
    return;
  }

  this->action_();
}

void OrconPairButton::loop() {
  if (!radio_owner_ || !init_ok) return;
  ramses::Frame frame;
  if (radio::poll(frame)) {
    orcon::onFrame(frame);
  }

  const auto &state = orcon::state();
  const auto &ids = orcon::ids();

  if (state.txCount != last_logged_tx) {
    last_logged_tx = state.txCount;
    ESP_LOGI(TAG, "RF TX #%lu result=%s frame=[%s] fan_id=%02X:%06lX",
             (unsigned long) state.txCount,
             state.lastTxOk ? "OK (transport only)" : "FAILED",
             state.lastTx, ids.fan.cls, (unsigned long) ids.fan.serial);
  }
  if (state.rxCount != last_logged_rx) {
    last_logged_rx = state.rxCount;
    ESP_LOGI(TAG, "RF RX #%lu RSSI=%.0f dBm frame=[%s] fan_known=%s fan_id=%02X:%06lX",
             (unsigned long) state.rxCount, radio::lastRssi(), state.lastRx,
             state.fanKnown ? "yes" : "no", ids.fan.cls,
             (unsigned long) ids.fan.serial);
  }

  const auto pair_state = orcon::pairState();
  if (pair_state != last_pair_state) {
    last_pair_state = pair_state;
    const char *label = pair_state == orcon::PairState::Idle ? "idle" :
                        pair_state == orcon::PairState::Searching ? "searching" :
                        pair_state == orcon::PairState::Paired ? "paired" : "timeout";
    ESP_LOGI(TAG, "Pairing state=%s, fan_known=%s, fan_id=%02X:%06lX, window_left=%lu ms",
             label, state.fanKnown ? "yes" : "no", ids.fan.cls,
             (unsigned long) ids.fan.serial,
             (unsigned long) orcon::pairWindowLeftMs());
  }

  orcon::tick(millis());
}

}  // namespace esphome::orcon_esphome
