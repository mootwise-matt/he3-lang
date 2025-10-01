#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

int main() {
    printf("Testing malloc in parse_variable_declaration...\n");
    
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
    printf("Consumed identifier, name.len = %d, current token: %d\n", name.len, parser->current.kind);
    
    printf("Step 3: malloc for identifier\n");
    char* identifier = malloc(name.len + 1);
    printf("malloc completed, identifier = %p\n", (void*)identifier);
    
    if (identifier) {
        printf("Step 4: memcpy\n");
        memcpy(identifier, name.start, name.len);
        identifier[name.len] = '\0';
        printf("memcpy completed, identifier = '%s'\n", identifier);
        free(identifier);
        printf("Freed identifier\n");
    } else {
        printf("malloc failed\n");
    }
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Malloc test completed\n");
    return 0;
}
