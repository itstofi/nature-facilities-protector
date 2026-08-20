#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "nfp_payload.h"

int main() {
  nfp::EnvironmentReading reading{2825, 5000, 95857, 100406, 3850};
  const std::array<std::uint8_t, nfp::kLegacyPayloadSize> expectedLegacy{
      0x01, 0x0B, 0x09, 0x13, 0x88, 0x00, 0x01,
      0x76, 0x71, 0x00, 0x01, 0x88, 0x36};
  const std::array<std::uint8_t, nfp::kExtendedPayloadSize> expectedExtended{
      0x01, 0x0B, 0x09, 0x13, 0x88, 0x00, 0x01, 0x76,
      0x71, 0x00, 0x01, 0x88, 0x36, 0x0F, 0x0A};

  std::array<std::uint8_t, nfp::kLegacyPayloadSize> encodedLegacy{};
  std::array<std::uint8_t, nfp::kExtendedPayloadSize> encodedExtended{};
  assert(nfp::encodeLegacyEnvironment(reading, encodedLegacy));
  assert(encodedLegacy == expectedLegacy);
  assert(nfp::encodeExtendedEnvironment(reading, encodedExtended));
  assert(encodedExtended == expectedExtended);

  nfp::EnvironmentReading decoded{};
  assert(nfp::decodeLegacyEnvironment(expectedLegacy, decoded));
  assert(decoded.temperatureCentiC == reading.temperatureCentiC);
  assert(decoded.batteryMv == 0);
  assert(nfp::decodeExtendedEnvironment(expectedExtended, decoded));
  assert(decoded.batteryMv == reading.batteryMv);

  auto wrongType = expectedLegacy;
  wrongType[0] = 0x02;
  assert(!nfp::decodeLegacyEnvironment(wrongType, decoded));

  auto zeroFrame = expectedLegacy;
  zeroFrame.fill(0);
  zeroFrame[0] = nfp::kEnvironmentMessageType;
  assert(!nfp::decodeLegacyEnvironment(zeroFrame, decoded));

  nfp::EnvironmentReading invalid{8501, 5000, 95857, 100406, 3850};
  assert(!nfp::encodeLegacyEnvironment(invalid, encodedLegacy));

  std::cout << "C++ payload tests passed.\n";
  return 0;
}
