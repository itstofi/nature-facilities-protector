from pathlib import Path

from nfp.evaluation import load_summaries, render_markdown


def test_evaluation_summary_preserves_reported_ranges(tmp_path: Path) -> None:
    path = tmp_path / "summary.csv"
    path.write_text(
        "environment,temp_min_c,temp_max_c,humidity_min_percent,humidity_max_percent,"
        "pressure_min_hpa,pressure_max_hpa,gas_min_ohm,gas_max_ohm,notes\n"
        "indoor,36.81,38.03,15,38,973.64,976.11,99191,99191,humidity outlier observed\n"
    )

    summaries = load_summaries(path)

    assert len(summaries) == 1
    assert summaries[0].environment == "indoor"
    assert summaries[0].pressure_range_hpa == (973.64, 976.11)
    assert summaries[0].gas_resistance_range_ohm == (99_191, 99_191)
    rendered = render_markdown(summaries)
    assert "36.81-38.03 °C" in rendered
    assert "99,191 Ω" in rendered
    assert "humidity outlier observed" in rendered


def test_repository_evaluation_summary_contains_three_environments() -> None:
    summaries = load_summaries(Path("data/evaluation_summary.csv"))

    assert [item.environment for item in summaries] == ["indoor", "urban-outdoor", "forest"]
    assert summaries[1].gas_resistance_range_ohm == (3_458_210, 3_716_250)
