#pragma once
#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {

using OrconCommand = void (*)();

void modeAway();
void mode1();
void mode2();
void mode3();
void modeAuto();
void modeBoost();
void startPairing();

class OrconPairButton : public button::Button, public Component {
 public:
  void setup() override;
  void loop() override;
  void set_action(OrconCommand action) { this->action_ = action; }
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

 protected:
  void press_action() override;

 private:
  OrconCommand action_{nullptr};
  bool radio_owner_ = false;
};
}  // namespace esphome::orcon_esphome
