#!/usr/bin/env python3
import pathlib, urllib.request

BASE = "https://raw.githubusercontent.com/me-processware/orcon-15rf-controller/main/src/"
FILES = [
    "ramses_codec.cpp", "ramses_codec.h",
    "orcon_frames.cpp", "orcon_frames.h",
    "orcon.cpp", "orcon.h",
    "ramses_radio.cpp", "ramses_radio.h",
    "cc1101_tx.cpp", "cc1101_tx.h",
    "orcon_log.cpp", "orcon_log.h",
]

root = pathlib.Path(__file__).resolve().parents[1]
out = root / "components" / "orcon"
out.mkdir(parents=True, exist_ok=True)

for name in FILES:
    target = out / name
    if target.exists() and target.stat().st_size > 0:
        print("Already vendored; keeping", name)
        continue
    print("Downloading", name)
    try:
        data = urllib.request.urlopen(BASE + name, timeout=30).read()
    except Exception as exc:
        raise SystemExit(
            f"Cannot download {name}: {exc}\n"
            "Check internet access, or restore the file from the original repository."
        )
    target.write_bytes(data)

(out / "config.h").write_text(r'''#pragma once
#include <stdint.h>
#define RADIO_CC1101_ASYNC 1
#define BOARD_GENERIC_ESPHOME 1
#define RF_FREQ_MHZ 868.300f
#define RF_BITRATE_KBPS 38.4f
#define RF_DEVIATION_KHZ 50.78f
#define RF_RXBW_KHZ 312.0f
#define RF_PREAMBLE_BYTES 6
#define PIN_CC_SCK 18
#define PIN_CC_MOSI 23
#define PIN_CC_MISO 19
#define PIN_CC_NSS 5
#define PIN_CC_GDO0 2
#define PIN_CC_GDO2 4
#define DEV_US_CLASS 0x25
#define DEV_US_SERIAL 0x012345UL
#define DEV_FAN_CLASS 0x20
#define DEV_FAN_SERIAL 0x000000UL
#define DEV_BCAST_CLASS 0x3F
#define DEV_BCAST_SERIAL 0x3FFFFUL
#define DEBUG_RX_HEX 1
#define SEND_CO2_INTERVAL_MS (10UL*60UL*1000UL)
#define SEND_DEMAND_INTERVAL_MS (5UL*60UL*1000UL)
#define SEND_DEVINFO_INTERVAL_MS (24UL*3600UL*1000UL)
#define SEND_STATUS_INTERVAL_MS (60UL*1000UL)
''')
print("Vendor step complete.")
