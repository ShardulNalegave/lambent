
#include "scanner.h"
#include "string.h"

scanner_t scanner;

void scanner_init(const char* source) {
    scanner.source = source;
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

token_t scanner_scan_token() {
    scanner.start = scanner.current;
    if (scanner_is_at_end()) return scanner_make_token(TOKEN_EOF);

    char c = scanner_advance();
    switch (c) {
        case '(': return scanner_make_token(TOKEN_LEFT_PAREN);
        case ')': return scanner_make_token(TOKEN_RIGHT_PAREN);
        case '[': return scanner_make_token(TOKEN_LEFT_SQUARE);
        case ']': return scanner_make_token(TOKEN_RIGHT_SQUARE);
    }

    return scanner_make_error_token("Unexpected Character");
}

static bool scanner_is_at_end() {
    return *scanner.current == '\0';
}

static token_t scanner_make_token(token_type_t type) {
    token_t token;
    token.type = type;
    token.start = scanner.start;
    token.len = (uint32_t)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static token_t scanner_make_error_token(const char* msg) {
    token_t token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.len = (uint32_t)strlen(msg);
    token.len = scanner.line;
    return token;
}

static char scanner_advance() {
    return *(++scanner.current);
}

static char scanner_peek() {
    return *scanner.current;
}

static bool scanner_match_next(char c) {
    if (scanner_is_at_end()) return false;
    if (scanner_peek() != c) return false;
    scanner.current++;
    return true;
}