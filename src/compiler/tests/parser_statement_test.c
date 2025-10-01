#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parse_statement function...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    // Test parse_statement
    Ast* ast = parse_statement(parser);
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse statement\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Statement test completed\n");
    return 0;
}
