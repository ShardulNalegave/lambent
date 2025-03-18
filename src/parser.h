
#ifndef LAMBENT_PARSER_H
#define LAMBENT_PARSER_H

#include "stdint.h"

typedef enum {
    EXPR_IDENT,
    EXPR_FUNCTION,
    EXPR_APPLICATION,
} expr_type_t;

typedef struct {
    expr_type_t type;
    void* data;
} expr_t;

typedef struct {
    char* name;
    uint8_t len;
} expr_ident_t;

typedef struct {
    expr_ident_t name;
    expr_t body;
} expr_function_t;

typedef struct {
    expr_t function;
    expr_t value;
} expr_application;

#endif // LAMBENT_PARSER_H