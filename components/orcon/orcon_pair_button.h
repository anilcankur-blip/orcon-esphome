#pragma once
#include <string>
#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {
class OrconPairButton : public button::Button, public Component {
 public:
  void setup() override;
  void loop() override;
  void set_mode(const std::string &mode) { mode_ = mode; }
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

 protected:
  void press_action() override;

 private:
  std::string mode_;
  bool radio_owner_ = false;
};
}  // namespace esphome::orcon_esphome
