
#ifndef LAMBENT_LEXER_H
#define LAMBENT_LEXER_H

#include "stddef.h"
#include "lambent/token.h"
#include "lambent/status.h"

typedef struct lexer {
    const char *source;
    size_t pos;
    size_t line;
    size_t column;
} lexer_t;

lexer_t lexer_create(const char *source);
lambent_status_e lexer_next_token(lexer_t *lexer, token_t *token);

#endif
