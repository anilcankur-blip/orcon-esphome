#include "orcon_pair_button.h"
#include "orcon.h"
#include "ramses_radio.h"
#include "orcon_compat.h"
#include "esp_mac.h"

namespace esphome::orcon_esphome {

static const char *TAG = "orcon";
static bool init_attempted = false;
static bool init_ok = false;

void OrconPairButton::setup() {
  if (!init_attempted) {
    init_attempted = true;
    this->radio_owner_ = true;
    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read ESP32 MAC address");
      return;
    }

    uint32_t us_serial = ((((uint32_t) mac[3]) << 16) |
                          (((uint32_t) mac[4]) << 8) |
                          mac[5]) & 0x3FFFFUL;

    if (!radio::begin()) {
      ESP_LOGE(TAG, "CC1101 initialization failed");
      return;
    }

    orcon::begin(0x25, us_serial, 0x20, 0);
    radio::startReceive();
    ESP_LOGI(TAG, "CC1101 Orcon transport ready, controller serial=%06lX",
             (unsigned long) us_serial);
    init_ok = true;
  }

  if (!init_ok) this->mark_failed();
}

void OrconPairButton::press_action() {
  if (!init_ok) {
    ESP_LOGE(TAG, "Command ignored because Orcon radio is not initialized");
    return;
  }

  if (mode_.empty()) {
    orcon::startPairing();
    return;
  }

  if (mode_ == "Away") orcon::setMode(orcon::Mode::Away);
  else if (mode_ == "1") orcon::setMode(orcon::Mode::Low);
  else if (mode_ == "2") orcon::setMode(orcon::Mode::Medium);
  else if (mode_ == "3") orcon::setMode(orcon::Mode::High);
  else if (mode_ == "Auto") orcon::setMode(orcon::Mode::Auto);
  else if (mode_ == "Boost") orcon::setMode(orcon::Mode::Boost);
}

void OrconPairButton::loop() {
  if (!radio_owner_ || !init_ok) return;
  ramses::Frame frame;
  if (radio::poll(frame)) orcon::onFrame(frame);
  orcon::tick(millis());
}

}  // namespace esphome::orcon_esphome
