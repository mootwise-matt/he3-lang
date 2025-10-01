#include <stdio.h>
#include <stdlib.h>
#include "../../include/ast.h"

int main() {
    printf("Testing AST node creation...\n");
    
    Ast* node = malloc(sizeof(Ast));
    if (node) {
        printf("Allocated AST node successfully\n");
        node->kind = AST_VAR_DECL;
        printf("Set kind to: %d\n", node->kind);
        free(node);
        printf("Freed AST node successfully\n");
    } else {
        printf("Failed to allocate AST node\n");
        return 1;
    }
    
    printf("AST test completed successfully\n");
    return 0;
}
