#pragma once
#include "esphome/components/number/number.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {

class OrconTimerNumber : public number::Number, public Component {
 public:
  void control(float value) override;
};

}  // namespace esphome::orcon_esphome
