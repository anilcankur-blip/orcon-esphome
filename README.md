# Orcon 15RF ESPHome adapter

This package contains the ESPHome adapter layer for
`me-processware/orcon-15rf-controller`.

The original project is a PlatformIO application, not an ESPHome external
component. The RF/protocol source therefore has to be vendored into the
component directory before ESPHome Builder can compile it.

The original project documents CC1101 async RX+TX as its working path:
CC1101 GDO0 is the UART TX/data-in line and GDO2 is the UART RX/data-out line,
using 38400 8N1 framing. It uses 868.3 MHz, 38.4 kbps and about 50.78 kHz
deviation. See the original repository and its source for those settings.

Hardware used here:
SCK GPIO18, MOSI GPIO23, MISO GPIO19, CSN GPIO5, GDO0 GPIO2, GDO2 GPIO4.

IMPORTANT:
I cannot push files into your GitHub account from this chat. Also, the current
web runtime can open the individual source files but cannot export the complete
GitHub src tree as a local checkout. Therefore this package is intentionally a
verified adapter layer plus a vendor script, not a claim that it is already a
complete flash-ready GitHub repository.

The ESPHome configuration exposes a fan entity with Away / Low / Medium / High /
Auto / Boost presets and a pairing button. To pair: press the pairing button in
Home Assistant, then power-cycle the Orcon unit within the three-minute pairing
window. Confirm actual reception and control before relying on it.

Do not flash this adapter until ESPHome Builder shows a successful compile.
This package has not yet been compiled in the user's ESPHome environment.
