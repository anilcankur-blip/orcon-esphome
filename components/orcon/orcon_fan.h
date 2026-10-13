#pragma once
#include "esphome/components/fan/fan.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {
class OrconFan : public fan::Fan, public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  fan::FanTraits get_traits() override;
  void control(const fan::FanCall &call) override;
 protected:
  void publish_orcon_state_();
};
}  // namespace esphome::orcon_esphome
