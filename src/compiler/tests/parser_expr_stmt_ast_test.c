#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing create_ast_node(AST_EXPR_STMT)...\n");
    
    printf("About to call create_ast_node(AST_EXPR_STMT)\n");
    Ast* expr_stmt = create_ast_node(AST_EXPR_STMT);
    printf("create_ast_node completed\n");
    
    if (expr_stmt) {
        printf("Created AST node with kind: %d\n", expr_stmt->kind);
        printf("AST_EXPR_STMT: %d\n", AST_EXPR_STMT);
        free(expr_stmt);
    } else {
        printf("Failed to create AST node\n");
    }
    
    printf("Create AST node test completed\n");
    return 0;
}
