#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

// Simplified version of parse_variable_declaration for testing
Ast* test_parse_variable_declaration(Parser* parser) {
    printf("DEBUG: test_parse_variable_declaration called\n");
    TokenKind keyword = parser->current.kind; // var or let
    parser_advance(parser); // consume var or let
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected variable name");
    
    // Skip type and initializer for now
    Ast* type = NULL;
    Ast* initializer = NULL;
    
    parser_consume(parser, TK_SEMI, "Expected ';' after variable declaration");
    
    printf("DEBUG: About to create AST node with AST_VAR_DECL = %d\n", AST_VAR_DECL);
    Ast* decl = create_ast_node(AST_VAR_DECL);
    printf("DEBUG: Created AST node, kind = %d\n", decl ? decl->kind : -1);
    if (!decl) return NULL;
    
    decl->identifier = malloc(name.len + 1);
    if (decl->identifier) {
        memcpy(decl->identifier, name.start, name.len);
        decl->identifier[name.len] = '\0';
    }
    
    decl->is_mutable = (keyword == TK_VAR);
    decl->children[0] = type;
    decl->children[1] = initializer;
    decl->child_count = 2;
    
    printf("DEBUG: Returning AST node with kind = %d\n", decl->kind);
    return decl;
}

int main() {
    printf("Testing simplified variable declaration parsing...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    printf("About to call test_parse_variable_declaration\n");
    Ast* ast = test_parse_variable_declaration(parser);
    printf("test_parse_variable_declaration completed\n");
    
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse variable declaration\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Simplified variable declaration test completed\n");
    return 0;
}
