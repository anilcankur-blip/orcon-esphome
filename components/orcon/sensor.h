#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {

class OrconHumiditySensor : public sensor::Sensor, public Component {
 public:
  void loop() override;

 protected:
  int last_published_ = -1;
};

}  // namespace esphome::orcon_esphome
