# JavaScript payload decoder

`decoder.js` accepts both environment frames documented in the student working folder:

- 13-byte historical BME680 frame
- 15-byte extension with battery millivolts

```js
const { decodeEnvironment } = require('./decoder');
const reading = decodeEnvironment('010b09138800017671000188360f0a');
```

The layout is big-endian:

| Offset | Bytes | Field | Scale |
| ---: | ---: | --- | ---: |
| 0 | 1 | message type (`0x01`) | — |
| 1 | 2 | unsigned temperature | ÷100 °C |
| 3 | 2 | relative humidity | ÷100 % |
| 5 | 4 | pressure | ÷100 hPa |
| 9 | 4 | gas resistance | Ω |
| 13 | 2 | optional battery voltage | mV |

Both decoders reject incorrect lengths/markers and values outside documented sensor bounds:
0–85 °C, 0–100% RH, 300–1100 hPa, 1–100,000,000 Ω, and (when present) 0–6,000 mV.

Run `node decoder/decoder.test.js` to verify both cross-language vectors and malformed-input
handling.
