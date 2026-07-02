"""Natural language -> build123d code, via the Claude API.

`generate_code` asks Claude to write build123d code for a described part.
`generate_model` runs that code through the executor and returns a CadModel.

The design is deliberately "code generation": the model writes ordinary
build123d Python that a human could read, edit, and version-control. That keeps
the CAD operations transparent and lets the user drop the AI entirely and hand-
write code when they want to.
"""

from __future__ import annotations

import os
import re

from .executor import run_code, ExecutionError
from .kernel import CadModel, GeometryError

DEFAULT_MODEL = "claude-opus-4-8"

# Teaches the model the exact dialect the executor expects: algebra mode,
# result variable, no imports. Kept concrete and example-driven so generation
# is reliable.
SYSTEM_PROMPT = """\
You write Python code using the build123d library to model 3D solids (B-rep,
OpenCASCADE kernel). Your code is executed in a sandbox where every build123d
name is already imported and `math` helpers (pi, sin, cos, radians, sqrt, ...)
are available.

Hard rules:
- Output ONLY Python code. No prose, no markdown fences.
- Do NOT write any `import` statements — everything is pre-imported.
- Assign the finished solid to a variable named exactly `result`.
- Use build123d ALGEBRA mode (operators), not builder-with blocks.
- All dimensions are in millimeters. Assume Z is up.
- Prefer parametric variables at the top so the intent is readable.

Algebra-mode cheatsheet:
- Primitives (centered at origin): Box(length, width, height),
  Cylinder(radius, height), Sphere(radius), Cone(bottom_radius, top_radius,
  height), Torus(major_radius, minor_radius).
- Combine with operators: `+` union, `-` cut, `&` intersect.
- Move/rotate a shape: `shape.move(Location((x, y, z)))` and
  `Rotation(rx, ry, rz) * shape` (angles in degrees). `Pos(x, y, z) * shape`
  is shorthand for a translation.
- 2D sketches: Rectangle(w, h), Circle(radius), RegularPolygon(radius, n),
  then `extrude(sketch, amount)` to make a solid, or `revolve(sketch, axis)`.
- Fillet/chamfer edges: `fillet(solid.edges(), radius=r)` and
  `chamfer(solid.edges(), length=c)`. Select subsets with
  `.edges().filter_by(Axis.Z)` or `.group_by(Axis.Z)[-1]` (topmost), etc.

Examples:

# A 20mm cube with a 6mm hole drilled through the Z axis
result = Box(20, 20, 20) - Cylinder(radius=3, height=25)

# An L-bracket, 40x40, 5mm thick, with rounded outer vertical edges
base = Box(40, 40, 5)
wall = Pos(0, -17.5, 17.5) * Box(40, 5, 40)
bracket = base + wall
result = fillet(bracket.edges().filter_by(Axis.Z), radius=2)

Return runnable code for the user's request."""


class AIError(RuntimeError):
    """Raised when code generation via the Claude API fails."""


def _extract_code(text: str) -> str:
    """Pull code out of the model reply, tolerating stray markdown fences."""
    fenced = re.findall(r"```(?:python)?\n(.*?)```", text, flags=re.DOTALL)
    if fenced:
        return fenced[0].strip()
    return text.strip()


def generate_code(prompt: str, *, model: str = DEFAULT_MODEL) -> str:
    """Ask Claude to write build123d code for `prompt`. Returns the code string.

    Requires the ANTHROPIC_API_KEY environment variable (or an `ant auth login`
    profile — the SDK resolves credentials automatically).
    """
    try:
        import anthropic
    except ImportError as exc:  # pragma: no cover - environment problem
        raise AIError(
            "The `anthropic` package is required for AI generation. "
            "Run `pip install anthropic`."
        ) from exc

    client = anthropic.Anthropic()
    try:
        response = client.messages.create(
            model=model,
            max_tokens=4096,
            thinking={"type": "adaptive"},
            system=SYSTEM_PROMPT,
            messages=[{"role": "user", "content": prompt}],
        )
    except anthropic.AnthropicError as exc:
        raise AIError(f"Claude API request failed: {exc}") from exc

    if response.stop_reason == "refusal":
        raise AIError("The model declined to generate code for this request.")

    text = "".join(block.text for block in response.content if block.type == "text")
    code = _extract_code(text)
    if not code:
        raise AIError("The model returned an empty response.")
    return code


def generate_model(
    prompt: str,
    *,
    model: str = DEFAULT_MODEL,
    max_repairs: int = 1,
) -> CadModel:
    """Generate build123d code for `prompt`, execute it, and return a CadModel.

    If execution fails, the error is fed back to the model up to `max_repairs`
    times so it can fix its own code (a small self-repair loop).
    """
    code = generate_code(prompt, model=model)
    last_error: Exception | None = None

    for _ in range(max_repairs + 1):
        try:
            return run_code(code)
        except (ExecutionError, GeometryError) as exc:
            last_error = exc
            code = _repair_code(prompt, code, str(exc), model=model)

    raise AIError(f"Could not produce a valid solid after repairs: {last_error}")


def _repair_code(prompt: str, code: str, error: str, *, model: str) -> str:
    """Ask the model to fix code that failed to execute."""
    import anthropic

    client = anthropic.Anthropic()
    repair_prompt = (
        f"The original request was:\n{prompt}\n\n"
        f"Your previous code was:\n{code}\n\n"
        f"It failed with this error:\n{error}\n\n"
        "Return corrected code following the same rules."
    )
    response = client.messages.create(
        model=model,
        max_tokens=4096,
        thinking={"type": "adaptive"},
        system=SYSTEM_PROMPT,
        messages=[{"role": "user", "content": repair_prompt}],
    )
    text = "".join(block.text for block in response.content if block.type == "text")
    return _extract_code(text)
