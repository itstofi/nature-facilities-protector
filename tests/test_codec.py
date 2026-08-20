import pytest

from nfp.codec import (
    EnvironmentReading,
    decode_environment,
    encode_extended_environment,
    encode_legacy_environment,
)

LEGACY_VECTOR = bytes.fromhex("010b0913880001767100018836")
EXTENDED_VECTOR = bytes.fromhex("010b09138800017671000188360f0a")
READING = EnvironmentReading(28.25, 50.0, 958.57, 100_406)


def test_decode_historical_13_byte_vector() -> None:
    assert decode_environment(LEGACY_VECTOR) == READING


def test_decode_historical_15_byte_battery_extension() -> None:
    assert decode_environment(EXTENDED_VECTOR) == EnvironmentReading(
        28.25, 50.0, 958.57, 100_406, 3_850
    )


def test_encode_legacy_matches_historical_vector() -> None:
    assert encode_legacy_environment(READING) == LEGACY_VECTOR


def test_encode_extended_preserves_legacy_prefix() -> None:
    reading = EnvironmentReading(28.25, 50.0, 958.57, 100_406, 3_850)

    encoded = encode_extended_environment(reading)

    assert encoded == EXTENDED_VECTOR
    assert encoded[:13] == encode_legacy_environment(reading)


def test_legacy_compatibility_rejects_negative_temperature() -> None:
    reading = EnvironmentReading(-5.25, 82.1, 1_012.34, 245_000)

    with pytest.raises(ValueError, match="temperature_c"):
        encode_legacy_environment(reading)


@pytest.mark.parametrize(
    "payload",
    [
        b"",
        bytes.fromhex("02" + "00" * 12),
        bytes.fromhex("01" + "00" * 11),
        bytes.fromhex("01" + "00" * 13),
        bytes.fromhex("01" + "00" * 15),
    ],
)
def test_decode_rejects_wrong_message_or_length(payload: bytes) -> None:
    with pytest.raises(ValueError):
        decode_environment(payload)


def test_decode_rejects_semantically_invalid_frames() -> None:
    for payload in (
        bytes.fromhex("01" + "00" * 12),
        bytes.fromhex("01" + "ff" * 12),
    ):
        with pytest.raises(ValueError, match="outside supported sensor bounds"):
            decode_environment(payload)


def test_extended_encoder_requires_battery() -> None:
    with pytest.raises(ValueError, match="battery_mv"):
        encode_extended_environment(READING)


@pytest.mark.parametrize(
    "reading",
    [
        EnvironmentReading(700.0, 50.0, 1_000.0, 1),
        EnvironmentReading(20.0, 100.01, 1_000.0, 1),
        EnvironmentReading(20.0, 50.0, -1.0, 1),
        EnvironmentReading(20.0, 50.0, 1_000.0, -1),
    ],
)
def test_legacy_encoder_rejects_out_of_range_values(reading: EnvironmentReading) -> None:
    with pytest.raises(ValueError):
        encode_legacy_environment(reading)
