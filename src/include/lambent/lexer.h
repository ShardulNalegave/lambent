
#ifndef LAMBENT_LEXER_H
#define LAMBENT_LEXER_H

#include "stddef.h"
#include "lambent/token.h"

typedef enum lambent_lexer_status {
    LAMBENT_LEXER_OK,
    LAMBENT_LEXER_UNEXPECTED_CHAR,
    LAMBENT_LEXER_UNTERMINATED_STRING,
} lambent_lexer_status_e;

typedef struct lambent_lexer {
    const char *source;
    size_t pos;
    size_t line;
    size_t column;
} lambent_lexer_t;

lambent_lexer_t lambent_lexer_create(const char *source);
lambent_lexer_status_e lambent_lexer_next_token(lambent_lexer_t *lexer, lambent_token_t *token);

#endif
