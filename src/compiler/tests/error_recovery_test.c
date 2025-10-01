#include <stdio.h>
#include <stdlib.h>
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../ast/ast.h"

int main() {
    printf("=== He³ Parser Error Recovery Test ===\n\n");
    
    // Test cases with various errors
    const char* test_cases[] = {
        // Missing semicolon
        "domain app.main;\nclass Program {\n  function main(): integer {\n    var x: integer = 5\n    return x;\n  }\n}",
        
        // Missing closing brace
        "domain app.main;\nclass Program {\n  function main(): integer {\n    var x: integer = 5;\n    return x;\n",
        
        // Invalid token
        "domain app.main;\nclass Program {\n  function main(): integer {\n    var x: integer = 5;\n    @invalid_token\n    return x;\n  }\n}",
        
        // Missing parameter type
        "domain app.main;\nclass Program {\n  function main(x): integer {\n    return x;\n  }\n}",
        
        // Unexpected token in class body
        "domain app.main;\nclass Program {\n  invalid_statement\n  function main(): integer {\n    return 0;\n  }\n}"
    };
    
    const char* test_names[] = {
        "Missing semicolon",
        "Missing closing brace", 
        "Invalid token",
        "Missing parameter type",
        "Unexpected token in class body"
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("=== Test %d: %s ===\n", i + 1, test_names[i]);
        printf("Source:\n%s\n", test_cases[i]);
        
        // Create lexer
        Lexer* lexer = lexer_create(test_cases[i]);
        if (!lexer) {
            printf("Failed to create lexer\n");
            continue;
        }
        
        // Create parser
        Parser* parser = parser_create(lexer);
        if (!parser) {
            printf("Failed to create parser\n");
            lexer_destroy(lexer);
            continue;
        }
        
        // Parse source
        printf("\nParsing with error recovery...\n");
        Ast* ast = parse_compilation_unit(parser);
        
        if (ast) {
            printf("Parse completed (with errors)\n");
            printf("Parser had errors: %s\n", parser->had_error ? "Yes" : "No");
            
            // Print AST
            printf("\nGenerated AST:\n");
            ast_print_tree(ast, 0);
            
            ast_destroy_tree(ast);
        } else {
            printf("Parse failed completely\n");
        }
        
        parser_destroy(parser);
        lexer_destroy(lexer);
        printf("\n");
    }
    
    printf("=== Error Recovery Test Complete ===\n");
    return 0;
}
