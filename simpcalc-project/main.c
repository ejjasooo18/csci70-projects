#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "scanner.h"
#include "parser.h"

/*
 * main.c - Driver for the SimpCalc scanner and parser.
 *
 * Authors: Keith Ayeras, Elijem Timothy Jaso, Dave Predigua
 * CSCI 70 SimpCalc Project: Scanning and Parsing
 *
 * This program finds all files containing "_input" in their name in the current directory.
 * For each one, it generates the corresponding scanner and parser output files,
 * then processes the input through the scanner and parser.
 */

void process_file(const char *filename) {
    char scan_out_name[256];
    char parse_out_name[256];
    
    // Replace "_input" with "_output_scan" and "_output_parse"
    const char *input_pos = strstr(filename, "_input");
    if (!input_pos) return;
    
    size_t prefix_len = input_pos - filename;
    const char *suffix = input_pos + 6; // 6 is length of "_input"
    
    snprintf(scan_out_name, sizeof(scan_out_name), "%.*s_output_scan%s", (int)prefix_len, filename, suffix);
    snprintf(parse_out_name, sizeof(parse_out_name), "%.*s_output_parse%s", (int)prefix_len, filename, suffix);
    
    FILE *in = fopen(filename, "r");
    if (!in) {
        printf("Could not open input file: %s\n", filename);
        return;
    }
    
    FILE *scan_out = fopen(scan_out_name, "w");
    if (!scan_out) {
        printf("Could not open scanner output file: %s\n", scan_out_name);
        fclose(in);
        return;
    }
    
    scanner_init(in, scan_out);
    
    int lex_error = 0;
    Token t;
    do {
        t = gettoken();
        if (t.type == T_ERROR) {
            lex_error = 1;
            break;
        }
    } while (t.type != T_EOF);
    
    FILE *parse_out = fopen(parse_out_name, "w");
    if (!parse_out) {
        printf("Could not open parser output file: %s\n", parse_out_name);
        fclose(in);
        fclose(scan_out);
        return;
    }
    
    if (!lex_error) {
        fseek(in, 0, SEEK_SET);
        fclose(scan_out);
        scan_out = fopen(scan_out_name, "w");
        scanner_init(in, scan_out);
        
        parse_program(parse_out, filename);
        
        // Drain scanner
        while (gettoken().type != T_EOF);
    }
    
    fclose(in);
    fclose(scan_out);
    fclose(parse_out);
    printf("Processed %s\n", filename);
}

int main() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, "_input") && !strstr(dir->d_name, "_output_")) {
                process_file(dir->d_name);
            }
        }
        closedir(d);
    } else {
        printf("Could not open current directory.\n");
        return 1;
    }
    
    return 0;
}
