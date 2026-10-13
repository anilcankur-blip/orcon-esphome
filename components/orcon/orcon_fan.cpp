#include "orcon_fan.h"
#include "ramses_radio.h"
#include "orcon.h"
#include <Arduino.h>
#include <string.h>

namespace esphome::orcon_esphome {

static const char *TAG = "orcon";

fan::FanTraits OrconFan::get_traits() {
  fan::FanTraits traits;
  traits.set_speed(true);
  traits.set_supported_speed_count(5);
  this->set_supported_preset_modes({"Away", "Low", "Medium", "High", "Auto", "Boost"});
  return traits;
}

void OrconFan::setup() {
  uint64_t mac = ESP.getEfuseMac();
  uint32_t us_serial = (uint32_t)(mac & 0x3FFFFUL);
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

static const char *mode_to_preset(orcon::Mode m) {
  switch (m) {
    case orcon::Mode::Away: return "Away";
    case orcon::Mode::Low: return "Low";
    case orcon::Mode::Medium: return "Medium";
    case orcon::Mode::High: return "High";
    case orcon::Mode::Auto: return "Auto";
    case orcon::Mode::Boost: return "Boost";
    default: return nullptr;
  }
}

void OrconFan::publish_orcon_state_() {
  const auto &s = orcon::state();
  const char *preset = mode_to_preset(s.mode);

  if (s.mode == orcon::Mode::Away || s.mode == orcon::Mode::Disable) {
    this->state = false;
    this->speed = 0;
  } else {
    this->state = true;
    switch (s.mode) {
      case orcon::Mode::Low: this->speed = 1; break;
      case orcon::Mode::Medium: this->speed = 2; break;
      case orcon::Mode::High: this->speed = 3; break;
      case orcon::Mode::Auto: this->speed = 4; break;
      case orcon::Mode::Boost: this->speed = 5; break;
      default: this->speed = 0; break;
    }
  }

  if (preset != nullptr) this->set_preset_mode_(preset);
  else this->clear_preset_mode_();
  this->publish_state();
}

void OrconFan::loop() {
  if (this->is_failed()) return;

  ramses::Frame frame;
  if (radio::poll(frame)) {
    if (orcon::onFrame(frame)) this->publish_orcon_state_();
  }
  orcon::tick(millis());
}

void OrconFan::control(const fan::FanCall &call) {
  if (call.get_state().has_value() && !*call.get_state()) {
    orcon::setMode(orcon::Mode::Away);
    this->state = false;
    this->speed = 0;
    this->clear_preset_mode_();
    this->publish_state();
    return;
  }

  if (call.has_preset_mode()) {
    const char *p = call.get_preset_mode();
    if (!strcmp(p, "Away")) orcon::setMode(orcon::Mode::Away);
    else if (!strcmp(p, "Low")) orcon::setMode(orcon::Mode::Low);
    else if (!strcmp(p, "Medium")) orcon::setMode(orcon::Mode::Medium);
    else if (!strcmp(p, "High")) orcon::setMode(orcon::Mode::High);
    else if (!strcmp(p, "Auto")) orcon::setMode(orcon::Mode::Auto);
    else if (!strcmp(p, "Boost")) orcon::setMode(orcon::Mode::Boost);
    this->publish_orcon_state_();
    return;
  }

  if (call.get_speed().has_value()) {
    switch (*call.get_speed()) {
      case 1: orcon::setMode(orcon::Mode::Low); break;
      case 2: orcon::setMode(orcon::Mode::Medium); break;
      case 3: orcon::setMode(orcon::Mode::High); break;
      case 4: orcon::setMode(orcon::Mode::Auto); break;
      case 5: orcon::setMode(orcon::Mode::Boost); break;
      default: orcon::setMode(orcon::Mode::Auto); break;
    }
    this->publish_orcon_state_();
    return;
  }

  orcon::setMode(orcon::Mode::Auto);
  this->publish_orcon_state_();
}

}  // namespace esphome::orcon_esphome
