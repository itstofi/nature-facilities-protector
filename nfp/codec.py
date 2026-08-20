"""Strict codecs for the thesis's legacy and battery-extended payloads."""

from __future__ import annotations

import math
import struct
from dataclasses import dataclass

MESSAGE_TYPE_ENVIRONMENT = 0x01
LEGACY_PAYLOAD_SIZE = 13
EXTENDED_PAYLOAD_SIZE = 15
_LEGACY = struct.Struct(">BHHII")
_EXTENDED = struct.Struct(">BHHIIH")


@dataclass(frozen=True)
class EnvironmentReading:
    temperature_c: float
    humidity_percent: float
    pressure_hpa: float
    gas_resistance_ohm: int
    battery_mv: int | None = None


def _scaled(value: float, factor: int, maximum: int, name: str) -> int:
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ValueError(f"{name} must be a finite number")
    if not math.isfinite(float(value)):
        raise ValueError(f"{name} must be a finite number")
    result = round(float(value) * factor)
    if not 0 <= result <= maximum:
        raise ValueError(f"{name} is outside the historical unsigned range")
    return result


def _unsigned_integer(value: int | None, minimum: int, maximum: int, name: str) -> int:
    if value is None or isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ValueError(f"{name} must be an integer")
    if (
        not math.isfinite(float(value))
        or int(value) != value
        or not minimum <= int(value) <= maximum
    ):
        raise ValueError(f"{name} is outside the encodable range")
    return int(value)


def _validate_wire(temperature: int, humidity: int, pressure: int, gas: int) -> None:
    if not (
        0 <= temperature <= 8_500
        and 0 <= humidity <= 10_000
        and 30_000 <= pressure <= 110_000
        and 1 <= gas <= 100_000_000
    ):
        raise ValueError("environment reading is outside supported sensor bounds")


def _fields(reading: EnvironmentReading) -> tuple[int, int, int, int]:
    fields = (
        _scaled(reading.temperature_c, 100, 8_500, "temperature_c"),
        _scaled(reading.humidity_percent, 100, 10_000, "humidity_percent"),
        _scaled(reading.pressure_hpa, 100, 110_000, "pressure_hpa"),
        _unsigned_integer(reading.gas_resistance_ohm, 1, 100_000_000, "gas_resistance_ohm"),
    )
    _validate_wire(*fields)
    return fields


def encode_legacy_environment(reading: EnvironmentReading) -> bytes:
    """Encode the historical 13-byte, unsigned-temperature BME680 frame."""
    return _LEGACY.pack(MESSAGE_TYPE_ENVIRONMENT, *_fields(reading))


def encode_extended_environment(reading: EnvironmentReading) -> bytes:
    """Encode the related 15-byte frame that appends battery millivolts."""
    battery = _unsigned_integer(reading.battery_mv, 0, 6_000, "battery_mv")
    return _EXTENDED.pack(MESSAGE_TYPE_ENVIRONMENT, *_fields(reading), battery)


def decode_environment(payload: bytes | bytearray | memoryview) -> EnvironmentReading:
    """Decode an exact 13-byte legacy or 15-byte battery-extended frame."""
    raw = bytes(payload)
    if len(raw) == LEGACY_PAYLOAD_SIZE:
        message_type, temperature, humidity, pressure, gas = _LEGACY.unpack(raw)
        battery = None
    elif len(raw) == EXTENDED_PAYLOAD_SIZE:
        message_type, temperature, humidity, pressure, gas, battery = _EXTENDED.unpack(raw)
    else:
        raise ValueError("environment payload must be exactly 13 or 15 bytes")
    if message_type != MESSAGE_TYPE_ENVIRONMENT:
        raise ValueError(f"unsupported message type: {message_type}")
    _validate_wire(temperature, humidity, pressure, gas)
    if battery is not None and not 0 <= battery <= 6_000:
        raise ValueError("battery voltage is outside supported sensor bounds")
    return EnvironmentReading(
        temperature_c=temperature / 100,
        humidity_percent=humidity / 100,
        pressure_hpa=pressure / 100,
        gas_resistance_ohm=gas,
        battery_mv=battery,
    )
