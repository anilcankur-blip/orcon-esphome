#pragma once

#include <stdint.h>

#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {

class OrconFanModeTextSensor : public text_sensor::TextSensor, public Component {
 public:
  void loop() override;

 protected:
  uint8_t last_published_mode_ = 0xFF;
};

}  // namespace esphome::orcon_esphome
