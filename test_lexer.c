#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/compiler/lexer/tokens.h"
#include "src/compiler/lexer/lexer.h"

int main() {
    const char* source = "domain app.hello;\n\nclass Program {\n  function main(): integer {\n    print(\"Hello, He³ World!\");\n    return 0;\n  }\n}";
    
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        printf("Failed to create lexer\n");
        return 1;
    }
    
    printf("Testing lexer output:\n");
    printf("Source: %s\n\n", source);
    
    Token token;
    int token_count = 0;
    
    while (!lexer_is_at_end(lexer)) {
        token = lexer_next_token(lexer);
        printf("Token %d: kind=%d, start='%.*s', line=%d, col=%d\n", 
               token_count++, token.kind, token.len, token.start, token.line, token.col);
        
        if (token_count > 50) {  // Prevent infinite loop
            printf("Too many tokens, stopping\n");
            break;
        }
    }
    
    printf("\nTotal tokens: %d\n", token_count);
    lexer_destroy(lexer);
    return 0;
}
