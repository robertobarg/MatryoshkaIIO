#!/usr/bin/env python3
"""
Compute transportation problem objective values from:
1) duals + instance
2) solution
3) both, and compare them

Usage
-----

From duals:
    python tp_obj.py dual duals.txt instance.txt

From solution:
    python tp_obj.py sol solution.txt

Compare both:
    python tp_obj.py compare duals.txt instance.txt solution.txt
"""

import argparse
import math
from pathlib import Path


def parse_numbers(line: str, cast=float):
    return [cast(x) for x in line.strip().split()]


def read_instance(path: Path):
    lines = [line.strip() for line in path.read_text().splitlines() if line.strip()]
    if len(lines) < 3:
        raise ValueError(f"Instance file '{path}' must contain at least 3 non-empty lines.")

    first = lines[0].split()
    if len(first) < 3:
        raise ValueError(
            f"First line of instance file '{path}' must contain at least 3 values: M N dummy."
        )

    try:
        m = int(first[0])
        n = int(first[1])
    except ValueError as exc:
        raise ValueError("M and N in the instance file must be integers.") from exc

    supplies = parse_numbers(lines[1], float)
    demands = parse_numbers(lines[2], float)

    if len(supplies) != m:
        raise ValueError(
            f"Instance file '{path}': expected {m} source quantities, found {len(supplies)}."
        )
    if len(demands) != n:
        raise ValueError(
            f"Instance file '{path}': expected {n} destination quantities, found {len(demands)}."
        )

    return m, n, supplies, demands


def read_duals(path: Path, m: int, n: int):
    lines = [line.strip() for line in path.read_text().splitlines() if line.strip()]
    if len(lines) < 2:
        raise ValueError(f"Duals file '{path}' must contain at least 2 non-empty lines.")

    row_duals = parse_numbers(lines[0], float)
    col_duals = parse_numbers(lines[1], float)

    if len(row_duals) != m:
        raise ValueError(
            f"Duals file '{path}': expected {m} row duals, found {len(row_duals)}."
        )
    if len(col_duals) != n:
        raise ValueError(
            f"Duals file '{path}': expected {n} column duals, found {len(col_duals)}."
        )

    return row_duals, col_duals


def compute_dual_objective(supplies, demands, row_duals, col_duals):
    return (
        sum(s * u for s, u in zip(supplies, row_duals))
        + sum(d * v for d, v in zip(demands, col_duals))
    )


def read_solution(path: Path):
    terms = []
    lines = path.read_text().splitlines()

    for lineno, raw_line in enumerate(lines, start=1):
        line = raw_line.strip()
        if not line:
            continue

        parts = line.split()
        if len(parts) != 4:
            raise ValueError(
                f"Line {lineno} in '{path}' must contain exactly 4 values: "
                "row_index column_index variable_value cost_coefficient"
            )

        try:
            row_idx = int(parts[0])
            col_idx = int(parts[1])
            var_value = float(parts[2])
            cost_coeff = float(parts[3])
        except ValueError as exc:
            raise ValueError(
                f"Line {lineno} in '{path}' contains invalid numeric data."
            ) from exc

        terms.append((row_idx, col_idx, var_value, cost_coeff))

    if not terms:
        raise ValueError(f"Solution file '{path}' is empty or contains no valid lines.")

    return terms


def compute_solution_objective(terms):
    return sum(var_value * cost_coeff for _, _, var_value, cost_coeff in terms)


def compute_obj_from_duals(duals_path: Path, instance_path: Path):
    m, n, supplies, demands = read_instance(instance_path)
    row_duals, col_duals = read_duals(duals_path, m, n)
    return compute_dual_objective(supplies, demands, row_duals, col_duals)


def compute_obj_from_solution(solution_path: Path):
    terms = read_solution(solution_path)
    return compute_solution_objective(terms)


def cmd_dual(args):
    obj = compute_obj_from_duals(Path(args.duals_file), Path(args.instance_file))
    print(obj)


def cmd_sol(args):
    obj = compute_obj_from_solution(Path(args.solution_file))
    print(obj)


def cmd_compare(args):
    dual_obj = compute_obj_from_duals(Path(args.duals_file), Path(args.instance_file))
    sol_obj = compute_obj_from_solution(Path(args.solution_file))

    abs_diff = abs(dual_obj - sol_obj)
    same = math.isclose(dual_obj, sol_obj, rel_tol=args.rel_tol, abs_tol=args.abs_tol)

    print(f"dual={dual_obj} sol={sol_obj} diff={abs_diff} match={'YES' if same else 'NO'}")

    if not same:
        raise SystemExit(2)

def build_parser():
    parser = argparse.ArgumentParser(
        description="Compute TP objective values from duals, solution, or compare both."
    )

    subparsers = parser.add_subparsers(dest="command", required=True)

    p_dual = subparsers.add_parser(
        "dual",
        help="Compute objective value from duals and instance."
    )
    p_dual.add_argument("duals_file", help="File containing row and column duals.")
    p_dual.add_argument("instance_file", help="TP instance file.")
    p_dual.set_defaults(func=cmd_dual)

    p_sol = subparsers.add_parser(
        "sol",
        help="Compute objective value from solution file."
    )
    p_sol.add_argument("solution_file", help="File containing basic variables.")
    p_sol.set_defaults(func=cmd_sol)

    p_compare = subparsers.add_parser(
        "compare",
        help="Compute objective from both duals and solution, then compare them."
    )
    p_compare.add_argument("duals_file", help="File containing row and column duals.")
    p_compare.add_argument("instance_file", help="TP instance file.")
    p_compare.add_argument("solution_file", help="File containing basic variables.")
    p_compare.add_argument(
        "--rel-tol",
        type=float,
        default=1e-9,
        help="Relative tolerance for objective comparison (default: 1e-9)."
    )
    p_compare.add_argument(
        "--abs-tol",
        type=float,
        default=1e-9,
        help="Absolute tolerance for objective comparison (default: 1e-9)."
    )
    p_compare.set_defaults(func=cmd_compare)

    return parser


def main():
    parser = build_parser()
    args = parser.parse_args()

    try:
        args.func(args)
    except Exception as e:
        parser.exit(status=1, message=f"Error: {e}\n")


if __name__ == "__main__":
    main()
