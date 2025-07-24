
#ifndef LAMBENT_PARSER_H
#define LAMBENT_PARSER_H

#include "lambent/lexer.h"
#include "stddef.h"
#include "stdbool.h"

typedef enum lambent_statement_kind {
    STMT_EXPR = 0,
    STMT_LET,
    STMT_COMMAND
} lambent_statement_kind_e;

typedef enum lambent_expression_kind {
    EXPR_VARIABLE = 0,
    EXPR_FUNCTION,
    EXPR_APPLICATION,
    EXPR_NUMERAL
} lambent_expression_kind_e;

typedef struct lambent_expression {
    lambent_expression_kind_e kind;
    union {
        struct {
            char *name;
        } variable;

        struct {
            char *param;
            struct lambent_expression *body;
        } function;

        struct {
            struct lambent_expression *func;
            struct lambent_expression *arg;
        } application;

        struct {
            int value;
        } numeral;
    };
} lambent_expression_t;

typedef struct lambent_statement {
    lambent_statement_kind_e kind;
    union {
        struct {
            lambent_expression_t *expr;
        } expr;

        struct {
            char *name;
            lambent_expression_t *value;
        } let;

        struct {
            char *name;
            bool str_arg;
            union {
                char *str;
                lambent_expression_t *expr;
            } arg;
        } command;
    };
} lambent_statement_t;

typedef struct lambent_program {
    size_t count;
    size_t capacity;
    lambent_statement_t **statements;
} lambent_program_t;

typedef struct lambent_parser {
    lambent_lexer_t *lexer;
    lambent_token_t current_tok;
    lambent_program_t program;
} lambent_parser_t;

lambent_parser_t lambent_parser_create(lambent_lexer_t *lexer);
lambent_program_t* lambent_parser_parse_program(lambent_parser_t *parser);

void lambent_parser_print_program(const lambent_program_t *program);

#endif
