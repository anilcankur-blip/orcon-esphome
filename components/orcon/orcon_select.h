#pragma once
#include "esphome/components/select/select.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {
class OrconModeSelect : public select::Select, public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void control(const std::string &value) override;

 protected:
  void publish_orcon_state_();
};
}  // namespace esphome::orcon_esphome
