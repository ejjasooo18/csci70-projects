/*
 * parse_test.c - Standalone test driver for the SimpCalc parser.
 * Authors: Keith Ayeras, Elijem Timothy Jaso, Dave Predigua
 * CSCI 70 SimpCalc Project: Scanning and Parsing
 *
 * Usage: parse_test <input.txt> <output_parse.txt>
 * Runs the scanner and parser on one input file and writes parse output.
 */
#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"

static const char *get_basename(const char *path)
{
    const char *p = strrchr(path, '/');
    if (!p)
        p = strrchr(path, '\\');
    return p ? (p + 1) : path;
}

int main(int argc, char *argv[])
{
    FILE *in, *outf;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <input> <output>\n", argv[0]);
        return 1;
    }
    in = fopen(argv[1], "r");
    if (!in) {
        perror(argv[1]);
        return 1;
    }
    outf = fopen(argv[2], "w");
    if (!outf) {
        perror(argv[2]);
        fclose(in);
        return 1;
    }

    scanner_init(in, NULL);
    parse_program(outf, get_basename(argv[1]));

    fclose(in);
    fclose(outf);
    return 0;
}
