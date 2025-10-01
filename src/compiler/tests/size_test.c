#include <stdio.h>
#include <stdlib.h>
#include "../../include/ast.h"

int main() {
    printf("Testing AST structure size...\n");
    printf("Size of Ast: %zu bytes\n", sizeof(Ast));
    printf("Size of AstKind: %zu bytes\n", sizeof(AstKind));
    printf("Size of Token: %zu bytes\n", sizeof(Token));
    printf("Size of TokenKind: %zu bytes\n", sizeof(TokenKind));
    
    Ast* node = malloc(sizeof(Ast));
    if (node) {
        printf("Allocated AST node successfully\n");
        node->kind = AST_VAR_DECL;
        printf("Set kind to: %d\n", node->kind);
        printf("Node address: %p\n", (void*)node);
        free(node);
        printf("Freed AST node successfully\n");
    } else {
        printf("Failed to allocate AST node\n");
        return 1;
    }
    
    printf("Size test completed successfully\n");
    return 0;
}
