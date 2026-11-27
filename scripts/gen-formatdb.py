#!/usr/bin/env python3

import ast
import json
import operator
import re
import sys
from pathlib import Path

SPEC = """
[set]
PalBool: PAL_FALSE, PAL_TRUE
PalResult: PAL_RESULT_
PalResultCode: PAL_RESULT_CODE_
PalResultSource: PAL_RESULT_SOURCE_

[bitmask]
PalWindowFlags: PAL_WINDOW_FLAG_
"""

_OPS = {
    ast.BitOr: operator.or_,       # a | b
    ast.BitAnd: operator.and_,     # a & b
    ast.BitXor: operator.xor,      # a ^ b
    ast.LShift: operator.lshift,   # a << b
    ast.RShift: operator.rshift,   # a >> b
    ast.Add: operator.add,         # a + b
    ast.Sub: operator.sub,         # a - b
    ast.Invert: operator.invert,   # ~a
    ast.UAdd: operator.pos,        # +a
    ast.USub: operator.neg,        # -a
}

BUILTIN_CAST_TYPES = frozenset({
    "char", "short", "int", "long", "signed", "unsigned",
    "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t",
    "size_t", "intptr_t", "uintptr_t", "ptrdiff_t",
})

CAST_RE = re.compile(r"\(\s*([A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*)\s*\)")
SUFFIX_RE = re.compile(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b")
DEFINE_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\s+(.+)$")


def warn(message: str):
    print(f"[gen-formatdb] warning: {message}", file=sys.stderr)


def parse_spec(spec_text: str):
    sets: dict[str, tuple[str, ...]] = {}
    bitmasks: dict[str, tuple[str, ...]] = {}
    section = "sets"

    for line_num, raw_line in enumerate(spec_text.splitlines(), start=1):
        line = raw_line.strip()
        if not line:
            continue
        if line == "[set]":
            section = "sets"
            continue
        if line == "[bitmask]":
            section = "bitmasks"
            continue
        if ":" not in line:
            raise ValueError(
                f"SPEC line {line_num}: expected 'TypeName: PREFIX_'")

        type_name, raw_prefixes = line.split(":", 1)
        prefixes = tuple(p.strip()
                         for p in raw_prefixes.split(",") if p.strip())
        (sets if section == "sets" else bitmasks)[type_name.strip()] = prefixes

    return sets, bitmasks


def strip_casts(expr: str, cast_types) -> str:
    def repl(m):
        words = m.group(1).split()
        return " " if all(w in cast_types for w in words) else m.group(0)

    return CAST_RE.sub(repl, expr)


def eval_c_expr(expr: str, symbols: dict[str, int], cast_types=BUILTIN_CAST_TYPES) -> int:
    expr = SUFFIX_RE.sub(r"\1", expr)
    expr = strip_casts(expr, cast_types).strip()

    def _eval(node):
        if isinstance(node, ast.Expression):
            return _eval(node.body)

        if isinstance(node, ast.Constant) and isinstance(node.value, int):
            return node.value

        if isinstance(node, ast.Name) and node.id in symbols:
            return symbols[node.id]

        if isinstance(node, ast.BinOp) and type(node.op) in _OPS:
            return _OPS[type(node.op)](_eval(node.left), _eval(node.right))

        if isinstance(node, ast.UnaryOp) and type(node.op) in _OPS:
            return _OPS[type(node.op)](_eval(node.operand))

        raise ValueError(f"Unsupported expression: {expr}")

    return int(_eval(ast.parse(expr, mode="eval")))


def build_prefix_rules(sets: dict, bitmasks: dict):
    rules: list[tuple[str, str, str]] = []
    for kind, table in (("sets", sets), ("bitmasks", bitmasks)):
        for type_name, prefixes in table.items():
            for p in prefixes:
                rules.append((p, kind, type_name))

    rules.sort(key=lambda r: len(r[0]), reverse=True)
    return rules


def clean_header_text(text: str) -> list[str]:
    text = re.sub(r"\\\r?\n", " ", text)
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return [re.sub(r"//.*$", "", line) for line in text.splitlines()]


def parse_headers(include_dir: Path, sets: dict, bitmasks: dict):
    rules = build_prefix_rules(sets, bitmasks)
    cast_types = BUILTIN_CAST_TYPES | set(sets) | set(bitmasks)
    db = {
        "sets": {t: {} for t in sets},
        "bitmasks": {t: {} for t in bitmasks},
    }
    
    symbols: dict[str, int] = {}

    # Pass 1: Collect all #define pairs across all headers
    defs: dict[str, str] = {}
    for header in sorted(include_dir.rglob("*.h")):
        text = header.read_text(encoding="utf-8", errors="replace")
        for line in clean_header_text(text):
            m = DEFINE_RE.match(line)
            if m:
                defs[m.group(1)] = m.group(2).strip()

    # Pass 2: Resolve constant expressions iteratively
    unresolved = list(defs.items())
    errors: dict[str, str] = {}
    while unresolved:
        next_unresolved = []
        for macro_name, raw_val in unresolved:
            try:
                symbols[macro_name] = eval_c_expr(raw_val, symbols, cast_types)
                errors.pop(macro_name, None)
            except Exception as exc:
                errors[macro_name] = str(exc)
                next_unresolved.append((macro_name, raw_val))
        if len(next_unresolved) == len(unresolved):
            break

        unresolved = next_unresolved

    for macro_name, raw_val in unresolved:
        if any(macro_name.startswith(prefix) for prefix, _, _ in rules):
            warn(f"{macro_name} = '{raw_val}' matches a prefix but could not be "
                 f"evaluated ({errors.get(macro_name, 'unknown error')}); skipped")

    # Pass 3: Populate output database by longest matching prefix
    for macro_name in defs:
        if macro_name not in symbols:
            continue

        for prefix, kind, type_name in rules:
            if macro_name.startswith(prefix):
                val_key = str(symbols[macro_name])
                target_map = db[kind][type_name]
                if val_key not in target_map:
                    target_map[val_key] = macro_name
                break

    return db


def main():
    root = Path(__file__).resolve().parent.parent
    include_dir = root / "include"
    build_dir = root / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    sets, bitmasks = parse_spec(SPEC)
    db = parse_headers(include_dir, sets, bitmasks)

    out_path = build_dir / "formatdb.json"
    out_path.write_text(json.dumps(db, indent=4) + "\n", encoding="utf-8")

    print(f"format database generated successfully")


if __name__ == "__main__":
    main()
