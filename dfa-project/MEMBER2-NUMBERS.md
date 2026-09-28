<title>Member 2 — Number Sub-DFA</title>

# Member 2 — Number sub-DFA (`S20`–`S26`) + `E2` Invalid Number Format

Companion to `PLAN.md` §5 (Member 2). This is the **final, verified** design: every
transition below was checked by simulating the drawn machine against all 9
`sample_output_scan_*.txt` files. Token sequence and every `Number` lexeme match the
reference exactly.

---

## 1. Two corrections to PLAN.md §5

PLAN.md's suggested skeleton is close but has two defects. Both are confirmed by the
reference output, not by opinion.

### 1.1 The skeleton is nondeterministic on `.`

PLAN.md lists both:

```
S20 --.--> S21                              (correct)
S20 --.--> / S22 --.--> / S25 --.--> E2     (wrong for S20)
```

Two arrows labelled `.` leaving `S20` is not a DFA. **`S20 --.--> S21` is the correct
one; `S20 --.--> E2` must be deleted.** Proof: `76.` emits *only* `E2` and no
`Number 76` (`sample_output_scan_2.txt:24`). If `S20 --.--> E2` fired directly, the
`.` would still be an error — but so would `98.76`, which is a valid `Number`
(`sample_output_scan_2.txt:20`). The dot must be *tentatively accepted* into the
lexeme at `S20`, which is what `S21` is for.

Only `S22` and `S25` — the states where a dot or exponent has **already** been
consumed — go to the stray-dot handler.

### 1.2 The `E2` arrows consume the offending character

PLAN.md doesn't say this, and it changes the output. The arrow into `E2` is **solid,
not dashed** — there is no backtrack, the character that broke the number is eaten
along with the lexeme.

| Input | Consumed by `E2` | Then resumes at | Reference |
|---|---|---|---|
| `3e+e3` | `3e+e` (4 chars) | `3` → `Number 3` | `sample_output_scan_2.txt:33-35` |
| `3.2east` | `3.2ea` (5 chars) | `st` → `Identifier st` | `sample_output_scan_2.txt:30-32` |
| `1.e.1` | `1.e` (3 chars) | `.1` → `E1`, `Number 1` | `sample_output_scan_4.txt` |

If you draw these dashed (backtracking), `3e+e3` yields `Identifier e3` instead of
`Number 3` and `1.e.1` gains a phantom `Identifier e`. I hit exactly this in
simulation before fixing it.

---

## 2. Final transition table

Reserved block `S20`–`S39`; only `S20`–`S26` are needed.

| State | Kind | On | Go to | Notes |
|---|---|---|---|---|
| `S0` | start (Member 1) | `digit` | `S20` | the only entry into this sub-machine |
| **`S20`** | **accept → `Number`** | `digit` | `S20` | integer part |
| | | `.` | `S21` | tentative float |
| | | `e` `E` | `S23` | exponent from a whole number |
| | | `other` | `S0` | **dashed** `other / *` — emit `Number`, retract 1 char |
| `S21` | non-accepting | `digit` | `S22` | dot was legitimate |
| | | `other` | `E2` | **solid**, consumes offender · `76.` `73.` `1.e` |
| **`S22`** | **accept → `Number`** | `digit` | `S22` | fraction digits |
| | | `e` `E` | `S23` | exponent from a float |
| | | `.` | `S26` | second dot — see §3 |
| | | `other` | `S0` | **dashed** `other / *` — emit `Number`, retract 1 char |
| `S23` | non-accepting | `+` `-` | `S24` | the **only** place a sign joins a number |
| | | `digit` | `S25` | unsigned exponent |
| | | `other` | `E2` | **solid**, consumes offender · `55.2e` `3.2ea` |
| `S24` | non-accepting | `digit` | `S25` | |
| | | `other` | `E2` | **solid**, consumes offender · `3e+e` |
| **`S25`** | **accept → `Number`** | `digit` | `S25` | exponent digits |
| | | `.` | `S26` | dot after exponent — see §3 |
| | | `other` | `S0` | **dashed** `other / *` — emit `Number`, retract 1 char |
| `S26` | accept **+** error | — | `E2` → `S0` | emits `Number`, then reports `E2`; dot discarded |

Every accepting state returns to `S0`. `E2` returns to `S0`.

**Accept-node convention.** Member 1's canvas does not double-circle `S1` itself; it
draws a separate `Identifier` double circle reached by a dashed `other /*` arrow. I
followed that: `S20`, `S22` and `S25` stay single circles and all three reach **one
shared `Number` double circle**. They are still the accepting states of the number
language — the emit node is where the token is written out.

---

## 3. `S26` — the stray-dot handler (PLAN.md §2.2, drawn properly)

This is the arrow PLAN.md warns people forget, and the reference behaviour is subtler
than "go to `E2`": the scanner emits the **completed number first**, *then* the error.

```
111.222e333.444   →   Number 111.222e333 ,  E2 ,  Number 444
1e1.              →   Number 1e1 ,  E2
```

Both from `sample_output_scan_4.txt`. Contrast with `76.`, which emits **no** number
at all — because there the dot arrived at `S20`, where it was still a plausible float.

**Draw `S26` as a double circle with a red border**, labelled:

```
S26   emit Number, then E2
      (the '.' is consumed)
```

with one outgoing arrow to `S0`, and a dashed reference line to the shared `E2` state
so the grader can see the error is the same `E2`.

*Compact alternative if the canvas gets crowded:* drop `S26` and draw
`S22 --. / emit Number first--> E2` and `S25 --. / emit Number first--> E2`. Same
behaviour, less clutter, but the dual output is hidden in an arrow label. **Recommend
keeping `S26`** — this is the one place the grader is looking for understanding.

---

## 4. Note for the diagram: why `-` is never part of a number

Required by PLAN.md §5. Put this in a text box beside `S23`/`S24`:

> `+` and `-` are always their own tokens (`Plus` / `Minus`). The **only** transition
> in the whole DFA where a sign is absorbed into a number lexeme is `S23 --+|--> S24`,
> i.e. immediately after an `e`/`E` exponent marker. There is no `S0 --(+|-)-->` arrow
> into the number sub-machine.
>
> Evidence: `3 - 2 - 1` → `Number 3, Minus, Number 2, Minus, Number 1`, while
> `9.8E+7+6.5e4+3e+21` → `Number 9.8E+7, Plus, Number 6.5e4, Plus, Number 3e+21`.
> The `+` in `E+7` is inside the number; the `+` between terms is not. One arrow
> placement decides both.

---

## 5. Integration notes (PLAN.md §6)

- **§6.2 `e`/`E`** — nothing to do on my side. There is no `e` arrow from `S0` into
  this sub-machine; `S0 --letter--> S1` (Member 1) owns `e` at the start state. My
  exponent arrows leave `S20` and `S22` only. Confirmed by `e+e` →
  `Identifier e, Plus, Identifier e`.
- **§6.3 `+`/`-`** — nothing to do on my side. See §4 above; the sign arrow leaves
  `S23` only.
- **Shared `E2`** — I draw the state once; the five inbound arrows are
  `S21`, `S23`, `S24`, and (via `S26`) `S22` and `S25`.
- I need `S0` and the `E2` position from Member 1's canvas before I can place mine.

---

## 6. Validation

Simulated the machine above against all 9 samples:

- **Token/error sequence:** 9/9 exact match (605 tokens total).
- **`Number` lexemes:** 75/75 exact match across all samples.

Valid, all confirmed `Number`: `12321` `98.76` `1.2e45` `9e87` `7E+8` `6E-5` `9.8E+7`
`6.5e4` `3e+21` `111.222e333` `123211231` `98.3243246`

Errors, all confirmed `E2`: `76.` `73.` `55.2e` `1.e` `3.2ea` (from `3.2east`)
`3e+e` (from `3e+e3`) and the stray dots in `111.222e333.444` and `.1e1.`

Line numbers were ignored in the comparison — the reference implementation's line
counter is buggy (PLAN.md §7), which does not affect the diagram.


---

## 7. What is on the canvas (`DFA_Project.drawio.xml`)

Drawn into Member 1's file, in the reserved `S20`-`S39` block, occupying the band
below the hub (`y` 460-790) so nothing of Member 1's is disturbed.

| Cell | ID | Position |
|---|---|---|
| `S20` | `iWjG5c7jbskbDDUZ4hYp-6` (Member 1's stub, moved) | 300, 460 |
| `S21`-`S25` | `M2-S21` … `M2-S25` | 440 / 580 / 720 / 850 / 980, 460 |
| `Number` accept | `M2-NUM` | 540, 630 |
| `E2` | `M2-E2` | 685, 788 |
| `S26` | `M2-S26` (red double circle) | 965, 695 |
| Legend | `M2-legend` | 890, 20 |
| State key | `M2-key` | 140, 930 |
| Constraints & notes | `M2-cons` | 480, 930 |

21 transitions, 3 of them grey returns to `S0`. Verified programmatically: no
duplicate IDs, no dangling endpoints, no overlapping shapes, and no edge routed
through a shape. Each of `S20`-`S25` has exactly one arrow per input class, so the
sub-machine is deterministic.

### Three things I changed in Member 1's region

1. **Moved his `S20` stub** from `(290, 190)` to `(300, 460)` and gave the existing
   `S0 --digit--> S20` edge a waypoint at `x = 327` so it drops down a free corridor.
   Same cell, same ID — his edge still binds to it.
2. **Made his two `other /*` arrows dashed** (`S1 --> Identifier`, `S2 --> Divide`),
   applying the convention `PLAN.md` §3.5 agreed on. They were solid, which under the
   legend I wrote would have meant "consumes the character" — the opposite of what
   they do.
3. Nothing else. The original file is kept as `DFA_Project.drawio.xml.bak`.

### Deviations from PLAN.md §3, and why

- **§3.3 error-state shape.** The plan says red-bordered double circle. Member 1 drew
  `E1` as a process box, so `E2` matches `E1` rather than the plan. The legend
  documents the box. `S26` is a red double circle because it emits a token *and*
  errors. **Team should confirm** — if we prefer the plan's shape, it is a two-cell
  restyle.
- **§3.7 legend position.** The plan says top-left; the top-left strip is crossed by
  Member 1's dispatch edges out of `S0`, so the legend sits top-right. That leaves the
  top-left free for the title/header block, which is Member 1's per §5.

### Open items for Member 1 / Member 3 (not mine to fix)

- Member 1's `S3 --newline--> S0` edge is routed straight through the `EndOfFile`
  node. Cosmetic, but it will show in the PDF.
- `S2 --other /*-->` still has no target — it ends at a free-floating point near
  `Divide`. That is Member 3's arrow per `PLAN.md` §6.1.
- The `Identifier`, `Divide` and `EndOfFile` accept nodes have no return arrow to
  `S0` yet (§6 checklist). My three returns show the pattern and the grey stroke to
  match.
