#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing create_ast_node function...\n");
    
    printf("About to call create_ast_node(AST_VAR_DECL)\n");
    Ast* node = create_ast_node(AST_VAR_DECL);
    printf("create_ast_node completed\n");
    
    if (node) {
        printf("Created AST node with kind: %d\n", node->kind);
        printf("Node address: %p\n", (void*)node);
        free(node);
        printf("Freed AST node\n");
    } else {
        printf("Failed to create AST node\n");
    }
    
    printf("Create AST node test completed\n");
    return 0;
}
