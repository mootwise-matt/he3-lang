#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

// Minimal working parser that just creates the AST node
Ast* minimal_parse_variable_declaration(Parser* parser) {
    printf("Minimal parse_variable_declaration called\n");
    
    // Just create the AST node without any complex logic
    Ast* decl = create_ast_node(AST_VAR_DECL);
    if (!decl) {
        printf("Failed to create AST node\n");
        return NULL;
    }
    
    printf("Created AST node with kind: %d\n", decl->kind);
    return decl;
}

int main() {
    printf("Testing minimal working parser...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    printf("About to call minimal_parse_variable_declaration\n");
    Ast* ast = minimal_parse_variable_declaration(parser);
    printf("minimal_parse_variable_declaration completed\n");
    
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse variable declaration\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Minimal working parser test completed\n");
    return 0;
}
