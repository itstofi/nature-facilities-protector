#!/usr/bin/env python3
"""Fail CI when source files contain non-placeholder credentials or private keys."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEXT_SUFFIXES = {
    ".c",
    ".cc",
    ".cff",
    ".cpp",
    ".csv",
    ".env",
    ".h",
    ".hpp",
    ".html",
    ".ino",
    ".js",
    ".json",
    ".md",
    ".py",
    ".toml",
    ".txt",
    ".yml",
    ".yaml",
}
CREDENTIAL_ARRAY = re.compile(
    r"(?is)\b(?:[a-z0-9]+_)*(?:node_?)?(?:app_?key|nws_?key|apps_?key|device_?eui|dev_?eui|app_?eui)"
    r"\s*\[[^]]*\]\s*=\s*\{([^}]*)\}"
)
HEX_BYTE = re.compile(r"0x([0-9a-fA-F]{2})")
SETTER_KEY = re.compile(r"(?i)\bset(?:AppKey|DevEui|AppEui)\s*\(\s*\"([^\"]+)\"")
PRIVATE_MARKERS = (
    "BEGIN PRIVATE KEY",
    '"private_key":',
    "TWILIO_AUTH_TOKEN=",
    "GOOGLE_APPLICATION_CREDENTIALS=",
)
TOKEN_PATTERNS = (
    ("AWS access key", re.compile(r"\bAKIA[0-9A-Z]{16}\b")),
    ("Twilio API key", re.compile(r"\bSK[0-9a-fA-F]{32}\b")),
    ("GitHub token", re.compile(r"\bgh[pousr]_[A-Za-z0-9]{20,}\b")),
)


def scan_text(path: str, content: str) -> list[str]:
    findings: list[str] = []
    for match in CREDENTIAL_ARRAY.finditer(content):
        values = [int(value, 16) for value in HEX_BYTE.findall(match.group(1))]
        if values and any(values):
            findings.append(f"{path}: non-placeholder LoRaWAN credential array")
    for match in SETTER_KEY.finditer(content):
        compact = re.sub(r"[^0-9a-fA-F]", "", match.group(1))
        if compact and set(compact) != {"0"} and "REPLACE" not in match.group(1).upper():
            findings.append(f"{path}: non-placeholder LoRaWAN credential setter")
    for marker in PRIVATE_MARKERS:
        if marker in content:
            findings.append(f"{path}: private credential marker {marker!r}")
    for label, pattern in TOKEN_PATTERNS:
        if pattern.search(content):
            findings.append(f"{path}: {label} shape")
    return findings


def main() -> int:
    findings: list[str] = []
    for path in ROOT.rglob("*"):
        relative = path.relative_to(ROOT)
        if relative == Path("scripts/check_no_secrets.py"):
            continue
        if not path.is_file() or ".git" in path.parts or path.suffix.lower() not in TEXT_SUFFIXES:
            continue
        findings.extend(scan_text(str(relative), path.read_text(errors="replace")))
    if findings:
        print("Credential scan failed:")
        for finding in findings:
            print(f"- {finding}")
        return 1
    print("Credential scan passed: no non-placeholder credentials found.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
