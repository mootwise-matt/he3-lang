#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parse_expression function with semicolon...\n");
    
    const char* source = "x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    printf("About to call parse_expression\n");
    Ast* expr = parse_expression(parser);
    printf("parse_expression completed\n");
    
    if (expr) {
        printf("Parsed expression with kind: %d\n", expr->kind);
        free(expr);
    } else {
        printf("Failed to parse expression\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parse expression debug test completed\n");
    return 0;
}
