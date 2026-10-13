#include "orcon_pair_button.h"
#include "orcon.h"

namespace esphome::orcon_esphome {
void OrconPairButton::press_action() { orcon::startPairing(); }
}  // namespace esphome::orcon_esphome
