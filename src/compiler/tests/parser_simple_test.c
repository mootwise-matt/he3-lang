#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing simple parser functionality...\n");
    
    // Test basic AST node creation
    Ast* node = create_ast_node(AST_VAR_DECL);
    if (node) {
        printf("Created AST node with kind: %d\n", node->kind);
        free(node);
    } else {
        printf("Failed to create AST node\n");
        return 1;
    }
    
    // Test simple variable declaration
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    Ast* ast = parse_statement(parser);
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
    } else {
        printf("Failed to parse statement\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Simple test completed\n");
    return 0;
}
