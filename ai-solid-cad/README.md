# ai-solid-cad

**Describe a part in plain language, get a real solid (STEP/STL).**

`ai-solid-cad` is an AI-operable solid CAD tool. You describe a part in natural
language; [Claude](https://www.anthropic.com/) writes
[build123d](https://build123d.readthedocs.io/) code; that code runs on the
industrial-grade **OpenCASCADE (OCCT)** B-rep kernel to produce a real,
manufacturable solid you can export to STEP, STL, or BREP.

```bash
aicad "a 20mm cube with a 6mm hole drilled through the top" -o cube.step
```

## Why this design

- **Existing kernel, not a toy.** All geometry is done by OpenCASCADE via
  `build123d`/OCP — booleans, fillets, chamfers, revolves, STEP/IGES I/O. No
  geometry kernel is reimplemented here.
- **Code generation, not a black box.** The AI writes ordinary, readable
  build123d Python. You can print it (`--show-code`), save it (`--save-code`),
  edit it, and re-run it without the AI (`--from-code`). The CAD operations stay
  transparent and version-controllable.
- **Python, because it fits.** LLMs generate Python most reliably, `build123d`'s
  "code-CAD" model maps cleanly onto code generation, and the OCCT binding is a
  binary wheel — no native build step.

## Architecture

```
 natural language ──► ai.py ─────► build123d code ──► executor.py ──► CadModel ──► STEP/STL
                    (Claude API)   (readable Python)   (sandboxed      (kernel.py,
                                                        exec + guard)   OpenCASCADE)
```

| Module | Responsibility |
| --- | --- |
| `ai.py` | Prompt → build123d code via the Claude API, with a small self-repair loop. |
| `executor.py` | Run generated code in a namespace with build123d pre-imported; guardrails (no imports, restricted builtins); capture `result`. |
| `kernel.py` | `CadModel` — measured properties (volume, area, bbox) and export to STEP/STL/BREP. |
| `cli.py` | The `aicad` command. |

## Install

```bash
pip install -e ".[ai]"       # AI generation + kernel
# or, kernel only (hand-written code, no API key needed):
pip install -e .
```

Set your Anthropic API key for AI generation:

```bash
export ANTHROPIC_API_KEY=sk-ant-...
```

## Usage

```bash
# Natural language -> STEP
aicad "an L-shaped bracket, 40x40, 5mm thick, with 3mm rounded outer edges" -o bracket.step

# See / keep the generated code
aicad "a hex nut for an M10 bolt, 8mm thick" --show-code -o nut.stl
aicad "a flanged bushing, bored 10mm through" --save-code bushing.py -o bushing.step

# No AI — run hand-written or edited build123d code
aicad --from-code bushing.py -o bushing.step
```

As a library:

```python
from ai_solid_cad import generate_model, run_code

model = generate_model("a 30mm gear blank, 8mm thick, with a 10mm bore")
print(model.summary())          # volume / area / bounding box
model.export("gear.step")

# Or drive the kernel directly, no AI:
model = run_code("result = Box(20, 20, 20) - Cylinder(radius=5, height=25)")
```

## The build123d dialect the model targets

Generated code uses build123d **algebra mode** and must assign the finished
solid to `result`. Everything is pre-imported; no `import` statements are
allowed (the executor enforces this). Millimeters, Z up.

```python
result = Box(20, 20, 20) - Cylinder(radius=3, height=25)   # cube with a bore
```

See [`examples/prompts.md`](examples/prompts.md) for more, and
[`examples/hand_written.py`](examples/hand_written.py) for a no-AI run.

## Security note

The executor is a **guardrail, not a sandbox**: it blocks `import` statements
and dangerous builtins (`open`, `eval`, `exec`, ...), which stops accidents but
not a determined adversary. If you run untrusted prompts/code, execute it inside
a real sandbox (container, seccomp, gVisor).

## Testing

```bash
pip install -e ".[dev]"
pytest        # deterministic core tests; no network / API key needed
```

## License

MIT — see [LICENSE](LICENSE).
