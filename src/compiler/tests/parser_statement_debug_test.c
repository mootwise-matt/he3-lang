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
    
    printf("About to call parser_check(parser, TK_LET)\n");
    bool check_let = parser_check(parser, TK_LET);
    printf("parser_check(parser, TK_LET) = %d\n", check_let);
    
    printf("About to call parse_statement\n");
    Ast* ast = parse_statement(parser);
    printf("parse_statement completed\n");
    
    if (ast) {
        printf("Parsed AST with kind: %d\n", ast->kind);
        free(ast);
    } else {
        printf("Failed to parse statement\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Parse statement debug test completed\n");
    return 0;
}
