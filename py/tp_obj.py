#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
tp_obj.py

Compute transportation problem objective values from:
1) duals + instance
2) solution
3) both, and compare them

Compatible with Python 3.6+.

This version mirrors the instance preprocessing used by the solver output:
- remove all zero-supply sources
- remove all zero-demand destinations
- transpose the reduced TP if the reduced number of sources is greater than
  the reduced number of destinations

It also performs graceful consistency checks so that improper uses are caught
early with readable error messages.

Usage
-----

From duals:
    python tp_obj.py dual duals.txt instance.txt

From solution:
    python tp_obj.py sol solution.txt [instance.txt]

Compare both:
    python tp_obj.py compare duals.txt instance.txt solution.txt
"""

from __future__ import print_function

import argparse
import math
import sys
from pathlib import Path
from typing import Optional


def parse_numbers(line, cast=float):
    return [cast(x) for x in line.strip().split()]


def file_must_exist(path, label):
    if not path.exists():
        raise ValueError("{} '{}' does not exist.".format(label, path))
    if not path.is_file():
        raise ValueError("{} '{}' is not a regular file.".format(label, path))


def read_instance_raw(path):
    file_must_exist(path, "Instance file")

    lines = [line.strip() for line in path.read_text().splitlines() if line.strip()]
    if len(lines) < 3:
        raise ValueError(
            "Instance file '{}' must contain at least 3 non-empty lines.".format(path)
        )

    first = lines[0].split()
    if len(first) < 3:
        raise ValueError(
            "First line of instance file '{}' must contain at least 3 values: M N dummy.".format(path)
        )

    try:
        m_orig = int(first[0])
        n_orig = int(first[1])
    except ValueError:
        raise ValueError("M and N in the instance file must be integers.")

    supplies_orig = parse_numbers(lines[1], float)
    demands_orig = parse_numbers(lines[2], float)

    if len(supplies_orig) != m_orig:
        raise ValueError(
            "Instance file '{}': expected {} source quantities, found {}.".format(
                path, m_orig, len(supplies_orig)
            )
        )
    if len(demands_orig) != n_orig:
        raise ValueError(
            "Instance file '{}': expected {} destination quantities, found {}.".format(
                path, n_orig, len(demands_orig)
            )
        )

    return {
        "m_orig": m_orig,
        "n_orig": n_orig,
        "supplies_orig": supplies_orig,
        "demands_orig": demands_orig,
    }


def preprocess_instance(instance_raw):
    supplies_orig = instance_raw["supplies_orig"]
    demands_orig = instance_raw["demands_orig"]

    # Remove zero-quantity rows/columns exactly as solver preprocessing does.
    supplies = [x for x in supplies_orig if x != 0]
    demands = [x for x in demands_orig if x != 0]

    removed_zero_supply = len(supplies_orig) - len(supplies)
    removed_zero_demand = len(demands_orig) - len(demands)

    # Normalize orientation exactly as solver preprocessing does.
    transposed = False
    if len(supplies) > len(demands):
        supplies, demands = demands, supplies
        transposed = True

    m = len(supplies)
    n = len(demands)

    return {
        "m": m,
        "n": n,
        "supplies": supplies,
        "demands": demands,
        "transposed": transposed,
        "removed_zero_supply": removed_zero_supply,
        "removed_zero_demand": removed_zero_demand,
        "m_orig": instance_raw["m_orig"],
        "n_orig": instance_raw["n_orig"],
        "supplies_orig": supplies_orig,
        "demands_orig": demands_orig,
    }


def read_instance(path):
    return preprocess_instance(read_instance_raw(path))


def read_duals(path, m, n):
    file_must_exist(path, "Duals file")

    lines = [line.strip() for line in path.read_text().splitlines() if line.strip()]
    if len(lines) < 2:
        raise ValueError(
            "Duals file '{}' must contain at least 2 non-empty lines.".format(path)
        )

    row_duals = parse_numbers(lines[0], float)
    col_duals = parse_numbers(lines[1], float)

    if len(row_duals) != m:
        raise ValueError(
            "Duals file '{}': expected {} row duals after preprocessing, found {}.".format(
                path, m, len(row_duals)
            )
        )
    if len(col_duals) != n:
        raise ValueError(
            "Duals file '{}': expected {} column duals after preprocessing, found {}.".format(
                path, n, len(col_duals)
            )
        )

    return row_duals, col_duals


def compute_dual_objective(supplies, demands, row_duals, col_duals):
    return (
        sum(s * u for s, u in zip(supplies, row_duals))
        + sum(d * v for d, v in zip(demands, col_duals))
    )


def read_solution(path, m=None, n=None):
    file_must_exist(path, "Solution file")

    terms = []
    lines = path.read_text().splitlines()

    for lineno, raw_line in enumerate(lines, start=1):
        line = raw_line.strip()
        if not line:
            continue

        parts = line.split()
        if len(parts) != 4:
            raise ValueError(
                "Line {} in '{}' must contain exactly 4 values: "
                "row_index column_index variable_value cost_coefficient".format(lineno, path)
            )

        try:
            row_idx = int(parts[0])
            col_idx = int(parts[1])
            var_value = float(parts[2])
            cost_coeff = float(parts[3])
        except ValueError:
            raise ValueError(
                "Line {} in '{}' contains invalid numeric data.".format(lineno, path)
            )

        if row_idx < 0 or col_idx < 0:
            raise ValueError(
                "Line {} in '{}': row and column indices must be nonnegative.".format(
                    lineno, path
                )
            )

        if m is not None and not (0 <= row_idx < m):
            raise ValueError(
                "Line {} in '{}': row index {} out of range [0, {}] after preprocessing.".format(
                    lineno, path, row_idx, m - 1
                )
            )
        if n is not None and not (0 <= col_idx < n):
            raise ValueError(
                "Line {} in '{}': column index {} out of range [0, {}] after preprocessing.".format(
                    lineno, path, col_idx, n - 1
                )
            )

        terms.append((row_idx, col_idx, var_value, cost_coeff))

    if not terms:
        raise ValueError(
            "Solution file '{}' is empty or contains no valid lines.".format(path)
        )

    return terms


def compute_solution_objective(terms):
    return sum(var_value * cost_coeff for _, _, var_value, cost_coeff in terms)


def detect_preprocessing_from_duals_and_instance(duals_path, instance_path):
    """
    Graceful check:
    compare dual dimensions with
    - original instance dimensions
    - reduced/transposed instance dimensions

    Return a dict explaining which interpretation fits.
    """
    raw = read_instance_raw(instance_path)
    pre = preprocess_instance(raw)

    file_must_exist(duals_path, "Duals file")
    lines = [line.strip() for line in duals_path.read_text().splitlines() if line.strip()]
    if len(lines) < 2:
        raise ValueError(
            "Duals file '{}' must contain at least 2 non-empty lines.".format(duals_path)
        )

    row_duals = parse_numbers(lines[0], float)
    col_duals = parse_numbers(lines[1], float)

    row_count = len(row_duals)
    col_count = len(col_duals)

    matches_raw = (row_count == raw["m_orig"] and col_count == raw["n_orig"])
    matches_pre = (row_count == pre["m"] and col_count == pre["n"])

    return {
        "matches_raw": matches_raw,
        "matches_pre": matches_pre,
        "raw_m": raw["m_orig"],
        "raw_n": raw["n_orig"],
        "pre_m": pre["m"],
        "pre_n": pre["n"],
        "row_count": row_count,
        "col_count": col_count,
        "pre": pre,
    }


def validate_expected_preprocessing(duals_path, instance_path):
    """
    Prevent improper uses:
    - if duals look like they match the original instance dimensions but not the
      preprocessed ones, abort with a clear message
    - if duals match neither, abort with dimensions summary
    """
    info = detect_preprocessing_from_duals_and_instance(duals_path, instance_path)

    if info["matches_pre"]:
        return info["pre"]

    if info["matches_raw"] and not info["matches_pre"]:
        raise ValueError(
            "Duals file appears to match the ORIGINAL instance dimensions "
            "({} x {}) rather than the PREPROCESSED dimensions ({} x {}). "
            "This script expects duals produced after zero-row/zero-column removal "
            "and optional transpose. Refusing to continue.".format(
                info["raw_m"], info["raw_n"], info["pre_m"], info["pre_n"]
            )
        )

    raise ValueError(
        "Duals file dimensions do not match the expected PREPROCESSED instance dimensions.\n"
        "  duals rows/cols      = {} / {}\n"
        "  original instance    = {} / {}\n"
        "  preprocessed instance= {} / {}\n"
        "Refusing to continue.".format(
            info["row_count"],
            info["col_count"],
            info["raw_m"],
            info["raw_n"],
            info["pre_m"],
            info["pre_n"],
        )
    )


def compute_obj_from_duals(duals_path, instance_path):
    inst = validate_expected_preprocessing(duals_path, instance_path)
    row_duals, col_duals = read_duals(duals_path, inst["m"], inst["n"])
    return compute_dual_objective(
        inst["supplies"], inst["demands"], row_duals, col_duals
    )


def compute_obj_from_solution(solution_path, instance_path=None):
    if instance_path is None:
        terms = read_solution(solution_path)
    else:
        inst = read_instance(instance_path)
        if inst["m"] == 0 or inst["n"] == 0:
            raise ValueError(
                "After preprocessing, the instance has dimension {} x {}. "
                "Cannot validate solution indices on an empty reduced instance.".format(
                    inst["m"], inst["n"]
                )
            )
        terms = read_solution(solution_path, m=inst["m"], n=inst["n"])
    return compute_solution_objective(terms)


def cmd_dual(args):
    obj = compute_obj_from_duals(Path(args.duals_file), Path(args.instance_file))
    print(obj)


def cmd_sol(args):
    solution_path = Path(args.solution_file)
    instance_path = Path(args.instance_file) if args.instance_file is not None else None
    obj = compute_obj_from_solution(solution_path, instance_path)
    print(obj)


def cmd_compare(args):
    dual_obj = compute_obj_from_duals(Path(args.duals_file), Path(args.instance_file))
    sol_obj = compute_obj_from_solution(
        Path(args.solution_file), Path(args.instance_file)
    )

    abs_diff = abs(dual_obj - sol_obj)
    same = math.isclose(dual_obj, sol_obj, rel_tol=args.rel_tol, abs_tol=args.abs_tol)

    print(
        "dual={} sol={} diff={} match={}".format(
            dual_obj, sol_obj, abs_diff, "YES" if same else "NO"
        )
    )

    if not same:
        raise SystemExit(2)


def build_parser():
    parser = argparse.ArgumentParser(
        description=(
            "Compute TP objective values from duals, solution, or compare both. "
            "Instance preprocessing mirrors the solver output: zero-row/zero-column "
            "removal and optional transpose to ensure rows <= columns."
        )
    )

    subparsers = parser.add_subparsers(dest="command")
    # Python 3.6-compatible replacement for required=True
    subparsers.required = True

    p_dual = subparsers.add_parser(
        "dual",
        help="Compute objective value from duals and instance.",
    )
    p_dual.add_argument("duals_file", help="File containing row and column duals.")
    p_dual.add_argument("instance_file", help="Original TP instance file.")
    p_dual.set_defaults(func=cmd_dual)

    p_sol = subparsers.add_parser(
        "sol",
        help="Compute objective value from solution file.",
    )
    p_sol.add_argument("solution_file", help="File containing basic variables.")
    p_sol.add_argument(
        "instance_file",
        nargs="?",
        help=(
            "Optional original TP instance file, used to validate indices after "
            "reduction/transposition."
        ),
    )
    p_sol.set_defaults(func=cmd_sol)

    p_compare = subparsers.add_parser(
        "compare",
        help="Compute objective from both duals and solution, then compare them.",
    )
    p_compare.add_argument("duals_file", help="File containing row and column duals.")
    p_compare.add_argument("instance_file", help="Original TP instance file.")
    p_compare.add_argument("solution_file", help="File containing basic variables.")
    p_compare.add_argument(
        "--rel-tol",
        type=float,
        default=1e-9,
        help="Relative tolerance for objective comparison (default: 1e-9).",
    )
    p_compare.add_argument(
        "--abs-tol",
        type=float,
        default=1e-9,
        help="Absolute tolerance for objective comparison (default: 1e-9).",
    )
    p_compare.set_defaults(func=cmd_compare)

    return parser


def main():
    parser = build_parser()
    args = parser.parse_args()

    try:
        args.func(args)
    except BrokenPipeError:
        # Graceful handling for cases like piping to `head`
        try:
            sys.stderr.close()
        except Exception:
            pass
        sys.exit(1)
    except KeyboardInterrupt:
        parser.exit(status=130, message="Interrupted.\n")
    except Exception as e:
        parser.exit(status=1, message="Error: {}\n".format(e))


if __name__ == "__main__":
    main()
