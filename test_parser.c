#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/compiler/lexer/tokens.h"
#include "src/compiler/lexer/lexer.h"
#include "src/compiler/parser/parser.h"
#include "src/compiler/ast/ast.h"

void print_ast_node(Ast* node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    switch (node->kind) {
        case AST_LITERAL:
            printf("AST_LITERAL: token_kind=%d, token.kind=%d", 
                   node->literal.token.kind, node->literal.token.kind);
            if (node->literal.token.kind == TK_STRING) {
                printf(", string='%s'", node->literal.string_value);
            } else if (node->literal.token.kind == TK_INT) {
                printf(", int=%lld", node->literal.int_value);
            }
            break;
        case AST_IDENTIFIER:
            printf("AST_IDENTIFIER: '%s'", node->identifier);
            break;
        case AST_RETURN:
            printf("AST_RETURN");
            break;
        case AST_EXPR_STMT:
            printf("AST_EXPR_STMT");
            break;
        case AST_CALL:
            printf("AST_CALL");
            break;
        case AST_METHOD:
            printf("AST_METHOD: '%s'", node->identifier);
            break;
        case AST_CLASS:
            printf("AST_CLASS: '%s'", node->identifier);
            break;
        case AST_DOMAIN:
            printf("AST_DOMAIN");
            break;
        default:
            printf("AST_%d", node->kind);
            break;
    }
    printf(" (line=%d, col=%d)\n", node->line, node->col);
    
    for (uint32_t i = 0; i < node->child_count; i++) {
        print_ast_node(node->children[i], depth + 1);
    }
}

int main() {
    const char* source = "domain app.hello;\n\nclass Program {\n  function main(): integer {\n    print(\"Hello, He³ World!\");\n    return 0;\n  }\n}";
    
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
    
    printf("Testing parser output:\n");
    printf("Source: %s\n\n", source);
    
    // Parse the source
    Ast* ast = parse_compilation_unit(parser);
    if (!ast) {
        printf("Failed to parse source\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        return 1;
    }
    
    printf("AST created successfully:\n");
    print_ast_node(ast, 0);
    
    // Clean up
    ast_destroy_node(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    return 0;
}
