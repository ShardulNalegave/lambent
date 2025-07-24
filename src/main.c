
#include "stdio.h"
#include "lambent/lexer.h"
#include "lambent/parser.h"

int main() {
    char *source =
        "let true = x => y => x;\n"
        "let result = div 5 2; // Comment\n"
        "#print result;\n";

    lambent_lexer_t lex = lambent_lexer_create(source);
    lambent_parser_t parser = lambent_parser_create(&lex);
    
    lambent_program_t *program = lambent_parser_parse_program(&parser);
    lambent_parser_print_program(program);

    return 0;
}
