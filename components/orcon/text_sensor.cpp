#include "esphome/core/defines.h"

#ifdef USE_TEXT_SENSOR

#include "text_sensor.h"
#include "orcon.h"

namespace esphome::orcon_esphome {

void OrconFanModeTextSensor::loop() {
  const auto &state = orcon::state();
  if (!state.fanMode31DAKnown || state.fanMode31DA == last_published_mode_) return;

  const char *value = nullptr;
  switch (state.fanMode31DA) {
    case 0x00: value = "Off"; break;
    case 0x01:
    case 0x0B: value = "Low"; break;
    case 0x02:
    case 0x0C: value = "Medium"; break;
    case 0x03:
    case 0x0D:
    case 0x17: value = "High"; break;
    case 0x15:
    case 0x16: value = "Away"; break;
    case 0x18:
    case 0x19: value = "Auto"; break;
    default: return;
  }

  last_published_mode_ = state.fanMode31DA;
  this->publish_state(value);
}

}  // namespace esphome::orcon_esphome

#endif  // USE_TEXT_SENSOR
