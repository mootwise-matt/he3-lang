#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parse_type function...\n");
    
    const char* source = "integer";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    printf("About to call parse_type\n");
    Ast* type = parse_type(parser);
    printf("parse_type completed\n");
    
    if (type) {
        printf("Parsed type with kind: %d\n", type->kind);
        free(type);
    } else {
        printf("Failed to parse type\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parser type test completed\n");
    return 0;
}
