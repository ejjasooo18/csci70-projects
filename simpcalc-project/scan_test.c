/*
 * scan_test.c - Standalone driver for testing the scanner on its own.
 *
 * CSCI 70 Project #1: Scanning and Parsing
 *
 * Usage: scan_test <input.txt> <output.txt>
 * Calls gettoken() until EndofFile and writes every token to the output file.
 * This is only for testing; the real program uses main.c.
 */
#include <stdio.h>
#include "scanner.h"

int main(int argc, char *argv[])
{
    FILE *in, *outf;
    Token t;

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

    scanner_init(in, outf);
    do {
        t = gettoken();
    } while (t.type != T_EOF);

    fclose(in);
    fclose(outf);
    return 0;
}
