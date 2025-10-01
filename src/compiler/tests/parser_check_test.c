#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parser_check function...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Current token: %d\n", parser->current.kind);
    printf("TK_VAR: %d\n", TK_VAR);
    printf("parser_check(parser, TK_VAR): %d\n", parser_check(parser, TK_VAR));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parser check test completed\n");
    return 0;
}
