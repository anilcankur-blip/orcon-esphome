#include "orcon_pair_button.h"
#include "orcon.h"

namespace esphome::orcon_esphome {
void OrconPairButton::press_action() {
  if (mode_.empty()) {
    orcon::startPairing();
    return;
  }

  if (mode_ == "Away") orcon::setMode(orcon::Mode::Away);
  else if (mode_ == "1") orcon::setMode(orcon::Mode::Low);
  else if (mode_ == "2") orcon::setMode(orcon::Mode::Medium);
  else if (mode_ == "3") orcon::setMode(orcon::Mode::High);
  else if (mode_ == "Auto") orcon::setMode(orcon::Mode::Auto);
  else if (mode_ == "Boost") orcon::setMode(orcon::Mode::Boost);
}
}  // namespace esphome::orcon_esphome
