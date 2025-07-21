
#include "stdio.h"
#include "lambent/lexer.h"

int main() {
    char *source =
        "let result = div 5 2; // Comment\n"
        "#print result;";

    lambent_lexer_t lex = lambent_lexer_create(source);
    lambent_token_t tok;

    while (lambent_lexer_next_token(&lex, &tok) == LAMBENT_LEXER_OK && tok.kind != TOKEN_EOF) {
        printf("%d | %s | %lu | %lu\n", tok.kind, tok.literal, tok.line, tok.column);
    }

    return 0;
}
