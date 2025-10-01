#include <stdio.h>
#include <stdlib.h>
#include "../../include/ast.h"
#include "../parser/parser.h"

int main() {
    printf("Testing AST enum values...\n");
    printf("AST_COMPUNIT: %d\n", AST_COMPUNIT);
    printf("AST_BLOCK: %d\n", AST_BLOCK);
    printf("AST_VAR_DECL: %d\n", AST_VAR_DECL);
    printf("AST_ASSIGN: %d\n", AST_ASSIGN);
    printf("AST_IF: %d\n", AST_IF);
    
    Ast* node = create_ast_node(AST_VAR_DECL);
    if (node) {
        printf("Created AST node with kind: %d\n", node->kind);
        free(node);
    } else {
        printf("Failed to create AST node\n");
    }
    
    printf("AST enum test completed\n");
    return 0;
}
