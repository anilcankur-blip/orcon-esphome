#include "sensor.h"
#include "orcon.h"

namespace esphome::orcon_esphome {

void OrconHumiditySensor::loop() {
  const auto &state = orcon::state();

  if (sensor_type_ == 0) {
    if (!state.humidityKnown) return;
    const float value = state.humidity;
    if (value > 100 || value == last_published_) return;
    last_published_ = value;
    this->publish_state(value);
    return;
  }

  float value = 0.0f;
  bool known = false;
  switch (sensor_type_) {
    case 1: value = state.tempExhaust; known = state.tempExhaustKnown; break;
    case 2: value = state.tempSupply;  known = state.tempSupplyKnown;  break;
    case 3: value = state.tempIndoor;  known = state.tempIndoorKnown;  break;
    case 4: value = state.tempOutdoor; known = state.tempOutdoorKnown; break;
    case 5: value = state.exhaustPct; known = true; break;
    case 6: value = state.supplyPct;  known = true; break;
    default: return;
  }

  if (!known || value == last_published_) return;
  last_published_ = value;
  this->publish_state(value);
}

}  // namespace esphome::orcon_esphome
