# Security and credential handling

## No production credentials

This public reconstruction intentionally contains no working LoRaWAN, Helium, Google service
account, Twilio, phone, or API credentials. `credentials.example.h` contains zero-byte
placeholders and the real `credentials.h` is ignored by Git.

Historical student sketches contained device and session credentials. Those values are **not** in
this repository and should be rotated before any original node or network application is reused.
The original thesis PDF is also withheld because some screenshots contain device identifiers,
local paths, and service configuration.

Run the repository scanner before every commit:

```bash
python scripts/check_no_secrets.py
```

CI runs the same check. The scanner blocks non-zero LoRaWAN credential arrays/setters and common
private-key/service-account markers. Secret scanning and push protection should remain enabled on
the GitHub repository.

## Prototype threat model

This is an academic environmental-monitoring prototype. It is not a certified smoke detector,
fire alarm, gas detector, or life-safety device. Thresholds, sensor calibration, enclosure design,
network availability, power behavior, alert delivery, and false-positive/false-negative rates all
require professional validation before field deployment.

## Hardware safety

- Attach the correct antenna before powering a LoRa radio.
- Verify battery polarity and supported battery/solar voltages.
- Treat MQ-2 heater/sensor calibration as hardware- and gas-specific.
- Do not use the sample 3,000 ppm alarm threshold as a regulatory or safety limit.

## Reporting

Report vulnerabilities privately to the repository owner. Do not open a public issue containing
credentials, device identifiers, service-account files, phone numbers, or private deployment data.
