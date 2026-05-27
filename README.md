# AHT20 BLE Sensor Node for EFR32

Firmware project for a Silicon Labs EFR32 Bluetooth SoC node that reads AHT20 temperature and humidity data, shows live values on the onboard LCD, and broadcasts measurements through BLE advertising packets.

## Suggested Repository Name

`aht20-ble-sensor-node-efr32`

## Suggested Description

Silicon Labs EFR32 BLE sensor-node firmware for AHT20 temperature and humidity monitoring with LCD display and advertising-based telemetry.

## Overview

This repository now contains a single embedded project at the repository root.

The firmware:

- reads the AHT20 sensor over I2C every 1000 ms
- displays student IDs, device name, temperature, humidity, and period on the LCD
- advertises the latest sensor values through BLE manufacturer data
- restarts advertising automatically after disconnection

## Repository Structure

```text
.
|-- README.md
|-- .gitignore
|-- app.c
|-- app.h
|-- aht20.c
|-- aht20.h
|-- main.c
|-- final_cau2.slcp
|-- final_cau2.slps
|-- final_cau2.pintool
|-- config/
|-- autogen/
|-- image/
|-- simplicity_sdk_2024.6.2/
`-- GNU ARM v12.2.1 - Default/
```

## Key Files

- `app.c`: application logic for LCD rendering, periodic measurement, and BLE advertising
- `app.h`: node-level configuration such as `DEVICE_ID` and timing constants
- `aht20.c` / `aht20.h`: AHT20 sensor driver over I2C
- `main.c`: Silicon Labs super-loop entry point
- `final_cau2.slcp`: Simplicity Studio project configuration

## Node Configuration

The current root project is configured as:

- `DEVICE_ID = "AHT20_1"`

If you want to build the second node version, change `DEVICE_ID` in `app.h` to:

```c
#define DEVICE_ID "AHT20_2"
```

## Runtime Behavior

At startup, the device:

1. initializes the Silicon Labs platform and Bluetooth stack
2. initializes the LCD
3. initializes the AHT20 sensor
4. starts a periodic 1000 ms sleeptimer
5. creates and starts a BLE advertising set

During execution:

- the timer raises a measurement flag every second
- `app_process_action()` performs one sensor read when the flag is set
- the LCD is refreshed with the latest values
- the BLE advertising payload is updated with temperature and humidity data

## BLE Advertising Payload

The firmware uses legacy BLE advertising data containing:

- Flags
- Manufacturer Specific Data
- Complete Local Name

The manufacturer data includes:

- demo company ID bytes
- `float temperature`
- `float humidity`

This allows a BLE scanner to receive measurements without opening a GATT connection.

## Hardware/Software Stack

- Silicon Labs EFR32 Bluetooth SoC platform
- Simplicity Studio / Simplicity SDK `2024.6.2`
- AHT20 temperature and humidity sensor
- GLIB + DMD LCD libraries
- Sleeptimer for periodic sampling

## Build and Flash

1. Open Simplicity Studio.
2. Import `final_cau2.slcp`.
3. Let the project regenerate files if prompted.
4. Build the project.
5. Flash the generated `.hex` or `.s37` file to the target board.

## Notes and Limitations

- `sensor_protocol.h` is currently commented out and not used by the firmware.
- UART command handling and dynamic period updates are intentionally not included in this version.
- BLE manufacturer data currently stores raw `float` bytes, so any scanner/parser must decode them with the same byte layout.
- The included build directory is useful for reference, but rebuilding from Simplicity Studio is still recommended.

## Next Improvements

- add a small BLE scanner script to decode advertised values
- replace demo manufacturer ID with a real assigned company ID if needed
- add a short wiring and board setup section
- document how to switch quickly between `AHT20_1` and `AHT20_2`
