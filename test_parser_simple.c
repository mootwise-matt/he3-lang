#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/compiler/lexer/tokens.h"
#include "src/compiler/lexer/lexer.h"
#include "src/compiler/parser/parser.h"
#include "src/compiler/ast/ast.h"

int main() {
    const char* source = "return 0;";
    
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
    
    printf("Testing parser with simple source: %s\n", source);
    
    // Parse a simple return statement
    Ast* ast = parse_return_statement(parser);
    if (!ast) {
        printf("Failed to parse return statement\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        return 1;
    }
    
    printf("AST created successfully:\n");
    printf("AST kind: %d\n", ast->kind);
    printf("Child count: %d\n", ast->child_count);
    
    if (ast->child_count > 0 && ast->children[0]) {
        Ast* child = ast->children[0];
        printf("Child kind: %d\n", child->kind);
        if (child->kind == AST_LITERAL) {
            printf("Literal token kind: %d\n", child->literal.token.kind);
            printf("Literal value: %lld\n", child->literal.int_value);
        }
    }
    
    // Clean up
    ast_destroy_node(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    return 0;
}
