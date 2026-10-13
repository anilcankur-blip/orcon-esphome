#pragma once
#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {
class OrconPairButton : public button::Button, public Component {
 protected:
  void press_action() override;
};
}  // namespace esphome::orcon_esphome
