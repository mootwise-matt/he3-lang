#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer/lexer.h"

int main() {
    printf("Testing lexer with simple input...\n");
    
    const char* source = "class Program {";
    printf("Source: '%s'\n", source);
    
    Lexer* lexer = lexer_create(source);
    
    printf("Lexer created, processing tokens:\n");
    
    int token_count = 0;
    while (true) {
        Token token = lexer_next_token(lexer);
        printf("Token %d: kind=%d, text='%.*s'\n", 
               token_count, token.kind, (int)token.len, token.start);
        
        if (token.kind == TK_EOF) {
            break;
        }
        
        token_count++;
        if (token_count > 10) {
            printf("Too many tokens, breaking...\n");
            break;
        }
    }
    
    lexer_destroy(lexer);
    return 0;
}

