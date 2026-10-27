#include "sensor.h"
#include "orcon.h"

namespace esphome::orcon_esphome {

void OrconHumiditySensor::loop() {
  const auto &state = orcon::state();
  if (!state.humidityKnown) return;

  const int humidity = state.humidity;
  if (humidity > 100 || humidity == last_published_) return;

  last_published_ = humidity;
  this->publish_state(static_cast<float>(humidity));
}

}  // namespace esphome::orcon_esphome
