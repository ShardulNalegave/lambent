
#ifndef LAMBENT_STATUS_H
#define LAMBENT_STATUS_H

#include "stdio.h"

typedef enum lambent_status {
    LAMBENT_OK,
    
    LEXER_UNEXPECTED_CHAR,
    LEXER_UNTERMINATED_STRING,
} lambent_status_e;

static inline void unexpected_character_error(char expected, char found, size_t line, size_t column) {
    printf("[LEXER ERROR] Unexpected character '%c' at line %lu column %lu\n", found, line, column);
    if (expected != '\0') printf("\tExpected character: '%c'\n", expected);
}

#endif
