#!/usr/bin/env python3
"""
Parse exprtk_benchmark-style markdown logs and summarize Base vs Optimized rounds.

Expected sections (per round):
  ### Base N / ### Optimized N
  --- EXPRTK EVAL --- / --- NATIVE EVAL --- / --- PARSE ----
  [exprtk|native|parse] Total Time: ... Rate: ... evals/sec|parse/sec Expression: ...

Usage:
  python3 scripts/benchmark_stats.py docs/benchmark_arena_allocator.md
  python3 scripts/benchmark_stats.py path/to/other_run.md --format markdown --output report.md
"""

from __future__ import annotations

import argparse
import math
import re
import statistics
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

SECTION_ROUND = re.compile(r"^###\s+(base|optimized)\s+(\d+)\s*$", re.IGNORECASE)
PHASE_EXPRTK = re.compile(r"^---\s*EXPRTK\s+EVAL\s*---\s*$", re.IGNORECASE)
PHASE_NATIVE = re.compile(r"^---\s*NATIVE\s+EVAL\s*---\s*$", re.IGNORECASE)
PHASE_PARSE = re.compile(r"^---\s*PARSE", re.IGNORECASE)
DATA_LINE = re.compile(
    r"^\[(exprtk|native|parse)\]\s+Total Time:\s+([\d.]+)\s+Rate:\s+([\d.]+)\s+"
    r"(evals/sec|parse/sec)\s+Expression:\s*(.+?)\s*$"
)

PHASE_NAMES = ("exprtk", "native", "parse")
PHASE_LABEL = {"exprtk": "EXPRTK eval", "native": "NATIVE eval", "parse": "PARSE"}


@dataclass
class Round:
    label: str
    run: int
    data: dict[str, dict[str, float]]


def parse_benchmark_md(path: Path) -> list[Round]:
    rounds: list[Round] = []
    current: Round | None = None
    phase: str | None = None

    text = path.read_text(encoding="utf-8", errors="replace")
    for line in text.splitlines():
        m = SECTION_ROUND.match(line.strip())
        if m:
            if current is not None:
                rounds.append(current)
            g1 = m.group(1).lower()
            label = "Optimized" if g1 == "optimized" else "Base"
            current = Round(
                label=label,
                run=int(m.group(2)),
                data={p: {} for p in PHASE_NAMES},
            )
            phase = None
            continue

        if current is None:
            continue

        if PHASE_EXPRTK.match(line.strip()):
            phase = "exprtk"
            continue
        if PHASE_NATIVE.match(line.strip()):
            phase = "native"
            continue
        if PHASE_PARSE.match(line.strip()):
            phase = "parse"
            continue

        m = DATA_LINE.match(line.rstrip())
        if m and phase:
            tag, _time_s, rate_s, _unit, expr = m.groups()
            if tag != phase:
                continue
            expr_key = expr.strip()
            current.data[phase][expr_key] = float(rate_s)

    if current is not None:
        rounds.append(current)

    return rounds


def _pct_delta(base_mean: float, opt_mean: float) -> float | None:
    if base_mean == 0:
        return None
    return 100.0 * (opt_mean - base_mean) / base_mean


def _geom_mean_ratios(base: list[float], opt: list[float]) -> float | None:
    """Geometric mean of per-run ratios opt/base (needs aligned length)."""
    if len(base) != len(opt) or not base:
        return None
    ratios = []
    for b, o in zip(base, opt):
        if b == 0:
            return None
        ratios.append(o / b)
    if not ratios:
        return None
    log_sum = sum(math.log(r) for r in ratios)
    return math.exp(log_sum / len(ratios))


def aligned_base_opt_rates(
    rounds: list[Round], phase: str, expr: str
) -> tuple[list[float], list[float]] | None:
    """Match Base N with Optimized N by run number when both exist."""
    base_map = {r.run: r.data[phase][expr] for r in rounds if r.label == "Base" and expr in r.data[phase]}
    opt_map = {
        r.run: r.data[phase][expr]
        for r in rounds
        if r.label == "Optimized" and expr in r.data[phase]
    }
    runs = sorted(set(base_map) & set(opt_map))
    if not runs:
        return None
    return [base_map[i] for i in runs], [opt_map[i] for i in runs]


def all_expressions(rounds: list[Round], phase: str) -> list[str]:
    seen: set[str] = set()
    order: list[str] = []
    for r in rounds:
        for e in r.data[phase]:
            if e not in seen:
                seen.add(e)
                order.append(e)
    return order


def build_rows(
    rounds: list[Round], phase: str, expr: str
) -> dict[str, Any] | None:
    base_vals = [r.data[phase][expr] for r in rounds if r.label == "Base" and expr in r.data[phase]]
    opt_vals = [
        r.data[phase][expr] for r in rounds if r.label == "Optimized" and expr in r.data[phase]
    ]
    if not base_vals and not opt_vals:
        return None
    row: dict[str, Any] = {"expression": expr}
    if base_vals:
        row["base_n"] = len(base_vals)
        row["base_mean"] = statistics.mean(base_vals)
        row["base_median"] = statistics.median(base_vals)
    else:
        row["base_n"] = 0
        row["base_mean"] = None
    if opt_vals:
        row["opt_n"] = len(opt_vals)
        row["opt_mean"] = statistics.mean(opt_vals)
        row["opt_median"] = statistics.median(opt_vals)
    else:
        row["opt_n"] = 0
        row["opt_mean"] = None

    aligned = aligned_base_opt_rates(rounds, phase, expr)
    if aligned and row.get("base_mean") is not None and row.get("opt_mean") is not None:
        b, o = aligned
        row["paired_n"] = len(b)
        gm = _geom_mean_ratios(b, o)
        row["geom_mean_ratio"] = gm
        if gm is not None:
            row["geom_mean_pct_vs_base"] = 100.0 * (gm - 1.0)
        row["mean_pct_vs_base"] = _pct_delta(row["base_mean"], row["opt_mean"])
    else:
        row["paired_n"] = 0
        row["geom_mean_ratio"] = None
        row["geom_mean_pct_vs_base"] = None
        if row.get("base_mean") is not None and row.get("opt_mean") is not None:
            row["mean_pct_vs_base"] = _pct_delta(row["base_mean"], row["opt_mean"])
        else:
            row["mean_pct_vs_base"] = None

    return row


def format_human(rows: list[dict[str, Any]], phase: str, source: Path) -> str:
    lines = [
        f"Source: {source}",
        f"Phase: {PHASE_LABEL[phase]}",
        "",
        f"{'Expression':<58} {'n':>5} {'base μ':>14} {'opt μ':>14} {'Δmean%':>10} {'Δgeom%':>10}",
        "-" * 115,
    ]
    for row in rows:
        ex = row["expression"]
        if len(ex) > 57:
            ex = ex[:54] + "..."
        n = row.get("paired_n") or min(row.get("base_n", 0), row.get("opt_n", 0))
        bm = row.get("base_mean")
        om = row.get("opt_mean")
        dmean = row.get("mean_pct_vs_base")
        dgeom = row.get("geom_mean_pct_vs_base")
        lines.append(
            f"{ex:<58} {n:5d} "
            f"{(bm if bm is not None else float('nan')):14.4g} "
            f"{(om if om is not None else float('nan')):14.4g} "
            f"{(dmean if dmean is not None else float('nan')):10.2f} "
            f"{(dgeom if dgeom is not None else float('nan')):10.2f}"
        )
    lines.append("")
    return "\n".join(lines)


def _fmt_val(x: float | None) -> str:
    """Format rate / mean for markdown tables."""
    if x is None:
        return "—"
    return f"{x:.8g}"


def _fmt_pct(x: float | None) -> str:
    if x is None:
        return "—"
    return f"{x:.2f}"


def format_markdown(rows: list[dict[str, Any]], phase: str, source: Path) -> str:
    lines = [
        f"## {PHASE_LABEL[phase]}",
        "",
        f"Source: `{source}`",
        "",
        "| Expression | n | base μ | opt μ | Δ mean % | Δ geom % |",
        "|---|--:|--:|--:|--:|--:|",
    ]
    for row in rows:
        bm = row.get("base_mean")
        om = row.get("opt_mean")
        d1 = row.get("mean_pct_vs_base")
        d2 = row.get("geom_mean_pct_vs_base")
        ex = row["expression"].replace("|", "\\|")
        lines.append(
            f"| {ex} | {row.get('paired_n', 0)} | "
            f"{_fmt_val(bm)} | {_fmt_val(om)} | "
            f"{_fmt_pct(d1)} | {_fmt_pct(d2)} |"
        )
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Summarize Base vs Optimized benchmark markdown.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Examples:\n"
        "  %(prog)s docs/benchmark_arena_allocator.md\n"
        "  %(prog)s run.md --format markdown -o run_report.md\n"
        "  %(prog)s run.md --phase parse --phase exprtk\n",
    )
    ap.add_argument("markdown", type=Path, help="Path to benchmark .md log")
    ap.add_argument(
        "--format",
        choices=("text", "markdown"),
        default="text",
        help="Output style (default: text)",
    )
    ap.add_argument("-o", "--output", type=Path, default=None, help="Write report to file")
    ap.add_argument(
        "--phase",
        choices=PHASE_NAMES,
        action="append",
        default=None,
        help="Only include this phase (repeatable). Default: all",
    )
    args = ap.parse_args()
    path = args.markdown.resolve()
    if not path.is_file():
        print(f"error: not a file: {path}", file=sys.stderr)
        return 1

    rounds = parse_benchmark_md(path)
    if not rounds:
        print("error: no ### Base/Optimized rounds found", file=sys.stderr)
        return 1

    base_runs = sum(1 for r in rounds if r.label == "Base")
    opt_runs = sum(1 for r in rounds if r.label == "Optimized")
    phases = args.phase or list(PHASE_NAMES)

    sections: list[str] = []
    header = [
        f"# Benchmark statistics",
        "",
        f"Rounds: Base={base_runs}, Optimized={opt_runs}",
        "",
    ]
    if args.format == "markdown":
        sections.append("\n".join(header))
    else:
        sections.append("\n".join(header).lstrip("# "))

    for phase in phases:
        exprs = all_expressions(rounds, phase)
        rows = []
        for e in exprs:
            row = build_rows(rounds, phase, e)
            if row is not None:
                rows.append(row)
        if not rows:
            continue
        if args.format == "markdown":
            sections.append(format_markdown(rows, phase, path))
        else:
            sections.append(format_human(rows, phase, path))

    report = "\n".join(sections)

    # Cross-phase summary: median of geom_mean_pct where paired
    if len(phases) > 1:
        summary_lines: list[str] = []
        if args.format == "markdown":
            summary_lines.append("## Summary")
            summary_lines.append("")
            summary_lines.append("Median Δ geom % (paired runs), by phase:")
            summary_lines.append("")
            summary_lines.append("| Phase | median Δ geom % | expressions |")
            summary_lines.append("|---|--:|--:|")
        else:
            summary_lines.append("Summary (paired geom-mean % change, by phase):")
        for phase in phases:
            exprs = all_expressions(rounds, phase)
            pcts = []
            for e in exprs:
                row = build_rows(rounds, phase, e)
                if row and row.get("geom_mean_pct_vs_base") is not None:
                    pcts.append(row["geom_mean_pct_vs_base"])
            if pcts:
                med = statistics.median(pcts)
                if args.format == "markdown":
                    summary_lines.append(
                        f"| {PHASE_LABEL[phase]} | {_fmt_pct(med)} | {len(pcts)} |"
                    )
                else:
                    summary_lines.append(
                        f"  {PHASE_LABEL[phase]}: median Δgeom% = {med:.2f} "
                        f"(over {len(pcts)} expressions)"
                    )
        if summary_lines:
            report = report + "\n" + "\n".join(summary_lines) + "\n"

    if args.output:
        args.output.write_text(report, encoding="utf-8")
        print(f"Wrote {args.output}", file=sys.stderr)
    else:
        print(report, end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())
