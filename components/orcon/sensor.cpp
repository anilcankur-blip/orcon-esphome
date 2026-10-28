#include "sensor.h"
#include "orcon.h"

namespace esphome::orcon_esphome {

void OrconHumiditySensor::loop() {
  const auto &state = orcon::state();
  if (!state.humidityKnown) return;

  const float value = state.humidity;
  if (value > 100 || value == last_published_) return;

  last_published_ = value;
  this->publish_state(value);
}

}  // namespace esphome::orcon_esphome
