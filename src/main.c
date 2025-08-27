
#include "stdio.h"
#include "lambent/lexer.h"
#include "lambent/parser.h"

int main() {
    char *source =
        "let true = x => y => x;\n"
        "let result = div 5 2; // Comment\n"
        "#print result;\n";

    lexer_t lex = lexer_create(source);
    parser_t parser = parser_create(&lex);
    
    program_t *program = parser_parse_program(&parser);
    parser_print_program(program);

    return 0;
}
