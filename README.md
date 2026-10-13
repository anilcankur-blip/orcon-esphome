# Orcon 15RF ESPHome external component

ESPHome external component adapter for an Orcon 15RF / compatible ventilation unit using an ESP32 and external CC1101 radio.

## Status

This repository is under development. A successful GitHub Actions build validates compilation only; it does not prove that RF communication with a physical Orcon unit works. Do not rely on the component until the RF receive, pairing, and command path have been tested on the target hardware.

## Hardware wiring

| CC1101 | ESP32 |
| --- | --- |
| SCK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| CSN / CS | GPIO5 |
| GDO0 | GPIO2 |
| GDO2 | GPIO4 |
| VCC / GND | Use the module's supported supply and common ground |

The radio code uses 868.3 MHz and 38400 baud asynchronous RX/TX. Check the wiring and module voltage requirements before powering the board.

## Install from ESPHome Builder

Copy `example.yaml` into your ESPHome configuration, set your Wi-Fi secrets, and change the device name if needed. The example loads this component directly from GitHub:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/anilcankur-blip/orcon-esphome
      ref: main
    components: [orcon]
```

The component exposes an Orcon mode select (Away, 1, 2, 3, Auto, Boost) and a pairing button. It intentionally does not expose a generic fan ON/OFF switch: Away is a real operating mode, not proof that the ventilation unit is powered off. The selected mode is updated from received fan telemetry; sending a command alone is not treated as confirmation.

## Pairing and testing

1. Compile the YAML in ESPHome Builder first.
2. If compilation succeeds, install it on the ESP32.
3. Open the device logs and verify that the CC1101 VERSION register is read as a plausible value (typically around `0x14`).
4. Start pairing from the Home Assistant button, then power-cycle the Orcon unit within the three-minute pairing window.
5. Verify that frames are received and that a fan-mode command gets a response before treating the setup as working.

## Development / CI

The GitHub Actions workflow validates and compiles `tests/compile.yaml`, which points to the local component directory. The user-facing `example.yaml` uses the GitHub external-component source. This allows changes to be tested before merging into `main`.

## Known limitations

- Physical RF communication has not yet been verified.
- Pairing identity persistence must be confirmed in code and on hardware; do not assume pairing survives a restart until tested.
- Additional protocol features (timed boost, bypass, AC/custom fan settings, telemetry sensors, and keeper settings) still need ESPHome entities.
- Different Orcon models may behave differently.
