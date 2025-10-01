#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer/lexer.h"

int main() {
    printf("Testing lexer with type keywords...\n");
    
    const char* source = "integer";
    Lexer* lexer = lexer_create(source);
    
    printf("Source: '%s'\n", source);
    
    Token token = lexer_next_token(lexer);
    printf("Token kind: %d\n", token.kind);
    printf("Token text: '%.*s'\n", (int)token.len, token.start);
    
    if (token.kind == TK_IDENTIFIER) {
        printf("'integer' is being tokenized as TK_IDENTIFIER\n");
    } else if (token.kind == TK_INTEGER) {
        printf("'integer' is being tokenized as TK_INTEGER\n");
    } else {
        printf("'integer' is being tokenized as something else\n");
    }
    
    lexer_destroy(lexer);
    
    printf("Lexer type test completed\n");
    return 0;
}
