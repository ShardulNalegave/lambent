
#include "lambent/lexer.h"

#include "stdio.h"
#include "inttypes.h"
#include "ctype.h"
#include "string.h"

static void unexpected_character_error(char expected, char found, size_t line, size_t column) {
    printf("[LEXER ERROR] Unexpected character '%c' at line %lu column %lu\n", found, line, column);
    if (expected != '\0') printf("\tExpected character: '%c'\n", expected);
}

static inline char peek(lambent_lexer_t *lexer) {
    return lexer->source[lexer->pos];
}

static inline char advance(lambent_lexer_t *lexer) {
    char c = lexer->source[lexer->pos++];
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

static inline lambent_token_t make_token(
    lambent_token_kind_e kind,
    char *literal,
    size_t line,
    size_t col
) {
    lambent_token_t tok;
    tok.kind = kind;
    tok.literal = literal;
    tok.line = line;
    tok.column = col;
    return tok;
}

static void skip_whitespace(lambent_lexer_t *lexer) {
    while (isspace(peek(lexer))) {
        advance(lexer);
    }
}

static void skip_comment(lambent_lexer_t *lexer) {
    while(peek(lexer) != '\n') {
        if (peek(lexer) == '\0') return;
        advance(lexer);
    }
}

static lambent_token_t read_number(lambent_lexer_t *lexer) {
    size_t start_column = lexer->column - 1;
    size_t start = lexer->pos - 1;

    while (isdigit(peek(lexer))) {
        advance(lexer);
    }

    int len = lexer->pos - start;
    char *literal = strndup(lexer->source + start, len);

    return make_token(TOKEN_NUMBER, literal, lexer->line, start_column);
}

static lambent_token_t read_identifier(lambent_lexer_t *lexer) {
    size_t start_column = lexer->column - 1;
    size_t start = lexer->pos - 1;

    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }

    int len = lexer->pos - start;
    char *literal = strndup(lexer->source + start, len);

    if (strcmp(literal, "let") == 0) {
        return make_token(TOKEN_LET, literal, lexer->line, start_column);
    }

    return make_token(TOKEN_IDENTIFIER, literal, lexer->line, start_column);
}

static lambent_token_t read_command(lambent_lexer_t *lexer) {
    size_t start_column = lexer->column - 1;
    size_t start = lexer->pos - 1;

    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }

    int len = lexer->pos - start;
    char *literal = strndup(lexer->source + start, len);

    return make_token(TOKEN_COMMAND, literal, lexer->line, start_column);
}

static lambent_lexer_status_e read_string(lambent_lexer_t *lexer, lambent_token_t *token) {
    size_t start_column = lexer->column; // no need to include the start quote
    size_t start_line = lexer->line;
    size_t start = lexer->pos;

    while (peek(lexer) != '"') {
        if (peek(lexer) == '\0') {
            printf("[LEXER ERROR] Unterminated string\n\tLine: %lu, Column: %lu", start_line, start_column);
            return LAMBENT_LEXER_UNTERMINATED_STRING;
        }

        advance(lexer);
    }

    int len = lexer->pos - start;
    char *literal = strndup(lexer->source + start, len);

    // TODO: Handle escape sequences in the literal

    advance(lexer); // discard the terminating quote

    *token = make_token(TOKEN_NUMBER, literal, start_line, start_column);
    token->line = start_line;

    return LAMBENT_LEXER_OK;
}

lambent_lexer_t lambent_lexer_create(const char *source) {
    lambent_lexer_t lexer;
    lexer.source = source;
    lexer.pos = 0;
    lexer.line = 1;
    lexer.column = 1;
    return lexer;
}

lambent_lexer_status_e lambent_lexer_next_token(lambent_lexer_t *lexer, lambent_token_t *token) {
    skip_whitespace(lexer);
    
    if (peek(lexer) == '\0') {
        *token = make_token(TOKEN_EOF, NULL, lexer->line, lexer->column);
        return LAMBENT_LEXER_OK;
    }

    size_t start_column = lexer->column;
    char c = advance(lexer);

    if (c == '(') {
        *token =  make_token(TOKEN_LPAREN, "(", lexer->line, start_column);
    } else if (c == ')') {
        *token = make_token(TOKEN_RPAREN, ")", lexer->line, start_column);
    } else if (c == ';') {
        *token = make_token(TOKEN_SEMICOLON, ";", lexer->line, start_column);
    } else if (c == '=') {
        if (peek(lexer) == '>') {
            advance(lexer);
            *token = make_token(TOKEN_FAT_ARROW, "=>", lexer->line, start_column);
        }
        else *token = make_token(TOKEN_ASSIGN, "=", lexer->line, start_column);
    } else if (c == '/') {
        if (peek(lexer) == '/') {
            skip_comment(lexer);
            return lambent_lexer_next_token(lexer, token);
        } else {
            unexpected_character_error('\0', c, lexer->line, start_column);
            return LAMBENT_LEXER_UNEXPECTED_CHAR;
        }
    } else if (c == '"') {
        return read_string(lexer, token);
    } else if (c == '#') {
        *token = read_command(lexer);
    } else if (isdigit(c)) {
        *token = read_number(lexer);
    } else if (isalpha(c) || c == '_') {
        *token = read_identifier(lexer);
    } else {
        unexpected_character_error('\0', c, lexer->line, start_column);
        return LAMBENT_LEXER_UNEXPECTED_CHAR;
    }

    return LAMBENT_LEXER_OK;
}
