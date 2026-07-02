"""Execute generated build123d code and capture the resulting solid.

The generated code is expected to build a solid and assign it to a variable
named ``result`` (algebra mode), e.g.::

    result = Box(20, 20, 20) - Cylinder(radius=5, height=25)

We run that code in a namespace that has all of build123d available, plus a
handful of math helpers, and then hand the ``result`` to `as_cad_model` for
validation.

Security note
-------------
This is a *guardrail*, not a true sandbox. We strip obviously dangerous
builtins (``open``, ``eval``, ``exec``, ``__import__``, ``compile``) so that a
casual generated snippet can't read files or shell out, and we forbid ``import``
statements in the source. This stops accidents, not a determined adversary.
Run untrusted code in a real sandbox (container / seccomp / gVisor) if that
matters to you.
"""

from __future__ import annotations

import ast
import math
from typing import Any

from .kernel import CadModel, as_cad_model, GeometryError


class ExecutionError(RuntimeError):
    """Raised when generated code fails to import, parse, or execute."""


# Builtins that generated CAD code has no legitimate need for.
_FORBIDDEN_BUILTINS = {
    "open", "eval", "exec", "compile", "__import__", "input",
    "globals", "locals", "vars", "getattr", "setattr", "delattr",
}


def _safe_builtins() -> dict[str, Any]:
    import builtins as _b

    allowed = {}
    for name in dir(_b):
        if name.startswith("_") or name in _FORBIDDEN_BUILTINS:
            continue
        allowed[name] = getattr(_b, name)
    return allowed


def _check_no_imports(code: str) -> None:
    """Reject `import ...` statements; build123d is pre-injected already."""
    try:
        tree = ast.parse(code)
    except SyntaxError as exc:
        raise ExecutionError(f"Generated code is not valid Python: {exc}") from exc

    for node in ast.walk(tree):
        if isinstance(node, (ast.Import, ast.ImportFrom)):
            raise ExecutionError(
                "Generated code must not contain `import` statements — "
                "build123d and math are already available."
            )


def build_namespace() -> dict[str, Any]:
    """Construct the execution namespace: all of build123d + math helpers."""
    try:
        import build123d
    except ImportError as exc:  # pragma: no cover - environment problem
        raise ExecutionError(
            "build123d is not installed. Run `pip install build123d`."
        ) from exc

    ns: dict[str, Any] = {"__builtins__": _safe_builtins()}
    # Star-import everything build123d exposes (Box, Cylinder, extrude, ...).
    ns.update({name: getattr(build123d, name) for name in build123d.__all__})
    # Convenience math helpers so prompts like "45 degree chamfer" work.
    ns.update(
        pi=math.pi,
        tau=math.tau,
        sin=math.sin, cos=math.cos, tan=math.tan,
        radians=math.radians, degrees=math.degrees,
        sqrt=math.sqrt,
    )
    return ns


def run_code(code: str, *, result_var: str = "result") -> CadModel:
    """Execute build123d `code` and return the validated `result` as a CadModel.

    Raises `ExecutionError` on parse/runtime failure, `GeometryError` if the
    produced value is not a usable solid.
    """
    _check_no_imports(code)
    namespace = build_namespace()

    try:
        exec(compile(code, "<generated>", "exec"), namespace)  # noqa: S102
    except Exception as exc:  # noqa: BLE001 - surface the real error to the user
        raise ExecutionError(f"Generated code raised {type(exc).__name__}: {exc}") from exc

    if result_var not in namespace:
        raise ExecutionError(
            f"Generated code never assigned `{result_var}`. "
            f"It must set `{result_var}` to the finished solid."
        )

    try:
        return as_cad_model(namespace[result_var], source=code)
    except GeometryError:
        raise  # already descriptive
