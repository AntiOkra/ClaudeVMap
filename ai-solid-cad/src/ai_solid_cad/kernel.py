"""The CadModel wrapper and geometry helpers.

`CadModel` is a thin container around whatever build123d object the generated
code produced (a `Part`, `Solid`, `Compound`, ...). It exposes measured
properties (volume, area, bounding box) and export helpers so the rest of the
application never has to touch the raw build123d topology object directly.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


class GeometryError(ValueError):
    """Raised when generated code produces something that is not a usable solid."""


@dataclass
class CadModel:
    """A solid produced by generated build123d code.

    `shape` is the underlying build123d object. `source` is the code that
    produced it, kept around for debugging and for writing a companion `.py`
    next to an export.
    """

    shape: Any
    source: str = ""
    metadata: dict[str, Any] = field(default_factory=dict)

    # --- measured properties -------------------------------------------------

    @property
    def volume(self) -> float:
        """Enclosed volume in mm³."""
        return float(self.shape.volume)

    @property
    def area(self) -> float:
        """Total surface area in mm²."""
        # build123d exposes `.area` on Part/Solid; fall back to summing faces.
        area = getattr(self.shape, "area", None)
        if area is not None:
            return float(area)
        return float(sum(f.area for f in self.shape.faces()))

    @property
    def bounding_box(self) -> tuple[tuple[float, float, float], tuple[float, float, float]]:
        """((min_x, min_y, min_z), (max_x, max_y, max_z)) in mm."""
        bb = self.shape.bounding_box()
        return (
            (bb.min.X, bb.min.Y, bb.min.Z),
            (bb.max.X, bb.max.Y, bb.max.Z),
        )

    @property
    def size(self) -> tuple[float, float, float]:
        """Overall (dx, dy, dz) extents in mm."""
        (x0, y0, z0), (x1, y1, z1) = self.bounding_box
        return (x1 - x0, y1 - y0, z1 - z0)

    def summary(self) -> str:
        """A short human-readable description of the geometry."""
        dx, dy, dz = self.size
        return (
            f"volume={self.volume:.3f} mm^3, "
            f"area={self.area:.3f} mm^2, "
            f"bbox={dx:.2f} x {dy:.2f} x {dz:.2f} mm"
        )

    # --- export --------------------------------------------------------------

    def export(self, path: str | Path) -> Path:
        """Write the solid to `path`. Format is chosen from the suffix.

        Supported: .step / .stp (STEP), .stl (mesh), .brep (OCCT native).
        """
        # Imported lazily so importing this module doesn't require build123d
        # (useful for the AI layer, which only needs to generate text).
        from build123d import export_step, export_stl, export_brep

        path = Path(path)
        suffix = path.suffix.lower()
        path.parent.mkdir(parents=True, exist_ok=True)

        if suffix in (".step", ".stp"):
            export_step(self.shape, str(path))
        elif suffix == ".stl":
            export_stl(self.shape, str(path))
        elif suffix == ".brep":
            export_brep(self.shape, str(path))
        else:
            raise GeometryError(
                f"Unsupported export format '{suffix}'. "
                "Use .step, .stp, .stl, or .brep."
            )
        return path


def as_cad_model(value: Any, source: str = "") -> CadModel:
    """Coerce an arbitrary generated value into a CadModel, validating it.

    Accepts a build123d shape, or a `BuildPart` builder (from which the built
    `.part` is taken). Anything without a measurable volume is rejected.
    """
    # A BuildPart context object exposes `.part`; unwrap it.
    part = getattr(value, "part", None)
    shape = part if part is not None else value

    if shape is None:
        raise GeometryError(
            "Generated code did not assign a solid to `result`."
        )

    # Validate it behaves like a solid: it must have a positive volume.
    try:
        volume = float(shape.volume)
    except Exception as exc:  # noqa: BLE001 - report the real cause to the user
        raise GeometryError(
            f"`result` is not a solid build123d object: {value!r} ({exc})"
        ) from exc

    if volume <= 0:
        raise GeometryError(
            f"`result` has non-positive volume ({volume}); it is empty or degenerate."
        )

    return CadModel(shape=shape, source=source)
