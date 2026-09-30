/*
 * parser.c - Recursive-descent parser for the SimpCalc language.
 * Authors: Keith Ayeras, Elijem Timothy Jaso, Dave Predigua
 * CSCI 70 SimpCalc Project: Scanning and Parsing
 *
 * Implements a top-down, recursive-descent parser. Each nonterminal in the
 * grammar has one corresponding C function. The parser reads tokens via
 * gettoken(), writes recognition messages to the parse output file, and
 * stops at the first syntax error using longjmp().
 *
 * Grammar summary:
 *   Prg       -> Blk EndOfFile
 *   Blk       -> Stm Blk | e          (e when cur not in {Identifier,PRINT,IF})
 *   Stm       -> Identifier := Exp ;
 *             |  PRINT ( Arg Argfollow ) ;
 *             |  IF Cnd : Blk Iffollow
 *   Argfollow -> , Arg Argfollow | e
 *   Arg       -> String | Exp
 *   Iffollow  -> ENDIF ; | ELSE Blk ENDIF ;
 *   Exp       -> Trm Trmfollow
 *   Trmfollow -> + Trm Trmfollow | - Trm Trmfollow | e
 *   Trm       -> Fac Facfollow
 *   Facfollow -> * Fac Facfollow | / Fac Facfollow | e
 *   Fac       -> Lit Litfollow
 *   Litfollow -> ** Lit Litfollow | e
 *   Lit       -> - Val | Val
 *   Val       -> Identifier | Number | SQRT ( Exp ) | ( Exp )
 *   Cnd       -> Exp Rel Exp
 *   Rel       -> < | = | > | <= | >= | !=
 */
#include <setjmp.h>
#include "parser.h"

static FILE   *pout;        /* parse output file */
static Token   cur;         /* current lookahead token */
static jmp_buf err_jmp;    /* longjmp target on first parse error */

static void Prg(void);
static void Blk(void);
static void Stm(void);
static void Argfollow(void);
static void Arg(void);
static void Iffollow(void);
static void Exp(void);
static void Trmfollow(void);
static void Trm(void);
static void Facfollow(void);
static void Fac(void);
static void Litfollow(void);
static void Lit(void);
static void Val(void);
static void Cnd(void);
static void Rel(void);

/* Advances cur on match; prints error line and longjmps on mismatch. */
static void match(TokenType expected)
{
    if (cur.type == expected) {
        cur = gettoken();
    } else {
        fprintf(pout, "Symbol expected\n");
        longjmp(err_jmp, 1);
    }
}

/* Rel -> < | = | > | <= | >= | != */
static void Rel(void)
{
    if (cur.type == T_LT || cur.type == T_EQUAL || cur.type == T_GT ||
        cur.type == T_LTEQUAL || cur.type == T_GTEQUAL || cur.type == T_NOTEQUAL) {
        cur = gettoken();
    } else {
        fprintf(pout, "Missing relational operator\n");
        longjmp(err_jmp, 1);
    }
}

/* Val -> Identifier | Number | SQRT ( Exp ) | ( Exp ) */
static void Val(void)
{
    if (cur.type == T_IDENTIFIER) {
        match(T_IDENTIFIER);
    } else if (cur.type == T_NUMBER) {
        match(T_NUMBER);
    } else if (cur.type == T_SQRT) {
        match(T_SQRT);
        match(T_LPAREN);
        Exp();
        match(T_RPAREN);
    } else {
        match(T_LPAREN);
        Exp();
        match(T_RPAREN);
    }
}

/* Lit -> - Val | Val */
static void Lit(void)
{
    if (cur.type == T_MINUS) {
        match(T_MINUS);
        Val();
    } else {
        Val();
    }
}

/* Litfollow -> ** Lit Litfollow | e */
static void Litfollow(void)
{
    if (cur.type == T_RAISE) {
        match(T_RAISE);
        Lit();
        Litfollow();
    }
}

/* Fac -> Lit Litfollow */
static void Fac(void)
{
    Lit();
    Litfollow();
}

/* Facfollow -> * Fac Facfollow | / Fac Facfollow | e */
static void Facfollow(void)
{
    if (cur.type == T_MULTIPLY) {
        match(T_MULTIPLY);
        Fac();
        Facfollow();
    } else if (cur.type == T_DIVIDE) {
        match(T_DIVIDE);
        Fac();
        Facfollow();
    }
}

/* Trm -> Fac Facfollow */
static void Trm(void)
{
    Fac();
    Facfollow();
}

/* Trmfollow -> + Trm Trmfollow | - Trm Trmfollow | e */
static void Trmfollow(void)
{
    if (cur.type == T_PLUS) {
        match(T_PLUS);
        Trm();
        Trmfollow();
    } else if (cur.type == T_MINUS) {
        match(T_MINUS);
        Trm();
        Trmfollow();
    }
}

/* Exp -> Trm Trmfollow */
static void Exp(void)
{
    Trm();
    Trmfollow();
}

/* Cnd -> Exp Rel Exp */
static void Cnd(void)
{
    Exp();
    Rel();
    Exp();
}

/* Arg -> String | Exp */
static void Arg(void)
{
    if (cur.type == T_STRING) {
        match(T_STRING);
    } else {
        Exp();
    }
}

/* Argfollow -> , Arg Argfollow | e */
static void Argfollow(void)
{
    if (cur.type == T_COMMA) {
        match(T_COMMA);
        Arg();
        Argfollow();
    }
}

/* Iffollow -> ELSE Blk ENDIF ; | ENDIF ; */
static void Iffollow(void)
{
    if (cur.type == T_ELSE) {
        match(T_ELSE);
        Blk();
        if (cur.type != T_ENDIF) {
            fprintf(pout, "Incomplete if Statement\n");
            longjmp(err_jmp, 1);
        }
        cur = gettoken();
        if (cur.type != T_SEMICOLON) {
            fprintf(pout, "Incomplete if Statement\n");
            longjmp(err_jmp, 1);
        }
        cur = gettoken();
    } else {
        if (cur.type != T_ENDIF) {
            fprintf(pout, "Incomplete if Statement\n");
            longjmp(err_jmp, 1);
        }
        cur = gettoken();
        if (cur.type != T_SEMICOLON) {
            fprintf(pout, "Incomplete if Statement\n");
            longjmp(err_jmp, 1);
        }
        cur = gettoken();
    }
}

/* Stm -> Identifier := Exp ; | PRINT ( Arg Argfollow ) ; | IF Cnd : Blk Iffollow */
static void Stm(void)
{
    if (cur.type == T_IDENTIFIER) {
        match(T_IDENTIFIER);
        match(T_ASSIGN);
        Exp();
        match(T_SEMICOLON);
        fprintf(pout, "Assignment Statement Recognized\n");
    } else if (cur.type == T_PRINT) {
        match(T_PRINT);
        match(T_LPAREN);
        Arg();
        Argfollow();
        match(T_RPAREN);
        match(T_SEMICOLON);
        fprintf(pout, "Print Statement Recognized\n");
    } else if (cur.type == T_IF) {
        match(T_IF);
        fprintf(pout, "If Statement Begins\n");
        Cnd();
        match(T_COLON);
        Blk();
        Iffollow();
        fprintf(pout, "If Statement Ends\n");
    } else {
        fprintf(pout, "Invalid Statement\n");
        longjmp(err_jmp, 1);
    }
}

/* Blk -> Stm Blk | e  (e when cur not in {Identifier, PRINT, IF}) */
static void Blk(void)
{
    if (cur.type == T_IDENTIFIER || cur.type == T_PRINT || cur.type == T_IF) {
        Stm();
        Blk();
    }
}

/* Prg -> Blk EndOfFile */
static void Prg(void)
{
    Blk();
    match(T_EOF);
}

/* Runs the full parse and writes the result line to parse_out. */
void parse_program(FILE *parse_out, const char *filename)
{
    pout = parse_out;
    cur  = gettoken();

    if (setjmp(err_jmp) == 0) {
        Prg();
        fprintf(pout, "%s is a valid SimpCalc program\n", filename);
    }
    /* On error: match() already wrote the error line before longjmp. */
}
