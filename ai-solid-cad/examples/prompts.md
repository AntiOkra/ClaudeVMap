# Example prompts

Prompts that work well with `aicad`. Dimensions are in millimeters.

```bash
aicad "a 20mm cube with a 6mm hole drilled through the top" -o cube.step
aicad "an L-shaped bracket, 40x40, 5mm thick, with 3mm rounded outer edges" -o bracket.step
aicad "a hex nut for an M10 bolt, 8mm thick" -o nut.stl
aicad "a flanged bushing: 30mm flange, 16mm tube 20mm tall, bored 10mm through" -o bushing.step
aicad "a gear-like disc, 50mm diameter, 8mm thick, with 6 evenly spaced 5mm mounting holes on a 40mm bolt circle" -o plate.step
```

Useful flags:

- `--show-code` — print the build123d code that was generated and run.
- `--save-code model.py` — keep the generated code to edit by hand later.
- `--dry-run` — generate/print code without executing (no build123d needed).
- `--from-code model.py` — run hand-written/edited code, skipping the AI.
