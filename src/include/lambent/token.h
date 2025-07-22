
#ifndef LAMBENT_TOKEN_H
#define LAMBENT_TOKEN_H

#include "stddef.h"

typedef enum lambent_token_kind {
    TOKEN_EOF = 0,

    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_SEMICOLON,
    TOKEN_ASSIGN,
    TOKEN_FAT_ARROW,

    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_COMMAND,

    TOKEN_LET
} lambent_token_kind_e;

typedef struct lambent_token {
    lambent_token_kind_e kind;
    char *literal;
    size_t line;
    size_t column;
} lambent_token_t;

#endif
