#pragma once
#include <string>
#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

namespace esphome::orcon_esphome {
class OrconPairButton : public button::Button, public Component {
 public:
  void set_mode(const std::string &mode) { mode_ = mode; }

 protected:
  void press_action() override;

 private:
  std::string mode_;
};
}  // namespace esphome::orcon_esphome
