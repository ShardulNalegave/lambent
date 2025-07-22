
#include "lambent/parser.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static inline void advance(lambent_parser_t *parser) {
    lambent_lexer_next_token(parser->lexer, &parser->current_tok);
}

// Forward declarations
static lambent_expression_t* parse_expression(lambent_parser_t *parser);
static lambent_expression_t* parse_application(lambent_parser_t *parser);
static lambent_expression_t* parse_atom(lambent_parser_t *parser);

static lambent_expression_t* parse_expression(lambent_parser_t *parser) {
    return parse_application(parser);
}

static lambent_expression_t* parse_application(lambent_parser_t *parser) {
    lambent_expression_t *left = parse_atom(parser);

    while (
        parser->current_tok.kind == TOKEN_IDENTIFIER ||
        parser->current_tok.kind == TOKEN_NUMBER || 
        parser->current_tok.kind == TOKEN_LPAREN
    ) {
        lambent_expression_t *right = parse_atom(parser);

        lambent_expression_t *app = malloc(sizeof(lambent_expression_t));
        app->kind = EXPR_APPLICATION;
        app->application.func = left;
        app->application.arg = right;

        left = app;
    }

    return left;
}

static lambent_expression_t* parse_atom(lambent_parser_t *parser) {
    switch (parser->current_tok.kind) {
        case TOKEN_NUMBER: {
            lambent_expression_t *expr = malloc(sizeof(lambent_expression_t));
            expr->kind = EXPR_NUMERAL;
            expr->numeral.value = atoi(parser->current_tok.literal);
            advance(parser);
            return expr;
        }

        case TOKEN_LPAREN: {
            advance(parser);
            lambent_expression_t *expr = parse_expression(parser);
            if (parser->current_tok.kind != TOKEN_RPAREN) {
                // TODO: Report error here
                return NULL;
            }
            advance(parser);
            return expr;
        }

        default: {
            // TODO: Improve error reporting here
            printf("Unexpected token\n");
            return NULL;
        }
    }
}

lambent_parser_t lambent_parser_create(lambent_lexer_t *lexer) {
    lambent_parser_t parser;
    parser.lexer = lexer;
    lambent_lexer_next_token(lexer, &parser.current_tok);
    return parser;
}
