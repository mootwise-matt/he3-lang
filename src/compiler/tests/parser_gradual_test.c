#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

// Gradually add complexity to find where it breaks
Ast* gradual_parse_variable_declaration(Parser* parser) {
    printf("Gradual parse_variable_declaration called\n");
    
    // Step 1: Just create the AST node
    Ast* decl = create_ast_node(AST_VAR_DECL);
    if (!decl) {
        printf("Failed to create AST node\n");
        return NULL;
    }
    printf("Step 1: Created AST node with kind: %d\n", decl->kind);
    
    // Step 2: Add basic fields
    decl->is_mutable = true;
    decl->child_count = 2;
    printf("Step 2: Set basic fields\n");
    
    // Step 3: Add identifier (simplified)
    decl->identifier = malloc(2);
    if (decl->identifier) {
        strcpy(decl->identifier, "x");
        printf("Step 3: Set identifier\n");
    }
    
    // Step 4: Set children to NULL for now
    decl->children[0] = NULL;
    decl->children[1] = NULL;
    printf("Step 4: Set children to NULL\n");
    
    printf("Returning AST node with kind: %d\n", decl->kind);
    return decl;
}

int main() {
    printf("Testing gradual parser...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    printf("About to call gradual_parse_variable_declaration\n");
    Ast* ast = gradual_parse_variable_declaration(parser);
    printf("gradual_parse_variable_declaration completed\n");
    
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse variable declaration\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Gradual parser test completed\n");
    return 0;
}
