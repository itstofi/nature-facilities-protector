# Firmware reference

These sketches are a clean, credential-free reconstruction of the 2024 academic prototype.
They target the **RAK4631 / RAK4630 WisBlock Core** and are compiled in CI against pinned RAK
board support and library versions. Compilation does not replace physical sensor, radio, power,
or calibration testing.

## Environment + LoRaWAN node

`nfp_environment_node/nfp_environment_node.ino` reads a BME680, encodes the 15-byte battery
extension specified in [`nfp_environment_node/nfp_payload.h`](nfp_environment_node/nfp_payload.h),
and sends it over LoRaWAN every five minutes. The same header also implements the historical
13-byte frame for decoder compatibility.

Required Arduino libraries / board support:

- RAK4630/RAK4631 nRF52 board support
- `LoRaWan-RAK4630`
- Adafruit BME680
- Adafruit Unified Sensor

Copy `credentials.example.h` to `credentials.h` and configure credentials locally. The real file
is ignored by Git. The sketch refuses to join when the application key remains all zeroes.

## MQ-2 smoke alarm

`nfp_smoke_alarm/nfp_smoke_alarm.ino` demonstrates clean-air calibration, PPM sampling, and a
local alarm threshold using the RAK12004 / ADC121C021 interface.

Required libraries:

- `RAKwireless MQx library`
- RAK4631 board support

The 3,000 ppm threshold is preserved from the student prototype for demonstration. It is **not a
certified fire-safety threshold** and must be calibrated and validated for the target gas,
enclosure, environment, and applicable safety standard.

## Hardware note

Never power a radio board without its antenna attached. Verify battery polarity and supported
battery/solar voltage before connecting hardware.
