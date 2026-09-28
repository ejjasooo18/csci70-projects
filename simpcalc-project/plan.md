# GAME PLAN: SimpCalc Scanner & Parser
**Due:** Wed, Sep 30, 2026 · **Language:** C · **Grade:** Scanner 50% / Parser 50%
**Deliverable:** `surname1_surname2_surname3_CSCI70_SCANNER_PARSER.zip` (source + comments/docs + certificate of authorship)

## Status
| Member | Part | Status |
|---|---|---|
| A | Scanner (`token.h`, `scanner.h`, `scanner.c`) | ✅ **Done.** Matches all 9 `sample_output_scan_*.txt` byte for byte |
| B | Parser (`parser.h`, `parser.c`) | ⬜ To do |
| C | Driver, tests, submission (`main.c`, test script, README, zip) | ⬜ To do |

Read [HANDOFF.md](HANDOFF.md) first.

## 1. What we're building
```
sample_input_N.txt ──► scanner.c (gettoken()) ──► token stream ──► parser.c (recursive descent)
                               │                                        │
                   sample_output_scan_N.txt                 sample_output_parse_N.txt
```
- `main.c` finds **every** `*_input*.txt` file in the directory and scans+parses each one.
- Output naming: `sample_input_1.txt` → `sample_output_scan_1.txt` and `sample_output_parse_1.txt` (swap `input` for `output_scan` / `output_parse`).
- Success = our output files are **identical** to the provided samples (we diff them).

## 2. The shared contract: `token.h` (done)
The token enum, the `Token` struct and `TOKEN_NAMES[]` are in [token.h](token.h). The scanner API is in [scanner.h](scanner.h):
```c
void  scanner_init(FILE *in, FILE *scan_out);  /* scan_out may be NULL */
Token gettoken(void);                          /* also writes the token line to scan_out */
```
Only change `token.h` after telling the other two.

## 3. Task split

### Member A: Scanner ✅ done
The DFA, keywords, strings, comments and every lexical-error format are implemented. See the header comment in [scanner.c](scanner.c).

### Member B: Parser (`parser.c`, `parser.h`)
1. Write one function per nonterminal: `Prg Blk Stm Argfollow Arg Iffollow Exp Trmfollow Trm Facfollow Fac Litfollow Lit Val Cnd Rel`.
2. Use a global `Token cur` and `match(TokenType expected)`: if it matches, advance with `gettoken()`; otherwise print the error and **stop parsing that file**.
3. Print the messages: `Assignment Statement Recognized`, `Print Statement Recognized`, `If Statement Begins` (at start of IF), and `If Statement Ends` (after Iffollow).
4. On success, print `<filename> is a valid SimpCalc program`.
5. On error, print `Parse Error on line N: <TokenName> Expected.` (e.g. `Colon Expected.`) and stop. Stopping can be a `longjmp` or an `error` flag checked after each call.
6. Test on samples 1, 3, 5–9. (Samples 2 and 4 fail on line 1/3 with `Assign Expected.`)

### Member C: Driver, integration, tests, submission (`main.c`, test script, README)
1. Install gcc (see HANDOFF.md). The build command is `gcc -Wall -o simpcalc main.c scanner.c parser.c`.
2. `main.c`: list the directory (`<dirent.h>` works under MinGW), find every file whose name contains `_input`, compute both output names, open the files, and call `scanner_init` and then the parser.
3. Scan output and parse output are **both produced from one pass**: each `gettoken()` call logs the token to the scan file. After the parser stops (success or error), keep calling `gettoken()` until `T_EOF` so the scan file is complete. (The scan samples list every token even when the parse stopped at line 1.)
4. Write `test.sh`/`test.bat`: build, run on a **copy** of the samples folder (so the expected files aren't overwritten), and `diff` each output against the expected file. Report pass/fail for all 18 files.
5. Final: header comments in every file, README (how to build and run), certificate of authorship, zip with the correct name.

## 4. Gotchas found in the samples
**Scanner (already handled in scanner.c):**
- An invalid number or a lone `!` **consumes the offending character**:
  - `3.2east` → Invalid number, then `Identifier st`
  - `!ouch` → error, then `Identifier uch`
- `.76` → Illegal character, then `Number 76`. `76.` → Invalid number format.
- The reference output has two line-number quirks, and we reproduce both:
  - A newline swallowed by an invalid number isn't counted.
  - An unterminated string counts its newline twice.

**Parser:**
- **The spec's Rel table has a typo**: it labels `<=` as GTEqual and `>=` as LTEqual. The scanner already uses the correct pairing (`<=` = `T_LTEQUAL`, `>=` = `T_GTEQUAL`). Just accept all six relational tokens in `Rel`.
- `AND`/`OR`/`NOT` are tokens but **not in the grammar**. `IF a > b AND ...` must fail with `Colon Expected.` (samples 3, 7, 8, 9). **Don't add them to the grammar.**
- `Blk → ε` whenever the token isn't Identifier/PRINT/IF. That's how `ENDIF`/`ELSE`/EOF end a block.
- `Val → (Exp)` is the fallback when the token isn't Identifier/Number/SQRT. So a bad token there becomes `LeftParen Expected.`
- The last line of the parse file has **no trailing newline** on error. It **has** one on success.
- The spec also lists `Invalid Statement`, `Incomplete if Statement` and `Missing relational operator`, but no sample shows them. **Default:** use `Parse Error on line N: Missing relational operator.` when Rel fails, and `<X> Expected.` everywhere else. Ask the instructor/TA if possible.

## 5. Timeline (2 days)
| When | B (Parser) | C (Driver/Test) |
|---|---|---|
| **Day 1 PM** | All 16 procedures + `match` | Install gcc, `main.c` directory loop + file naming, diff script |
| **Day 1 night** | Messages + error stop logic | **Integration #1**: real scanner + parser; samples 1, 5, 6 pass |
| **Day 2 AM** | Fix diffs on samples 3, 7, 8, 9 | Run full suite, triage failures, write extra edge-case inputs |
| **Day 2 PM** | Comments/docs in parser | README, certificate, **all 18 diffs pass**, zip, test the zip on a clean folder |
| **Before deadline** | Everyone reviews and submits | |

## 6. Definition of done
- [ ] `gcc -Wall` builds with no warnings
- [ ] Running in the samples folder regenerates all 9 scan and 9 parse outputs, and `diff` shows no differences
- [ ] Also works when the folder contains other, non-sample `*_input*.txt` files
- [ ] Every function has a comment. Every file has a header comment (authors, purpose)
- [ ] Zip is named correctly and contains the source, README and certificate of authorship

## 7. Git workflow
- One branch per person (`parser`, `driver`). Merge to `main` at each integration point.
- Don't commit `.exe` files or generated output files.
