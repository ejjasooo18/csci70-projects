/*
 * parser.h - Public interface of the SimpCalc recursive-descent parser.
 *
 * CSCI 70 Project #1: Scanning and Parsing
 *
 * Usage:
 *     scanner_init(in, scan_out);
 *     parse_program(parse_out, filename);
 */
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"   /* also includes token.h and stdio.h */

/*
 * Parses the token stream from the already-initialised scanner and writes
 * recognition messages plus the result line to parse_out.
 * filename is used only in the final "... is a valid SimpCalc program" line.
 */
void parse_program(FILE *parse_out, const char *filename);

#endif
