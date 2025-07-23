
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
        case TOKEN_IDENTIFIER: {
            lambent_token_t ident = parser->current_tok;
            advance(parser);

            if (parser->current_tok.kind == TOKEN_FAT_ARROW) {
                advance(parser);
                lambent_expression_t *body = parse_expression(parser);

                lambent_expression_t *expr = malloc(sizeof(lambent_expression_t));
                expr->kind = EXPR_FUNCTION;
                expr->function.param = ident.literal;
                expr->function.body = body;
                return expr;
            }
            
            lambent_expression_t *expr = malloc(sizeof(lambent_expression_t));
            expr->kind = EXPR_VARIABLE;
            expr->variable.name = ident.literal;
            return expr;
        }

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

// Forward declarations
static lambent_statement_t* parse_statement(lambent_parser_t *parser);
static lambent_statement_t* parse_expr_statement(lambent_parser_t *parser);
static lambent_statement_t* parse_let_statement(lambent_parser_t *parser);
static lambent_statement_t* parse_command_statement(lambent_parser_t *parser);

static lambent_statement_t* parse_statement(lambent_parser_t *parser) {
    if (parser->current_tok.kind == TOKEN_LET) {
        return parse_let_statement(parser);
    } else if (parser->current_tok.kind == TOKEN_COMMAND) {
        return parse_command_statement(parser);
    } else {
        return parse_expr_statement(parser);
    }
}

static lambent_statement_t* parse_expr_statement(lambent_parser_t *parser) {
    lambent_expression_t *expr = parse_expression(parser);

    if (parser->current_tok.kind != TOKEN_SEMICOLON) {
        // TODO: Throw unexpected token error
        return NULL;
    }

    lambent_statement_t *stmt = malloc(sizeof(lambent_statement_t));
    stmt->kind = STMT_EXPR;
    stmt->expr.expr = expr;
    return stmt;
}

static lambent_statement_t* parse_let_statement(lambent_parser_t *parser) {
    advance(parser); // discard let token
    
    if (parser->current_tok.kind != TOKEN_IDENTIFIER) {
        // TODO: Throw unexpected token error
        return NULL;
    }

    lambent_token_t name = parser->current_tok;
    advance(parser);

    if (parser->current_tok.kind != TOKEN_ASSIGN) {
        // TODO: Throw unexpected token error
        return NULL;
    }

    lambent_expression_t *value = parse_expression(parser);

    if (parser->current_tok.kind != TOKEN_SEMICOLON) {
        // TODO: Throw unexpected token error
        return NULL;
    }

    lambent_statement_t *stmt = malloc(sizeof(lambent_statement_t));
    stmt->kind = STMT_LET;
    stmt->let.name = name.literal;
    stmt->let.value = value;
    return stmt;
}

static lambent_statement_t* parse_command_statement(lambent_parser_t *parser) {
    lambent_token_t name = parser->current_tok;
    advance(parser);

    lambent_statement_t *stmt = malloc(sizeof(lambent_statement_t));
    stmt->kind = STMT_COMMAND;
    stmt->command.name = name.literal;

    if (parser->current_tok.kind == TOKEN_STRING) {
        stmt->command.str_arg = true;
        stmt->command.arg.str = parser->current_tok.literal;
        advance(parser);
    } else {
        stmt->command.str_arg = false;
        stmt->command.arg.expr = parse_expression(parser);
    }

    if (parser->current_tok.kind != TOKEN_SEMICOLON) {
        // TODO: Throw unexpected token error
        return NULL;
    }

    return stmt;
}

lambent_parser_t lambent_parser_create(lambent_lexer_t *lexer) {
    lambent_parser_t parser;
    parser.lexer = lexer;
    lambent_lexer_next_token(lexer, &parser.current_tok);
    return parser;
}

lambent_program_t* lambent_parser_parse_program(lambent_parser_t *parser) {
    lambent_program_t *program = malloc(sizeof(lambent_program_t));
    program->statements = malloc(sizeof(lambent_statement_t*) * 16);
    program->count = 0;
    program->capacity = 16;
    
    while (parser->current_tok.kind != TOKEN_EOF) {
        lambent_statement_t *stmt = parse_statement(parser);
        if (!stmt) {
            printf("Failed to parse statement\n");
            return NULL;
        }
        
        // Resize array if needed
        if (program->count >= program->capacity) {
            program->capacity *= 2;
            program->statements = realloc(
                program->statements, 
                sizeof(lambent_statement_t*) * program->capacity
            );
        }
        
        program->statements[program->count++] = stmt;
    }
    
    return program;
}
