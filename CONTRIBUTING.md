# Contributing

Thank you for improving this academic IoT reference project.

## Before opening a pull request

1. Do not add real LoRaWAN keys, EUIs, account screenshots, phone numbers, Sheet identifiers,
   service-account files, private endpoints, or precise deployment locations.
2. Preserve the historical 13-byte and 15-byte payload formats. Any incompatible format requires a
   new marker/version and its own golden vectors.
3. Add tests before changing codec, decoder, evaluation, or secret-scanning behavior.
4. Run the complete local gate:

```bash
python -m pip install -r requirements-dev.txt
make verify
```

5. If firmware changes, compile both RAK4631 sketches with the pinned versions documented in CI.
6. Mark hardware-only validation honestly. A successful compile does not prove RF, sensor,
   calibration, battery, solar, enclosure, or alert behavior.

## Pull-request expectations

- Explain the user-visible or interoperability effect.
- Include test evidence and any hardware model/library versions used.
- Update documentation when changing packet fields or setup.
- Keep vendor names, notices, and upstream license references accurate.
- Never present this prototype as a certified fire, smoke, gas, or life-safety product.
