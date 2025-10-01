#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parse_variable_declaration function...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Current token: %d\n", parser->current.kind);
    
    // Test parse_variable_declaration directly
    Ast* ast = parse_variable_declaration(parser);
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse variable declaration\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Variable declaration test completed\n");
    return 0;
}
