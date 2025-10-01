#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing parse_statement function step by step...\n");
    
    const char* source = "var x;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    printf("Parser created, current token: %d\n", parser->current.kind);
    printf("TK_VAR: %d\n", TK_VAR);
    printf("TK_LET: %d\n", TK_LET);
    
    printf("About to call parser_check(parser, TK_VAR)\n");
    bool check_var = parser_check(parser, TK_VAR);
    printf("parser_check(parser, TK_VAR) = %d\n", check_var);
    
    if (check_var) {
        printf("About to call parse_variable_declaration\n");
        Ast* ast = parse_variable_declaration(parser);
        printf("parse_variable_declaration completed\n");
        
        if (ast) {
            printf("Parsed AST with kind: %d\n", ast->kind);
            free(ast);
        } else {
            printf("Failed to parse variable declaration\n");
        }
    } else {
        printf("parser_check(parser, TK_VAR) returned false\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parse statement step test completed\n");
    return 0;
}
