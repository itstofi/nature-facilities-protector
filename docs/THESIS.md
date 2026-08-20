# Thesis and case-study note

The private source document is:

> Mustafa Jweed, *Nature and Facilities Protector*, Bachelor of Science Final Year Project,
> Department of Engineering, University of Nicosia, 2024.

The public repository provides an abridged, security-clean case study rather than the raw thesis.
The raw PDF includes historical LoRaWAN/device identifiers, local service paths, and third-party
vendor figures. Publishing it unchanged would create credential, privacy, and provenance risks.

See:

- [`case-study.pdf`](case-study.pdf) — sanitized public summary
- [`case-study.html`](case-study.html) — accessible source for the PDF
- [`architecture.html`](architecture.html) — self-contained system diagram
- [`../data/evaluation_summary.csv`](../data/evaluation_summary.csv) — thesis-reported ranges

The two images under `docs/images/` are cropped serial-monitor outputs from student bench tests.
The crops intentionally exclude source-code panes, vendor sketch names, account information,
network identifiers, and personal data. They are evidence of acquisition-loop output only—not
sensor-accuracy or safety-validation results.
