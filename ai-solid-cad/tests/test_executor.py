"""Tests for the code executor and CadModel — no AI/network required.

These exercise the deterministic core: running build123d code, measuring the
resulting solid, rejecting bad output, and enforcing the no-import guardrail.
Skipped automatically if build123d isn't installed.
"""

import math

import pytest

pytest.importorskip("build123d")

from ai_solid_cad import run_code
from ai_solid_cad.executor import ExecutionError
from ai_solid_cad.kernel import GeometryError


def test_box_volume():
    model = run_code("result = Box(10, 10, 10)")
    assert model.volume == pytest.approx(1000.0)
    assert model.size == pytest.approx((10.0, 10.0, 10.0))


def test_boolean_cut_reduces_volume():
    model = run_code("result = Box(20, 20, 20) - Cylinder(radius=5, height=25)")
    hole = math.pi * 5**2 * 20
    assert model.volume == pytest.approx(20**3 - hole, rel=1e-3)


def test_missing_result_is_an_error():
    with pytest.raises(ExecutionError):
        run_code("x = Box(10, 10, 10)")


def test_import_statements_are_rejected():
    with pytest.raises(ExecutionError):
        run_code("import os\nresult = Box(1, 1, 1)")


def test_non_solid_result_is_rejected():
    with pytest.raises(GeometryError):
        run_code("result = 42")


def test_syntax_error_is_reported():
    with pytest.raises(ExecutionError):
        run_code("result = Box(10, 10,")


def test_export_step(tmp_path):
    model = run_code("result = Box(5, 5, 5)")
    out = model.export(tmp_path / "cube.step")
    assert out.exists() and out.stat().st_size > 0


def test_export_stl(tmp_path):
    model = run_code("result = Sphere(radius=5)")
    out = model.export(tmp_path / "ball.stl")
    assert out.exists() and out.stat().st_size > 0
