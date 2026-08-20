"""Load and render thesis-reported ranges without external dependencies."""

from __future__ import annotations

import csv
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class EvaluationSummary:
    environment: str
    temperature_range_c: tuple[float, float]
    humidity_range_percent: tuple[float, float]
    pressure_range_hpa: tuple[float, float]
    gas_resistance_range_ohm: tuple[int, int]
    notes: str


def load_summaries(path: Path) -> list[EvaluationSummary]:
    with path.open(newline="", encoding="utf-8") as handle:
        rows = csv.DictReader(handle)
        return [
            EvaluationSummary(
                environment=row["environment"],
                temperature_range_c=(float(row["temp_min_c"]), float(row["temp_max_c"])),
                humidity_range_percent=(
                    float(row["humidity_min_percent"]),
                    float(row["humidity_max_percent"]),
                ),
                pressure_range_hpa=(
                    float(row["pressure_min_hpa"]),
                    float(row["pressure_max_hpa"]),
                ),
                gas_resistance_range_ohm=(
                    int(row["gas_min_ohm"]),
                    int(row["gas_max_ohm"]),
                ),
                notes=row["notes"],
            )
            for row in rows
        ]


def _range(low: float | int, high: float | int, unit: str) -> str:
    if low == high:
        value = f"{low:,}" if isinstance(low, int) else f"{low:g}"
        return f"{value} {unit}"
    if isinstance(low, int) and isinstance(high, int):
        return f"{low:,}-{high:,} {unit}"
    return f"{low:g}-{high:g} {unit}"


def render_markdown(summaries: list[EvaluationSummary]) -> str:
    lines = [
        "| Environment | Temperature | Humidity | Pressure | Gas resistance | Notes |",
        "| --- | ---: | ---: | ---: | ---: | --- |",
    ]
    for item in summaries:
        lines.append(
            f"| {item.environment} | "
            f"{_range(*item.temperature_range_c, '°C')} | "
            f"{_range(*item.humidity_range_percent, '%')} | "
            f"{_range(*item.pressure_range_hpa, 'hPa')} | "
            f"{_range(*item.gas_resistance_range_ohm, 'Ω')} | {item.notes} |"
        )
    return "\n".join(lines)
