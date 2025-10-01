#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parser_consume function...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    // Test parser_advance
    printf("About to call parser_advance\n");
    parser_advance(parser);
    printf("parser_advance completed, current token: %d\n", parser->current.kind);
    
    // Test parser_consume
    printf("About to call parser_consume\n");
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected variable name");
    printf("parser_consume completed, token kind: %d\n", name.kind);
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parser consume test completed\n");
    return 0;
}
