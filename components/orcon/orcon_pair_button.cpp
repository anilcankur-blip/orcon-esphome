#include "orcon_pair_button.h"
#include "orcon.h"
#include "ramses_radio.h"
#include "orcon_compat.h"
#include "esp_mac.h"

namespace esphome::orcon_esphome {

static const char *TAG = "orcon";
static bool init_attempted = false;
static bool init_ok = false;

// Each entity name maps directly to its command; YAML needs no mode parameter.
static void modeAway()  { orcon::setMode(orcon::Mode::Away); }
static void mode1()     { orcon::setMode(orcon::Mode::Low); }
static void mode2()     { orcon::setMode(orcon::Mode::Medium); }
static void mode3()     { orcon::setMode(orcon::Mode::High); }
static void modeAuto()  { orcon::setMode(orcon::Mode::Auto); }
static void modeBoost() { orcon::setMode(orcon::Mode::Boost); }

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

  const std::string &name = this->get_name();
  if (name == "Orcon Eşleştirmeyi Başlat" || name == "Orcon Pairing") {
    orcon::startPairing();
  } else if (name == "Orcon Away") {
    modeAway();
  } else if (name == "Orcon 1") {
    mode1();
  } else if (name == "Orcon 2") {
    mode2();
  } else if (name == "Orcon 3") {
    mode3();
  } else if (name == "Orcon Auto") {
    modeAuto();
  } else if (name == "Orcon Boost") {
    modeBoost();
  } else {
    ESP_LOGW(TAG, "No command mapped for button name: %s", name.c_str());
  }
}

void OrconPairButton::loop() {
  if (!radio_owner_ || !init_ok) return;
  ramses::Frame frame;
  if (radio::poll(frame)) orcon::onFrame(frame);
  orcon::tick(millis());
}

}  // namespace esphome::orcon_esphome
