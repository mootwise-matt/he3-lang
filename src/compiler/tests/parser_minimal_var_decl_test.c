#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing minimal variable declaration parsing...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    // Test just the basic parsing without type or initializer
    printf("About to call parse_variable_declaration\n");
    Ast* ast = parse_variable_declaration(parser);
    printf("parse_variable_declaration returned\n");
    
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse variable declaration\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Minimal variable declaration test completed\n");
    return 0;
}
