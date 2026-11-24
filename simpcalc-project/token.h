/*
 * token.h - Token definitions shared by the SimpCalc scanner and parser.
 * Authors: Keith Ayeras, Elijem Timothy Jaso, Dave Predigua
 * CSCI 70 SimpCalc Project: Scanning and Parsing
 *
 * This is the shared contract between the scanner (scanner.c) and the
 * parser (parser.c). Tell the rest of the team before changing it.
 */
#ifndef TOKEN_H
#define TOKEN_H

#define MAX_LEXEME 1024

typedef enum {
    T_IDENTIFIER, T_NUMBER, T_STRING,
    T_ASSIGN, T_SEMICOLON, T_COLON, T_COMMA,
    T_LPAREN, T_RPAREN,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_RAISE,
    T_LT, T_EQUAL, T_GT, T_LTEQUAL, T_GTEQUAL, T_NOTEQUAL,
    T_PRINT, T_IF, T_ELSE, T_ENDIF, T_SQRT, T_AND, T_OR, T_NOT,
    T_ERROR, T_EOF
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME];
    int line;               /* line on which the token starts */
} Token;

/* Display names, indexed by TokenType (e.g. TOKEN_NAMES[T_GTEQUAL] is "GTEqual"). */
extern const char *TOKEN_NAMES[];

#endif
