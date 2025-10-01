#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing simple variable declaration parsing...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    
    // Test step by step
    printf("Step 1: parser_advance\n");
    parser_advance(parser);
    printf("Current token after advance: %d\n", parser->current.kind);
    
    printf("Step 2: parser_consume identifier\n");
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected variable name");
    printf("Consumed identifier, current token: %d\n", parser->current.kind);
    
    printf("Step 3: parser_match colon\n");
    bool has_colon = parser_match(parser, TK_COLON);
    printf("Has colon: %d, current token: %d\n", has_colon, parser->current.kind);
    
    printf("Step 4: parser_consume semicolon\n");
    parser_consume(parser, TK_SEMI, "Expected ';' after variable declaration");
    printf("Consumed semicolon, current token: %d\n", parser->current.kind);
    
    printf("Step 5: create_ast_node\n");
    Ast* decl = create_ast_node(AST_VAR_DECL);
    printf("Created AST node with kind: %d\n", decl ? decl->kind : -1);
    
    if (decl) {
        free(decl);
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Simple variable declaration test completed\n");
    return 0;
}
