/*
 * scanner.c - Lexical analyzer for the SimpCalc language.
 *
 * CSCI 70 Project #1: Scanning and Parsing
 *
 * Reads a SimpCalc source file one character at a time and groups the
 * characters into tokens (identifiers, keywords, numbers, strings, operators
 * and punctuation). Whitespace and // comments are skipped. Each token that
 * gettoken() returns is also written to the scanner output file as
 *
 *     <TokenName padded to 31 columns><lexeme>
 *
 * Lexical errors are reported in the output and returned as Error tokens so
 * that scanning can continue.
 *
 * Numbers are recognized with a DFA (see scan_number):
 *
 *     INT  --digit--> INT     INT  --'.'--> DOT     INT  --e/E--> EXP
 *     DOT  --digit--> FRAC    FRAC --digit--> FRAC  FRAC --e/E--> EXP
 *     EXP  --digit--> EXPD    EXP  --'+'/'-'--> SIGN
 *     SIGN --digit--> EXPD    EXPD --digit--> EXPD
 *
 * INT, FRAC and EXPD are accepting states. When DOT, EXP or SIGN sees an
 * invalid character, that character is consumed as part of the bad number.
 */
#include <ctype.h>
#include <string.h>
#include "scanner.h"

const char *TOKEN_NAMES[] = {
    "Identifier", "Number", "String",
    "Assign", "Semicolon", "Colon", "Comma",
    "LeftParen", "RightParen",
    "Plus", "Minus", "Multiply", "Divide", "Raise",
    "LessThan", "Equal", "GreaterThan", "LTEqual", "GTEqual", "NotEqual",
    "Print", "If", "Else", "Endif", "Sqrt", "And", "Or", "Not",
    "Error", "EndofFile"
};

/* Case-sensitive keywords and the token each one maps to. */
static const struct { const char *word; TokenType type; } KEYWORDS[] = {
    {"PRINT", T_PRINT}, {"IF", T_IF}, {"ELSE", T_ELSE}, {"ENDIF", T_ENDIF},
    {"SQRT", T_SQRT}, {"AND", T_AND}, {"OR", T_OR}, {"NOT", T_NOT}
};

/* The kinds of lexical error, each with its own message format. */
typedef enum { ERR_ILLEGAL, ERR_NUMBER, ERR_UNTERMINATED, ERR_BANG } LexError;

static FILE *src;           /* source file being scanned */
static FILE *out;           /* scanner output file (may be NULL) */
static int line;            /* current line number, starting at 1 */
static int pushback;        /* one character of pushback, or NO_CHAR */
static int pending_dot;     /* set when a '.' follows an exponent (see scan_number) */

#define NO_CHAR (-2)

/*
 * Returns the next character, converting "\r\n" to '\n' so the scanner
 * behaves the same whether the file was opened in text or binary mode.
 * Counts lines as newlines are read.
 */
static int readc(void)
{
    int c;
    if (pushback != NO_CHAR) {
        c = pushback;
        pushback = NO_CHAR;
    } else {
        c = fgetc(src);
        if (c == '\r') {
            int next = fgetc(src);
            if (next == '\n')
                c = '\n';
            else if (next != EOF)
                ungetc(next, src);
        }
    }
    if (c == '\n')
        line++;
    return c;
}

/* Puts back the last character read so the next readc() returns it again. */
static void unreadc(int c)
{
    if (c == EOF)
        return;
    if (c == '\n')
        line--;
    pushback = c;
}

/* Writes a token to the scanner output file. */
static void emit(const Token *t)
{
    if (out)
        fprintf(out, "%-31s%s\n", TOKEN_NAMES[t->type], t->lexeme);
}

/* Builds a token of the given type, writes it to the output and returns it. */
static Token make_token(TokenType type, const char *lexeme, int start_line)
{
    Token t;
    t.type = type;
    strncpy(t.lexeme, lexeme, MAX_LEXEME - 1);
    t.lexeme[MAX_LEXEME - 1] = '\0';
    t.line = start_line;
    emit(&t);
    return t;
}

/*
 * Reports a lexical error in the output file and returns an Error token.
 * The message text and spacing match the expected sample outputs exactly.
 */
static Token lex_error(LexError kind, const char *lexeme, int err_line)
{
    Token t;
    t.type = T_ERROR;
    strncpy(t.lexeme, lexeme, MAX_LEXEME - 1);
    t.lexeme[MAX_LEXEME - 1] = '\0';
    t.line = err_line;
    if (out) {
        switch (kind) {
        case ERR_ILLEGAL:
            fprintf(out, "Lexical Error: Illegal character/character sequence   on line %d\n", err_line);
            fprintf(out, "Error  on line %d\n", err_line);
            break;
        case ERR_NUMBER:
            fprintf(out, "Lexical Error: Invalid number format   on line %d\n", err_line);
            fprintf(out, "Error   on line %d\n", err_line);
            break;
        case ERR_UNTERMINATED:
            fprintf(out, "Lexical Error: Unterminated  on line %d\n", err_line);
            fprintf(out, "Error   on line %d\n", err_line);
            break;
        case ERR_BANG:
            fprintf(out, "Lexical Error reading character ! on line %d\n", err_line);
            fprintf(out, "Error  on line %d\n", err_line);
            break;
        }
    }
    return t;
}

/* Appends character c to the lexeme buffer of length *len, if there is room. */
static void append(char *buf, int *len, int c)
{
    if (*len < MAX_LEXEME - 1) {
        buf[(*len)++] = (char)c;
        buf[*len] = '\0';
    }
}

void scanner_init(FILE *in, FILE *scan_out)
{
    src = in;
    out = scan_out;
    line = 1;
    pushback = NO_CHAR;
    pending_dot = 0;
}

/* Skips whitespace and // comments. Returns the first character after them. */
static int skip_blanks(void)
{
    int c;
    for (;;) {
        c = readc();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v')
            continue;
        if (c == '/') {
            int next = readc();
            if (next == '/') {
                while ((c = readc()) != '\n' && c != EOF)
                    ;
                if (c == EOF)
                    return EOF;
                continue;
            }
            unreadc(next);
        }
        return c;
    }
}

/* Scans an identifier or keyword whose first character is c. */
static Token scan_word(int c, int start_line)
{
    char buf[MAX_LEXEME];
    int len = 0;
    size_t i;

    buf[0] = '\0';
    while (c != EOF && (isalnum(c) || c == '_')) {
        append(buf, &len, c);
        c = readc();
    }
    unreadc(c);

    for (i = 0; i < sizeof KEYWORDS / sizeof KEYWORDS[0]; i++)
        if (strcmp(buf, KEYWORDS[i].word) == 0)
            return make_token(KEYWORDS[i].type, buf, start_line);
    return make_token(T_IDENTIFIER, buf, start_line);
}

/*
 * Scans a number whose first digit is c, using the DFA described at the top
 * of this file.
 */
static Token scan_number(int c, int start_line)
{
    enum { INT, DOT, FRAC, EXP, SIGN, EXPD } state = INT;
    char buf[MAX_LEXEME];
    int len = 0;

    buf[0] = '\0';
    append(buf, &len, c);
    for (;;) {
        c = readc();
        switch (state) {
        case INT:
        case FRAC:
            if (isdigit(c))       { append(buf, &len, c); continue; }
            if (c == '.' && state == INT) { append(buf, &len, c); state = DOT; continue; }
            if (c == 'e' || c == 'E') { append(buf, &len, c); state = EXP; continue; }
            unreadc(c);
            return make_token(T_NUMBER, buf, start_line);

        case DOT:
            if (isdigit(c)) { append(buf, &len, c); state = FRAC; continue; }
            break;                  /* invalid number */

        case EXP:
            if (isdigit(c))           { append(buf, &len, c); state = EXPD; continue; }
            if (c == '+' || c == '-') { append(buf, &len, c); state = SIGN; continue; }
            break;                  /* invalid number */

        case SIGN:
            if (isdigit(c)) { append(buf, &len, c); state = EXPD; continue; }
            break;                  /* invalid number */

        case EXPD:
            if (isdigit(c)) { append(buf, &len, c); continue; }
            /*
             * A '.' after the exponent (e.g. "111.222e333.444") ends the number,
             * and the '.' itself is then reported as an invalid number.
             */
            if (c == '.')
                pending_dot = 1;
            unreadc(c);
            return make_token(T_NUMBER, buf, start_line);
        }

        /*
         * Invalid number: the offending character is consumed as part of it.
         * To match the reference output, a newline swallowed here is not
         * counted as a line.
         */
        if (c == EOF)
            return lex_error(ERR_NUMBER, buf, start_line);
        append(buf, &len, c);
        if (c == '\n')
            line--;
        return lex_error(ERR_NUMBER, buf, start_line);
    }
}

/* Scans a string literal. The opening quote has already been read. */
static Token scan_string(int start_line)
{
    char buf[MAX_LEXEME];
    int len = 0;
    int c;

    buf[0] = '\0';
    append(buf, &len, '"');
    for (;;) {
        c = readc();
        if (c == '"') {
            append(buf, &len, c);
            return make_token(T_STRING, buf, start_line);
        }
        if (c == '\n' || c == EOF) {
            /*
             * Strings cannot span lines. To match the reference output, the
             * newline that ends an unterminated string is counted twice.
             */
            if (c == '\n')
                line++;
            return lex_error(ERR_UNTERMINATED, buf, start_line);
        }
        append(buf, &len, c);
    }
}

Token gettoken(void)
{
    int c, next, start_line;

    if (pending_dot) {
        pending_dot = 0;
        start_line = line;
        readc();                    /* the '.' left over from scan_number */
        return lex_error(ERR_NUMBER, ".", start_line);
    }

    c = skip_blanks();
    start_line = line;

    if (c == EOF)
        return make_token(T_EOF, " ", start_line);
    if (isalpha(c) || c == '_')
        return scan_word(c, start_line);
    if (isdigit(c))
        return scan_number(c, start_line);
    if (c == '"')
        return scan_string(start_line);

    switch (c) {
    case ';': return make_token(T_SEMICOLON, ";", start_line);
    case ',': return make_token(T_COMMA, ",", start_line);
    case '(': return make_token(T_LPAREN, "(", start_line);
    case ')': return make_token(T_RPAREN, ")", start_line);
    case '+': return make_token(T_PLUS, "+", start_line);
    case '-': return make_token(T_MINUS, "-", start_line);
    case '/': return make_token(T_DIVIDE, "/", start_line);   /* "//" was handled by skip_blanks */
    case '=': return make_token(T_EQUAL, "=", start_line);
    case ':':
        next = readc();
        if (next == '=') return make_token(T_ASSIGN, ":=", start_line);
        unreadc(next);
        return make_token(T_COLON, ":", start_line);
    case '*':
        next = readc();
        if (next == '*') return make_token(T_RAISE, "**", start_line);
        unreadc(next);
        return make_token(T_MULTIPLY, "*", start_line);
    case '<':
        next = readc();
        if (next == '=') return make_token(T_LTEQUAL, "<=", start_line);
        unreadc(next);
        return make_token(T_LT, "<", start_line);
    case '>':
        next = readc();
        if (next == '=') return make_token(T_GTEQUAL, ">=", start_line);
        unreadc(next);
        return make_token(T_GT, ">", start_line);
    case '!':
        next = readc();
        if (next == '=') return make_token(T_NOTEQUAL, "!=", start_line);
        /* A lone '!' is an error, and the character after it is consumed too. */
        if (next == '\n' || next == EOF)
            unreadc(next);
        return lex_error(ERR_BANG, "!", start_line);
    default: {
        char bad[2] = { (char)c, '\0' };
        return lex_error(ERR_ILLEGAL, bad, start_line);
    }
    }
}
