"""Run a hand-written build123d model through the executor (no AI, no API key).

    python examples/hand_written.py

Demonstrates that the same engine the AI targets can be driven directly. The
code string here is exactly what a prompt would produce, so you can iterate on
a design by hand and only reach for the AI when you want a first draft.
"""

from ai_solid_cad import run_code

CODE = """
# A simple flanged bushing: an outer flange plus a tube, bored through.
flange = Cylinder(radius=15, height=4)
tube = Pos(0, 0, 12) * Cylinder(radius=8, height=20)
bore = Cylinder(radius=5, height=60)
result = (flange + tube) - bore
"""

if __name__ == "__main__":
    model = run_code(CODE)
    print(model.summary())
    model.export("bushing.step")
    print("wrote bushing.step")
