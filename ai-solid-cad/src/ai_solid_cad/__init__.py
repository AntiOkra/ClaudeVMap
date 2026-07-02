"""AI Solid CAD — natural-language-driven solid modeling on the OpenCASCADE kernel.

The public surface is intentionally small:

    from ai_solid_cad import generate_model, run_code, CadModel

`generate_model` turns a natural-language prompt into a solid by asking Claude
to write build123d code, then executing it. `run_code` executes build123d code
directly (no AI), which is also what the executor uses under the hood.
"""

from .kernel import CadModel
from .executor import run_code, ExecutionError
from .ai import generate_code, generate_model, AIError

__all__ = [
    "CadModel",
    "run_code",
    "generate_code",
    "generate_model",
    "ExecutionError",
    "AIError",
]

__version__ = "0.1.0"
