from scripts.check_no_secrets import scan_text


def test_secret_scan_rejects_non_placeholder_lorawan_credentials() -> None:
    content = "uint8_t node" + "App" + "Key[16] = {0x01, 0x00, 0x00, 0x00};"

    assert scan_text("firmware.ino", content)


def test_secret_scan_rejects_project_prefixed_credentials() -> None:
    content = "constexpr uint8_t NFP_APP_" + "KEY[16] = {0x01, 0x00, 0x00, 0x00};"

    assert scan_text("credentials.h", content)


def test_secret_scan_accepts_zero_placeholder_credentials() -> None:
    content = "uint8_t nodeAppKey[16] = {0x00, 0x00, 0x00, 0x00};"

    assert scan_text("credentials.example.h", content) == []


def test_secret_scan_rejects_non_placeholder_setter() -> None:
    content = "LoRaWan." + "setApp" + 'Key("01 02 03 04")'

    assert scan_text("firmware.ino", content)


def test_secret_scan_accepts_zero_placeholder_setter() -> None:
    content = "LoRaWan." + "setApp" + 'Key("00 00 00 00")'

    assert scan_text("firmware.ino", content) == []


def test_secret_scan_rejects_common_cloud_token_shapes() -> None:
    aws = "AKIA" + "A" * 16
    twilio = "SK" + "a" * 32

    assert scan_text("config.html", f"{aws}\n{twilio}")
