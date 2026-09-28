/*
 * scanner.h - Public interface of the SimpCalc scanner (lexical analyzer).
 *
 * CSCI 70 Project #1: Scanning and Parsing
 *
 * Usage:
 *     scanner_init(in, scan_out);
 *     Token t;
 *     do { t = gettoken(); ... } while (t.type != T_EOF);
 */
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "token.h"

/*
 * Prepares the scanner to read the source file `in`. Every token that
 * gettoken() returns is also written to `scan_out` (pass NULL to skip
 * logging). Either text or binary mode works for `in`, because CRLF line
 * endings are normalized internally.
 */
void scanner_init(FILE *in, FILE *scan_out);

/*
 * Returns the next token from the input, skipping whitespace and // comments.
 * A lexical error is reported to scan_out and returned as a T_ERROR token.
 * After the end of input, every call returns T_EOF.
 */
Token gettoken(void);

#endif
