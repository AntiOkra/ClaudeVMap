"""Command-line interface: `aicad "make a 20mm cube with a 5mm hole" -o cube.step`.

Subcommands are implicit via flags:
  - Default: describe a part in natural language, get a solid.
  - `--from-code FILE`: skip the AI and run hand-written build123d code.
  - `--show-code`: print the generated code (also runs unless --dry-run).
  - `--dry-run`: generate/print code but don't execute or export.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from . import __version__


def _build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="aicad",
        description="AI-driven solid CAD: natural language -> build123d -> STEP/STL.",
    )
    p.add_argument("prompt", nargs="?", help="Natural-language description of the part.")
    p.add_argument("-o", "--output", help="Output file (.step/.stp/.stl/.brep).")
    p.add_argument(
        "--from-code",
        metavar="FILE",
        help="Run build123d code from FILE instead of calling the AI.",
    )
    p.add_argument("--model", default="claude-opus-4-8", help="Claude model id.")
    p.add_argument("--show-code", action="store_true", help="Print the code that was run.")
    p.add_argument(
        "--dry-run",
        action="store_true",
        help="Generate/print code only; do not execute or export.",
    )
    p.add_argument(
        "--save-code",
        metavar="FILE",
        help="Write the generated build123d code to FILE.",
    )
    p.add_argument("--version", action="version", version=f"aicad {__version__}")
    return p


def main(argv: list[str] | None = None) -> int:
    args = _build_parser().parse_args(argv)

    # Imports are local so `aicad --version` works without build123d installed.
    from .executor import run_code, ExecutionError
    from .ai import generate_code, AIError
    from .kernel import GeometryError

    # 1. Obtain the code (either hand-written or AI-generated).
    if args.from_code:
        code = Path(args.from_code).read_text()
    else:
        if not args.prompt:
            print("error: provide a prompt, or use --from-code FILE", file=sys.stderr)
            return 2
        try:
            code = generate_code(args.prompt, model=args.model)
        except AIError as exc:
            print(f"error: {exc}", file=sys.stderr)
            return 1

    if args.show_code or args.dry_run:
        print(code)
    if args.save_code:
        Path(args.save_code).write_text(code)
        print(f"saved code -> {args.save_code}", file=sys.stderr)

    if args.dry_run:
        return 0

    # 2. Execute the code into a solid.
    try:
        model = run_code(code)
    except (ExecutionError, GeometryError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(model.summary(), file=sys.stderr)

    # 3. Export if requested.
    if args.output:
        path = model.export(args.output)
        print(f"wrote {path}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
