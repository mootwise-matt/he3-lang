#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing minimal parser functionality...\n");
    
    // Test lexer first
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        printf("Failed to create lexer\n");
        return 1;
    }
    printf("Lexer created successfully\n");
    
    // Test parser creation
    Parser* parser = parser_create(lexer);
    if (!parser) {
        printf("Failed to create parser\n");
        lexer_destroy(lexer);
        return 1;
    }
    printf("Parser created successfully\n");
    printf("Current token: %d\n", parser->current.kind);
    
    // Test AST node creation
    Ast* node = create_ast_node(AST_VAR_DECL);
    if (node) {
        printf("Created AST node with kind: %d\n", node->kind);
        free(node);
    } else {
        printf("Failed to create AST node\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Minimal test completed successfully\n");
    return 0;
}
