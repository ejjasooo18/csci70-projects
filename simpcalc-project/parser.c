/*
 * parser.c - Recursive-descent parser for the SimpCalc language.
 *
 * CSCI 70 Project #1: Scanning and Parsing
 *
 * Implements a top-down, recursive-descent parser. Each nonterminal in the
 * grammar has one corresponding C function. The parser reads tokens via
 * gettoken(), writes recognition messages to the parse output file, and
 * stops at the first syntax error using longjmp().
 *
 * Grammar summary:
 *   Prg       -> Blk
 *   Blk       -> Stm Blk | e          (e when cur not in {Identifier,PRINT,IF})
 *   Stm       -> Identifier := Exp ;
 *             |  PRINT ( Arg Argfollow ) ;
 *             |  IF Cnd : Blk Iffollow ;
 *   Argfollow -> , Arg Argfollow | e
 *   Arg       -> Exp
 *   Iffollow  -> ELSE Blk ENDIF | ENDIF
 *   Exp       -> - Trm | Trm
 *   Trmfollow -> + Trm Trmfollow | - Trm Trmfollow | e
 *   Trm       -> Fac Trmfollow
 *   Facfollow -> * Fac Facfollow | / Fac Facfollow | e
 *   Fac       -> Lit Facfollow
 *   Litfollow -> ** Lit Litfollow | e
 *   Lit       -> Val Litfollow
 *   Val       -> Identifier | Number | String | SQRT ( Exp ) | ( Exp )
 *   Cnd       -> Exp Rel Exp
 *   Rel       -> < | = | > | <= | >= | !=
 */
#include <setjmp.h>
#include "parser.h"

static FILE   *pout;        /* parse output file                              */
static Token   cur;         /* current lookahead token                        */
static jmp_buf err_jmp;    /* longjmp target on first parse error            */

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
        fprintf(pout, "Parse Error on line %d: %s Expected.",
                cur.line, TOKEN_NAMES[expected]);
        longjmp(err_jmp, 1);
    }
}

/* Rel -> < | = | > | <= | >= | != */
static void Rel(void)
{
    /* TODO: implement */
}

/* Val -> Identifier | Number | String | SQRT(Exp) | (Exp) */
static void Val(void)
{
    /* TODO: implement */
}

/* Litfollow -> ** Lit Litfollow | e */
static void Litfollow(void)
{
    /* TODO: implement */
}

/* Lit -> Val Litfollow */
static void Lit(void)
{
    /* TODO: implement */
}

/* Facfollow -> * Fac Facfollow | / Fac Facfollow | e */
static void Facfollow(void)
{
    /* TODO: implement */
}

/* Fac -> Lit Facfollow */
static void Fac(void)
{
    /* TODO: implement */
}

/* Trmfollow -> + Trm Trmfollow | - Trm Trmfollow | e */
static void Trmfollow(void)
{
    /* TODO: implement */
}

/* Trm -> Fac Trmfollow */
static void Trm(void)
{
    /* TODO: implement */
}

/* Exp -> - Trm | Trm */
static void Exp(void)
{
    /* TODO: implement */
}

/* Cnd -> Exp Rel Exp */
static void Cnd(void)
{
    /* TODO: implement */
}

/* Arg -> Exp */
static void Arg(void)
{
    /* TODO: implement */
}

/* Argfollow -> , Arg Argfollow | e */
static void Argfollow(void)
{
    /* TODO: implement */
}

/* Iffollow -> ELSE Blk ENDIF | ENDIF; prints "If Statement Ends" after ENDIF. */
static void Iffollow(void)
{
    /* TODO: implement */
}

/* Stm -> Identifier := Exp ; | PRINT(Arg Argfollow) ; | IF Cnd : Blk Iffollow ; */
static void Stm(void)
{
    /* TODO: implement */
}

/* Blk -> Stm Blk | e  (e when cur not in {Identifier, PRINT, IF}) */
static void Blk(void)
{
    /* TODO: implement */
}

/* Prg -> Blk, then expects T_EOF. */
static void Prg(void)
{
    /* TODO: implement */
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
