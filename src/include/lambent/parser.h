
#ifndef LAMBENT_PARSER_H
#define LAMBENT_PARSER_H

#include "lambent/lexer.h"
#include "lambent/status.h"

#include "stddef.h"
#include "stdbool.h"

typedef enum statement_kind {
    STMT_EXPR = 0,
    STMT_LET,
    STMT_COMMAND
} statement_kind_e;

typedef enum expression_kind {
    EXPR_VARIABLE = 0,
    EXPR_FUNCTION,
    EXPR_APPLICATION,
    EXPR_NUMERAL
} expression_kind_e;

typedef struct expression {
    expression_kind_e kind;
    union {
        struct {
            char *name;
        } variable;

        struct {
            char *param;
            struct expression *body;
        } function;

        struct {
            struct expression *func;
            struct expression *arg;
        } application;

        struct {
            int value;
        } numeral;
    };
} expression_t;

typedef struct statement {
    statement_kind_e kind;
    union {
        struct {
            expression_t *expr;
        } expr;

        struct {
            char *name;
            expression_t *value;
        } let;

        struct {
            char *name;
            bool str_arg;
            union {
                char *str;
                expression_t *expr;
            } arg;
        } command;
    };
} statement_t;

typedef struct program {
    size_t count;
    size_t capacity;
    statement_t **statements;
} program_t;

typedef struct parser {
    lexer_t *lexer;
    token_t current_tok;
    program_t program;
} parser_t;

parser_t parser_create(lexer_t *lexer);
program_t* parser_parse_program(parser_t *parser);

void parser_print_program(const program_t *program);

#endif
