#pragma once
#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {
class OrconPairButton : public button::Button, public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

 protected:
  void press_action() override;

 private:
  bool radio_owner_ = false;
};
}  // namespace esphome::orcon_esphome
