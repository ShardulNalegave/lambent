
#ifndef LAMBENT_SCANNER_H
#define LAMBENT_SCANNER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_ERROR,

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,

    TOKEN_DOT,

    TOKEN_IDENT,
    TOKEN_NUMBER,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_CARET,

    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_L,
} token_type_t;

typedef struct {
    token_type_t type;
    uint32_t line;
    const char* start;
    uint32_t len;
} token_t;

typedef struct {
    const char* source;
    const char* start;
    const char* current;
    uint32_t line;
} scanner_t;

void scanner_init(const char* source);
token_t scanner_scan_token();

#endif // LAMBENT_SCANNER_H
