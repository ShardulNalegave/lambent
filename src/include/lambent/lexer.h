
#ifndef LAMBENT_LEXER_H
#define LAMBENT_LEXER_H

#include "stddef.h"
#include "lambent/token.h"

typedef struct lambent_lexer {
    const char *source;
    size_t pos;
    size_t line;
    size_t column;
} lambent_lexer_t;

lambent_lexer_t lambent_lexer_create(const char *source);
lambent_token_t lambent_lexer_next_token(lambent_lexer_t *lexer);

#endif
