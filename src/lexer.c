
#include "lambent/lexer.h"

#include "ctype.h"

static char peek(lambent_lexer_t *lexer) {
    return lexer->source[lexer->pos];
}

static void advance(lambent_lexer_t *lexer) {
    lexer->pos++;
}

static void skip_whitespace(lambent_lexer_t *lexer) {
    char c = peek(lexer);
    while (c && isspace(c)) {
        if (c == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        advance(lexer);
    }
}

static lambent_token_t make_token(lambent_lexer_t *lexer, lambent_token_kind_e kind, char *literal) {
    lambent_token_t tok;
    tok.kind = kind;
    tok.literal = literal;
    tok.line = lexer->line;
    tok.column = lexer->column;
    return tok;
}

lambent_lexer_t lambent_lexer_create(const char *source) {
    lambent_lexer_t lexer;
    lexer.source = source;
    lexer.pos = 0;
    lexer.line = 1;
    lexer.column = 1;
    return lexer;
}

lambent_token_t lambent_lexer_next_token(lambent_lexer_t *lexer) {
    skip_whitespace(lexer);

    char c = peek(lexer);
    
    if (c == '\0') {
        return make_token(lexer, TOKEN_EOF, '\0');
    }
}
