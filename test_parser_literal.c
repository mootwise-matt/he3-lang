#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/compiler/lexer/tokens.h"
#include "src/compiler/lexer/lexer.h"
#include "src/compiler/parser/parser.h"
#include "src/compiler/ast/ast.h"

int main() {
    const char* source = "0";
    
    // Create lexer
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        printf("Failed to create lexer\n");
        return 1;
    }
    
    // Create parser
    Parser* parser = parser_create(lexer);
    if (!parser) {
        printf("Failed to create parser\n");
        lexer_destroy(lexer);
        return 1;
    }
    
    printf("Testing parser with literal: %s\n", source);
    
    // Parse a simple literal
    Ast* ast = parse_primary_expression(parser);
    if (!ast) {
        printf("Failed to parse literal\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        return 1;
    }
    
    printf("AST created successfully:\n");
    printf("AST kind: %d\n", ast->kind);
    printf("Literal token kind: %d\n", ast->token.kind);
    printf("Literal value: %lld\n", ast->literal_value.int_value);
    
    // Clean up
    ast_destroy_node(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    return 0;
}
