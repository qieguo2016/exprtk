#!/usr/bin/env python3
"""
Merge all exprtk .hpp files in src/ into a single header.

- Inlines project headers (#include "xxx.hpp") recursively
- Keeps standard library includes (#include <...>) as-is, deduplicated at top
- Auto-discovers include order from exprtk.hpp - no hardcoded file list
- Run after any src change to regenerate: python scripts/merge_headers.py
"""

import os
import re
from pathlib import Path

SRC_DIR = Path(__file__).resolve().parent.parent / "src"
OUTPUT_FILE = SRC_DIR.parent / "exprtk.hpp"
ENTRY_POINT = "exprtk.hpp"


def collect_includes(content: str, src_dir: Path) -> list[str]:
    """Extract project includes (#include "xxx.hpp") in order."""
    includes = []
    for m in re.finditer(r'#include\s+"([^"]+)"', content):
        inc = m.group(1)
        if inc.endswith(".hpp") and (src_dir / inc).exists():
            includes.append(inc)
    return includes


def collect_stdlib_includes(content: str) -> list[str]:
    """Extract standard library includes (#include <...>)."""
    return re.findall(r'#include\s+<[^>]+>', content)


def topological_order(entry: str, src_dir: Path) -> list[str]:
    """Get file order via DFS from entry, respecting dependencies."""
    order = []
    visited = set()

    def visit(f: str):
        if f in visited:
            return
        visited.add(f)
        path = src_dir / f
        if not path.exists():
            return
        content = path.read_text(encoding="utf-8", errors="replace")
        for inc in collect_includes(content, src_dir):
            visit(inc)
        order.append(f)

    visit(entry)
    return order


def guard_name_from_file(filename: str) -> str | None:
    """EXPRTK_SRC_COMMON_HPP from common.hpp. exprtk.hpp uses INCLUDE_EXPRTK_HPP."""
    if filename == "exprtk.hpp":
        return "INCLUDE_EXPRTK_HPP"
    base = filename.replace(".hpp", "").upper().replace("-", "_")
    return f"EXPRTK_SRC_{base}_HPP"


def strip_guards_and_project_includes(content: str, filename: str, src_dir: Path) -> str:
    """Remove include guards and #include \"xxx.hpp\" lines."""
    lines = content.split("\n")
    result = []
    guard = guard_name_from_file(filename)
    in_guard_block = False
    preproc_depth = 0

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Skip project includes
        if re.match(r'#include\s+"[^"]+\.hpp"', stripped):
            i += 1
            continue

        # Skip stdlib includes (collected at top)
        if re.match(r"#include\s+<[^>]+>", stripped):
            i += 1
            continue

        # Include guard: #ifndef EXPRTK_SRC_XXX_HPP
        if re.match(rf"#ifndef\s+{re.escape(guard)}\s*$", stripped):
            in_guard_block = True
            preproc_depth = 1
            i += 1
            continue
        # #define EXPRTK_SRC_XXX_HPP (immediately after #ifndef)
        if in_guard_block and preproc_depth == 1 and re.match(rf"#define\s+{re.escape(guard)}\s*$", stripped):
            i += 1
            continue

        # Track #if/#ifdef/#ifndef and #endif - skip #endif that closes guard
        if stripped.startswith("#if"):
            preproc_depth += 1
        elif stripped == "#endif" or stripped.startswith("#endif "):
            if preproc_depth == 1:
                i += 1
                continue  # Skip guard's closing #endif
            preproc_depth -= 1

        result.append(line)
        i += 1

    return "\n".join(result)


def merge(src_dir: Path, output_path: Path) -> None:
    order = topological_order(ENTRY_POINT, src_dir)
    all_stdlib = []
    seen_stdlib = set()
    merged_parts = []

    for f in order:
        path = src_dir / f
        content = path.read_text(encoding="utf-8", errors="replace")

        # Collect stdlib includes (keep order, dedupe)
        for inc in collect_stdlib_includes(content):
            if inc not in seen_stdlib:
                seen_stdlib.add(inc)
                all_stdlib.append(inc)

        body = strip_guards_and_project_includes(content, f, src_dir)
        body = body.strip()
        if body:
            merged_parts.append(f"// === {f} ===\n{body}")

    out = []
    out.append("// Auto-generated - do not edit")
    out.append("")
    out.append("#ifndef INCLUDE_EXPRTK_HPP")
    out.append("#define INCLUDE_EXPRTK_HPP")
    out.append("")
    for inc in sorted(all_stdlib, key=lambda x: x.lower()):
        out.append(inc)
    out.append("")
    out.append("")
    out.append("\n\n".join(merged_parts))
    out.append("")
    out.append("#endif // INCLUDE_EXPRTK_HPP")
    out.append("")

    output_path.write_text("\n".join(out), encoding="utf-8")
    print(f"Wrote {output_path} ({len(order)} files merged)")


if __name__ == "__main__":
    merge(SRC_DIR, OUTPUT_FILE)
