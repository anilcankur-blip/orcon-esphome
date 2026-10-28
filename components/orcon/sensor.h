#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {

class OrconHumiditySensor : public sensor::Sensor, public Component {
 public:
  void set_sensor_type(uint8_t type) { sensor_type_ = type; }
  void loop() override;

 protected:
  uint8_t sensor_type_ = 0;  // 0=humidity, 1=exhaust, 2=supply, 3=indoor, 4=outdoor
  float last_published_ = -999.0f;
};

}  // namespace esphome::orcon_esphome
