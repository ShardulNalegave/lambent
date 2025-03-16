
#ifndef lambent_scanner_h
#define lambent_scanner_h

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_ERROR,

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_SQUARE,
    TOKEN_RIGHT_SQUARE,

    TOKEN_DOT,

    TOKEN_CONSTANT,
    TOKEN_VARIABLE,
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

static bool scanner_is_at_end();
static token_t scanner_make_token(token_type_t type);
static token_t scanner_make_error_token(const char* msg);
static char scanner_advance();
static char scanner_peek();
static bool scanner_match_next(char c);

#endif
