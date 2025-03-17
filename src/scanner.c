
#include "scanner.h"
#include "string.h"

scanner_t scanner;

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool scanner_is_at_end() {
    return *scanner.current == '\0';
}

static char scanner_advance() {
    return *(scanner.current++);
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
    token.line = scanner.line;
    return token;
}

static void scanner_skip_whitespace() {
    for (;;) {
        char c = scanner_peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                scanner_advance();
                break;
            case '\n':
                scanner_advance();
                scanner.line++;
                break;
            default:
                return;
        }
    }
}

static token_t scanner_read_identifier() {
    while (is_alpha(scanner_peek()) || is_digit(scanner_peek())) scanner_advance();
    return scanner_make_token(TOKEN_IDENT);
}

static token_t scanner_read_number() {
    while (is_digit(scanner_peek())) scanner_advance();
    return scanner_make_token(TOKEN_NUMBER);
}

void scanner_init(const char* source) {
    scanner.source = source;
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

token_t scanner_scan_token() {
    scanner_skip_whitespace();

    scanner.start = scanner.current;
    if (scanner_is_at_end()) return scanner_make_token(TOKEN_EOF);

    char c = scanner_advance();

    if (c == 'L') return scanner_make_token(TOKEN_L);
    if (is_alpha(c)) return scanner_read_identifier();
    if (is_digit(c)) return scanner_read_number();

    switch (c) {
        case '(': return scanner_make_token(TOKEN_LEFT_PAREN);
        case ')': return scanner_make_token(TOKEN_RIGHT_PAREN);
        case '.': return scanner_make_token(TOKEN_DOT);
        case '+': return scanner_make_token(TOKEN_PLUS);
        case '-': return scanner_make_token(TOKEN_MINUS);
        case '*': return scanner_make_token(TOKEN_STAR);
        case '/': return scanner_make_token(TOKEN_SLASH);
        case '^': return scanner_make_token(TOKEN_CARET);

        case '&': {
            if (!scanner_is_at_end() && scanner_peek() == '&')
                return scanner_make_token(TOKEN_AND);
            break;
        };
        case '|': {
            if (!scanner_is_at_end() && scanner_peek() == '|')
                return scanner_make_token(TOKEN_OR);
            break;
        };
        case '=': {
            if (!scanner_is_at_end() && scanner_peek() == '=')
                return scanner_make_token(TOKEN_EQUAL);
            break;
        };
        case '!': {
            if (!scanner_is_at_end() && scanner_peek() == '=')
                return scanner_make_token(TOKEN_NOT_EQUAL);
            return scanner_make_token(TOKEN_NOT);
        };
    }

    return scanner_make_error_token("Unexpected Character");
}