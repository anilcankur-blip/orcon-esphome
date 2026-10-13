#include "orcon_select.h"
#include "ramses_radio.h"
#include "orcon.h"
#include "orcon_compat.h"
#include "esp_mac.h"

namespace esphome::orcon_esphome {

static const char *TAG = "orcon";

void OrconModeSelect::setup() {
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
  ESP_LOGI(TAG, "CC1101 Orcon transport ready, controller serial=%06lX",
           (unsigned long) us_serial);
}

void OrconModeSelect::publish_orcon_state_() {
  const auto mode = orcon::state().mode;
  const char *name = nullptr;
  switch (mode) {
    case orcon::Mode::Away: name = "Away"; break;
    case orcon::Mode::Low: name = "1"; break;
    case orcon::Mode::Medium: name = "2"; break;
    case orcon::Mode::High: name = "3"; break;
    case orcon::Mode::Auto: name = "Auto"; break;
    case orcon::Mode::Boost: name = "Boost"; break;
    default: break;
  }

  // Publish only a mode reported by the fan; never treat a transmitted
  // command as proof that the physical unit accepted it.
  if (name != nullptr && this->has_state() == false) {
    this->publish_state(name);
  } else if (name != nullptr && this->current_option() != name) {
    this->publish_state(name);
  }
}

void OrconModeSelect::loop() {
  if (this->is_failed()) return;

  ramses::Frame frame;
  if (radio::poll(frame)) {
    if (orcon::onFrame(frame)) this->publish_orcon_state_();
  }
  orcon::tick(millis());
}

void OrconModeSelect::control(const std::string &value) {
  orcon::Mode mode = orcon::Mode::Unknown;
  if (value == "Away") mode = orcon::Mode::Away;
  else if (value == "1") mode = orcon::Mode::Low;
  else if (value == "2") mode = orcon::Mode::Medium;
  else if (value == "3") mode = orcon::Mode::High;
  else if (value == "Auto") mode = orcon::Mode::Auto;
  else if (value == "Boost") mode = orcon::Mode::Boost;

  if (mode != orcon::Mode::Unknown) {
    // State will update when the ventilation unit reports its actual mode.
    orcon::setMode(mode);
  }
}

}  // namespace esphome::orcon_esphome
