# Handoff: Scanner and Parser are done, driver and submission are next

**For:** Member C (Driver, tests, submission)
**Due:** Wed, Sep 30, 2026. The full plan and task list are in [plan.md](plan.md).

## TL;DR
- The scanner is finished and tested. It produces all 9 `sample_output_scan_*.txt` files **byte for byte**.
- The parser is finished and tested. It produces all 9 `sample_output_parse_*.txt` files **byte for byte**.
- **C:** write `main.c`, the test script and the README, and put the zip together.
- **Don't edit `scanner.c`, `token.h`, `parser.c`, or `parser.h`** without telling the team.

## What's in the repo
| File | What it is |
|---|---|
| [token.h](token.h) | `TokenType` enum, `Token` struct, `TOKEN_NAMES[]`. **Used by scanner, parser, and main.** |
| [scanner.h](scanner.h) | Scanner API: `scanner_init()` and `gettoken()` |
| [scanner.c](scanner.c) | The scanner. Its header comment describes the number DFA. |
| [parser.h](parser.h) | Parser API: `parse_program()` |
| [parser.c](parser.c) | The parser. Implements all 16 recursive-descent grammar procedures. |
| [scan_test.c](scan_test.c) | A throwaway driver that runs only the scanner on one file. Not part of the final program. |
| `PROJECT SAMPLES-1/` | The instructor's sample inputs and expected outputs. **Never write output into this folder.** Copy it first. |

## The scanner API
```c
#include "scanner.h"   /* also includes token.h and stdio.h */

scanner_init(in, scan_out);  /* in: the source file; scan_out: the ..._output_scan_N.txt file */
Token t = gettoken();        /* returns the next token AND writes its line to scan_out */
```
```c
typedef struct {
    TokenType type;     /* T_IDENTIFIER, T_NUMBER, T_STRING, T_ASSIGN, ..., T_ERROR, T_EOF */
    char lexeme[1024];  /* the matched text, e.g. "discriminant", ":=", "\"hi\"" */
    int line;           /* line the token starts on: use this in parse error messages */
} Token;
```
- `TOKEN_NAMES[t.type]` gives the display name: `"Colon"`, `"Assign"`, `"Semicolon"`, and so on. These are exactly the names that go into `Parse Error on line N: <Name> Expected.`
- Lexical errors come back as `T_ERROR` tokens. The scanner has already written the error message to the scan file, so the parser should treat `T_ERROR` like any other unexpected token.
- After the end of the file, every call returns `T_EOF`. Calling it extra times is safe.
- Keyword tokens: `T_PRINT T_IF T_ELSE T_ENDIF T_SQRT T_AND T_OR T_NOT`. Relational tokens: `T_LT T_EQUAL T_GT T_LTEQUAL T_GTEQUAL T_NOTEQUAL`.

## Setup (both of you)
1. Install gcc on Windows. In PowerShell:
   ```
   winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e
   ```
   Then **restart VS Code or your terminal** and check with `gcc --version`. MSYS2 or lab machines work too.
2. Check that the scanner builds and matches the samples:
   ```
   gcc -Wall -o scan_test scan_test.c scanner.c
   scan_test "PROJECT SAMPLES-1/sample_input_2.txt" test_scan_2.txt
   fc test_scan_2.txt "PROJECT SAMPLES-1/sample_output_scan_2.txt"
   ```
   `fc` should print "no differences encountered". (In Git Bash, use `diff` instead.)

## Member B: Parser ✅ (Done)
**Goal:** make `parser.c` produce the `sample_output_parse_*.txt` files. **Status: Finished and verified across all 9 samples.**

- Public entry point for C to call:
  ```c
  /* parser.h */
  void parse_program(FILE *parse_out, const char *filename);  /* filename goes in "... is a valid SimpCalc program" */
  ```
- Reads tokens with `gettoken()` and stops on the first syntax error.
- Draining the scanner after `parse_program()` is required so that `scan_out` captures all tokens in the file.

## Member C: Driver, tests, submission
**Goal:** one program that runs in a folder and writes a scan file and a parse file for every `*_input*.txt`.

- In `main.c`, for each file whose name contains `_input`:
  1. Build the two output names by replacing `input` with `output_scan` and with `output_parse`.
  2. Open the input with `"r"` and both outputs with `"w"`. Text mode on Windows gives the CRLF line endings the samples use.
  3. Call `scanner_init(in, scan_out)`, then B's `parse_program(parse_out, filename)`.
  4. **Then drain the scanner:** `while (gettoken().type != T_EOF);`. The parser stops at the first error, but the scan file must still list every token. Samples 2 and 4 depend on this.
  5. Close all three files.
- Skip files that already contain `_output_`, so re-running doesn't try to parse its own output.
- Build: `gcc -Wall -o simpcalc main.c scanner.c parser.c`. Don't include `scan_test.c`, because it has its own `main()`.
- Test script: copy only the `sample_input_*.txt` files into a temp folder, run `simpcalc` there, and diff all 18 outputs against the originals.
- Before submitting: write the README (build and run steps), certificate of authorship, and header comments with our names. Name the zip `surname1_surname2_surname3_CSCI70_SCANNER_PARSER.zip`, then unzip it into a clean folder and build from scratch.

## Things that look like bugs but aren't
These are in `scanner.c` on purpose. The instructor's expected output does them, and "fixing" them breaks the diffs for samples 2 and 4:
- A bad number or a lone `!` **eats the next character**. For example, `3.2east` gives an error and then `Identifier st`.
- A newline eaten by an invalid number **isn't counted** as a line.
- An unterminated string counts its newline **twice**.
- The odd spacing in the error lines (`Error  on line` vs `Error   on line`) is copied from the samples exactly.

## Open questions (ask the TA if you get the chance)
- The spec mentions `Invalid Statement`, `Incomplete if Statement` and `Missing relational operator`, but no sample output uses them. Where should they appear, and in what format?

Ping me if the scanner does anything unexpected on your inputs.
