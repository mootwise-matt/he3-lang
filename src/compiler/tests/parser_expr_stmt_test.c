#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parse_expression_statement function...\n");
    
    const char* source = "x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    printf("About to call parse_expression_statement\n");
    Ast* stmt = parse_expression_statement(parser);
    printf("parse_expression_statement completed\n");
    
    if (stmt) {
        printf("Parsed statement with kind: %d\n", stmt->kind);
        free(stmt);
    } else {
        printf("Failed to parse expression statement\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parse expression statement test completed\n");
    return 0;
}
