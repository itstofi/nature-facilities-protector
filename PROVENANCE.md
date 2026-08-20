# Provenance and attribution

## Student project

**Nature and Facilities Protector** was developed by Mustafa Jweed as a 2024 Bachelor of Science
final-year project in Computer Engineering at the University of Nicosia, supervised by Dr Stelios
Neophytou.

The public repository is a security-clean reconstruction created from the private thesis and
working folder. The original thesis PDF is not redistributed because it contains historical
credentials/device identifiers in screenshots and incorporates third-party vendor figures.

## What is original in this repository

The following files were newly authored for this public release:

- the Python, JavaScript, and C++ payload codecs
- the test suites and credential scanner
- the sanitized reference firmware sketches
- the architecture diagram and case-study text
- the evaluation-summary dataset derived from ranges reported in the thesis

## Third-party hardware and libraries

The prototype used RAKwireless WisBlock hardware, including the RAK4631/RAK4630 core and modular
sensor boards. Product names are preserved accurately and are not rebranded.

The firmware references third-party Arduino libraries and board support, including:

- RAK4630/RAK4631 nRF52 board support and `LoRaWan-RAK4630`
- Adafruit BME680 and Adafruit Unified Sensor
- RAKwireless MQx library (`ADC121C021` interface)

Install and use those dependencies under their own licenses. They are not vendored here. See
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for upstream links and service acknowledgements.

## Historical examples excluded

The private working folder included vendor example sketches and documentation attributed to
RAKwireless contributors. Those files are not copied into this repository. The public firmware is
an original, reduced reference implementation that uses the documented hardware APIs and a
credential-free configuration boundary.

## Technical claims

Helium is described as a LoRaWAN connectivity network with blockchain-incentivized coverage. This
repository does **not** claim that environmental payloads are stored on-chain. Evaluation values
are reported as student-prototype observations, not independent certification.
