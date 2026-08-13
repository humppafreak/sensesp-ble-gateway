# SensESP BLE Gateway

BLE gateway library for [SensESP](https://github.com/SignalK/SensESP) that bridges Bluetooth Low Energy devices to [signalk-server](https://github.com/SignalK/signalk-server)'s BLE provider API.

## Features

- **BLE scanning** with advertisements forwarded to signalk-server via HTTP POST
- **GATT client** support (connect, discover, subscribe, read, write) on Bluedroid targets
- **Control WebSocket** for gateway metadata (hello, status) and GATT commands
- **4-level scan watchdog** for esp_hosted targets (restart, RPC reset, GPIO hard-reset, reboot)
- **Multiple BT stack support**:
  - `EspHostedBluedroidBLE` — ESP32-P4 + C6 companion via esp_hosted SDIO
  - `NativeBLE` — native Bluedroid on ESP32, ESP32-C3, ESP32-S3, etc.
  - `NimBLEProvisioner` — NimBLE for memory-constrained chips (ESP32-C5 with WiFi)

## Hardware Tested

| Board | BT Stack | Network | Status |
|-------|----------|---------|--------|
| Waveshare ESP32-P4-WIFI6-POE-ETH | Bluedroid + esp_hosted | Ethernet | Full (WS + GATT) |
| Waveshare ESP32-C5-WIFI6-KIT | NimBLE | WiFi | POST-only (WS disabled for RAM) |

## Quick Start

Add to your `platformio.ini`:

```ini
lib_deps =
    SignalK/SensESP@>=3.0.0
    https://github.com/dirkwa/sensesp-ble-gateway.git
```

### P4 Example (Bluedroid + Ethernet)

```cpp
#include "sensesp_ble_gateway/ble_signalk_gateway.h"
#include "sensesp_ble_gateway/esp_hosted_bluedroid_ble.h"

auto ble = std::make_shared<EspHostedBluedroidBLE>();
auto gateway = std::make_shared<BLESignalKGateway>(ble, app->get_ws_client());
gateway->start();
```

### C5 Example (NimBLE + WiFi)

```cpp
#include "sensesp_ble_gateway/ble_signalk_gateway.h"
#include "sensesp_ble_gateway/nimble_ble.h"

auto ble = std::make_shared<NimBLEProvisioner>();
BLESignalKGatewayConfig gw_cfg;
gw_cfg.enable_control_ws = false;  // Save RAM
auto gateway = std::make_shared<BLESignalKGateway>(ble, app->get_ws_client(), gw_cfg);
gateway->start();
```

See the [examples/](examples/) directory for complete working firmware.

## Flashing a Release

Every [published release](../../releases) has a merged, single-file firmware
binary attached for each example (`sensesp-ble-gateway-<example>-merged.bin`),
built by [`.github/workflows/release-firmware.yml`](.github/workflows/release-firmware.yml).
These already contain the bootloader, partition table, and app image at their
correct addresses — flash the whole file starting at **offset `0x0`**, not at
the offset you'd use for a bare `bootloader.bin` or `firmware.bin`:

```
esptool.py --chip <chip> erase_flash
esptool.py --chip <chip> write_flash 0x0 sensesp-ble-gateway-<example>-merged.bin
```

Use the matching `--chip` for the example you're flashing: `esp32p4` for
`p4_ble_gateway`, `esp32c5` for `c5_ble_gateway`, `esp32` for
`esp32_ble_gateway`. Flashing at the wrong offset (e.g. `0x1000`, where a
bare bootloader normally goes) shifts every section one page later than the
ROM bootloader expects and shows up as `flash read err, 1000` on the serial
console at boot.

## Requirements

- SensESP >= 3.3.0 (needs `SKWSClient::get_auth_token()`, hostname persistence fix, and stale polling href fix — all merged into main, pending next release; until then use `https://github.com/SignalK/SensESP.git#main` in `lib_deps`)
- signalk-server with BLE provider API (branch `ble-provider-api`)
- PlatformIO with `framework = espidf, arduino` (pioarduino)

## Note on the ESP32-P4 C6 Antenna

The Waveshare ESP32-P4-WIFI6-POE-ETH uses the ESP32-C6-MINI-**1U** module which has **no built-in PCB antenna**. You must connect an external 2.4 GHz antenna to the IPEX connector for BLE to work.
