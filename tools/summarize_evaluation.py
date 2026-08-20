#!/usr/bin/env python3
"""Print the thesis evaluation summary as a Markdown table."""

from pathlib import Path

from nfp.evaluation import load_summaries, render_markdown

if __name__ == "__main__":
    print(render_markdown(load_summaries(Path("data/evaluation_summary.csv"))))
