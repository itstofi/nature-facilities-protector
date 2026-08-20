#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace nfp {

constexpr std::uint8_t kEnvironmentMessageType = 0x01;
constexpr std::size_t kLegacyPayloadSize = 13;
constexpr std::size_t kExtendedPayloadSize = 15;

struct EnvironmentReading {
  std::uint16_t temperatureCentiC;
  std::uint16_t humidityCentiPercent;
  std::uint32_t pressureCentiHpa;
  std::uint32_t gasResistanceOhm;
  std::uint16_t batteryMv;
};

template <std::size_t N>
inline void writeU16(std::array<std::uint8_t, N>& out, std::size_t offset,
                     std::uint16_t value) {
  out[offset] = static_cast<std::uint8_t>(value >> 8);
  out[offset + 1] = static_cast<std::uint8_t>(value);
}

template <std::size_t N>
inline void writeU32(std::array<std::uint8_t, N>& out, std::size_t offset,
                     std::uint32_t value) {
  out[offset] = static_cast<std::uint8_t>(value >> 24);
  out[offset + 1] = static_cast<std::uint8_t>(value >> 16);
  out[offset + 2] = static_cast<std::uint8_t>(value >> 8);
  out[offset + 3] = static_cast<std::uint8_t>(value);
}

template <std::size_t N>
inline std::uint16_t readU16(const std::array<std::uint8_t, N>& in, std::size_t offset) {
  return static_cast<std::uint16_t>((static_cast<std::uint16_t>(in[offset]) << 8) |
                                    in[offset + 1]);
}

template <std::size_t N>
inline std::uint32_t readU32(const std::array<std::uint8_t, N>& in, std::size_t offset) {
  return (static_cast<std::uint32_t>(in[offset]) << 24) |
         (static_cast<std::uint32_t>(in[offset + 1]) << 16) |
         (static_cast<std::uint32_t>(in[offset + 2]) << 8) |
         static_cast<std::uint32_t>(in[offset + 3]);
}

template <std::size_t N>
inline void encodeCommon(std::array<std::uint8_t, N>& payload,
                         const EnvironmentReading& reading) {
  payload[0] = kEnvironmentMessageType;
  writeU16(payload, 1, reading.temperatureCentiC);
  writeU16(payload, 3, reading.humidityCentiPercent);
  writeU32(payload, 5, reading.pressureCentiHpa);
  writeU32(payload, 9, reading.gasResistanceOhm);
}

inline std::array<std::uint8_t, kLegacyPayloadSize> encodeLegacyEnvironment(
    const EnvironmentReading& reading) {
  std::array<std::uint8_t, kLegacyPayloadSize> payload{};
  encodeCommon(payload, reading);
  return payload;
}

inline std::array<std::uint8_t, kExtendedPayloadSize> encodeExtendedEnvironment(
    const EnvironmentReading& reading) {
  std::array<std::uint8_t, kExtendedPayloadSize> payload{};
  encodeCommon(payload, reading);
  writeU16(payload, 13, reading.batteryMv);
  return payload;
}

template <std::size_t N>
inline bool decodeCommon(const std::array<std::uint8_t, N>& payload,
                         EnvironmentReading& reading) {
  if (payload[0] != kEnvironmentMessageType) return false;
  reading.temperatureCentiC = readU16(payload, 1);
  reading.humidityCentiPercent = readU16(payload, 3);
  reading.pressureCentiHpa = readU32(payload, 5);
  reading.gasResistanceOhm = readU32(payload, 9);
  return true;
}

inline bool decodeLegacyEnvironment(
    const std::array<std::uint8_t, kLegacyPayloadSize>& payload,
    EnvironmentReading& reading) {
  if (!decodeCommon(payload, reading)) return false;
  reading.batteryMv = 0;
  return true;
}

inline bool decodeExtendedEnvironment(
    const std::array<std::uint8_t, kExtendedPayloadSize>& payload,
    EnvironmentReading& reading) {
  if (!decodeCommon(payload, reading)) return false;
  reading.batteryMv = readU16(payload, 13);
  return true;
}

}  // namespace nfp
