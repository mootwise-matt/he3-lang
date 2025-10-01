#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../lexer/lexer.h"

// Test helper functions
void test_assert(bool condition, const char* message) {
    if (!condition) {
        printf("FAIL: %s\n", message);
        exit(1);
    }
}

// Regression test: Whitespace handling after identifiers
void test_regression_whitespace_after_identifier() {
    printf("Regression: Whitespace after identifier...\n");
    
    const char* source = "Program {";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_IDENTIFIER, "Expected identifier");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_LBRACE, "Expected left brace after whitespace");
    
    lexer_destroy(lexer);
    printf("✓ Whitespace after identifier test passed\n");
}

// Regression test: EOF handling
void test_regression_eof_handling() {
    printf("Regression: EOF handling...\n");
    
    const char* source = "class";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_CLASS, "Expected class keyword");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_EOF, "Expected EOF");
    
    // Multiple EOF calls should return EOF
    Token token3 = lexer_next_token(lexer);
    test_assert(token3.kind == TK_EOF, "Expected EOF on subsequent calls");
    
    lexer_destroy(lexer);
    printf("✓ EOF handling test passed\n");
}

// Regression test: NULL character handling
void test_regression_null_handling() {
    printf("Regression: NULL character handling...\n");
    
    const char* source = "class\0extra";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_CLASS, "Expected class keyword");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_EOF, "Expected EOF after null character");
    
    lexer_destroy(lexer);
    printf("✓ NULL character handling test passed\n");
}

// Regression test: Function keyword recognition
void test_regression_function_keyword() {
    printf("Regression: Function keyword recognition...\n");
    
    const char* source = "function for";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_FUNCTION, "Expected function keyword");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_FOR, "Expected for keyword");
    
    lexer_destroy(lexer);
    printf("✓ Function keyword recognition test passed\n");
}

// Regression test: Assignment operator variations
void test_regression_assignment_operators() {
    printf("Regression: Assignment operator variations...\n");
    
    const char* source = "= := ==";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_ASSIGN, "Expected assignment operator");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_ASSIGN, "Expected assignment operator for :=");
    
    Token token3 = lexer_next_token(lexer);
    test_assert(token3.kind == TK_EQUAL, "Expected equality operator");
    
    lexer_destroy(lexer);
    printf("✓ Assignment operator variations test passed\n");
}

// Regression test: Complex whitespace
void test_regression_complex_whitespace() {
    printf("Regression: Complex whitespace...\n");
    
    const char* source = "  class  \n  Program  \t\t{  ";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_CLASS, "Expected class keyword");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_IDENTIFIER, "Expected identifier");
    
    Token token3 = lexer_next_token(lexer);
    test_assert(token3.kind == TK_LBRACE, "Expected left brace");
    
    lexer_destroy(lexer);
    printf("✓ Complex whitespace test passed\n");
}

// Regression test: Line and block comments
void test_regression_comment_handling() {
    printf("Regression: Comment handling...\n");
    
    const char* source = "class // line comment\n/* block\ncomment */ Program";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_CLASS, "Expected class keyword");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_IDENTIFIER, "Expected identifier after comments");
    
    lexer_destroy(lexer);
    printf("✓ Comment handling test passed\n");
}

// Regression test: String with spaces
void test_regression_string_with_spaces() {
    printf("Regression: String with spaces...\n");
    
    const char* source = "\"Hello World\"";
    Lexer* lexer = lexer_create(source);
    
    Token token = lexer_next_token(lexer);
    test_assert(token.kind == TK_STRING, "Expected string token");
    test_assert(strncmp(token.literal.string_value, "Hello World", 11) == 0,
               "String content incorrect");
    
    lexer_destroy(lexer);
    printf("✓ String with spaces test passed\n");
}

// Regression test: Number followed by identifier
void test_regression_number_identifier_separation() {
    printf("Regression: Number/identifier separation...\n");
    
    const char* source = "42 abc 3.14 def";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_INT, "Expected integer");
    test_assert(token1.literal.int_value == 42, "Integer value incorrect");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_IDENTIFIER, "Expected identifier");
    
    Token token3 = lexer_next_token(lexer);
    test_assert(token3.kind == TK_FLOAT, "Expected float");
    test_assert(token3.literal.float_value == 3.14, "Float value incorrect");
    
    Token token4 = lexer_next_token(lexer);
    test_assert(token4.kind == TK_IDENTIFIER, "Expected identifier");
    
    lexer_destroy(lexer);
    printf("✓ Number/identifier separation test passed\n");
}

// Regression test: Operator sequences
void test_regression_operator_sequences() {
    printf("Regression: Operator sequences...\n");
    
    const char* source = "<=>=!=";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_LESS_EQUAL, "Expected less-equal operator");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_GREATER_EQUAL, "Expected greater-equal operator");
    
    Token token3 = lexer_next_token(lexer);
    test_assert(token3.kind == TK_NOT_EQUAL, "Expected not-equal operator");
    
    lexer_destroy(lexer);
    printf("✓ Operator sequences test passed\n");
}

// Regression test: Full He³ program
void test_regression_full_program() {
    printf("Regression: Full He³ program...\n");
    
    const char* source = 
        "domain app.hello;\n"
        "\n"
        "class Program {\n"
        "  function main(): integer {\n"
        "    return 42;\n"
        "  }\n"
        "}";
    
    Lexer* lexer = lexer_create(source);
    
    TokenKind expected[] = {
        TK_DOMAIN, TK_IDENTIFIER, TK_DOT, TK_IDENTIFIER, TK_SEMI,
        TK_CLASS, TK_IDENTIFIER, TK_LBRACE,
        TK_FUNCTION, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_IDENTIFIER, TK_LBRACE,
        TK_RETURN, TK_INT, TK_SEMI,
        TK_RBRACE,
        TK_RBRACE,
        TK_EOF
    };
    
    for (int i = 0; i < 21; i++) {
        Token token = lexer_next_token(lexer);
        if (token.kind != expected[i]) {
            printf("Token %d: Expected %d, Got %d\n", i, expected[i], token.kind);
            test_assert(false, "Token mismatch in full program");
        }
    }
    
    lexer_destroy(lexer);
    printf("✓ Full He³ program test passed\n");
}

int main() {
    printf("Running He³ Lexer Regression Tests\n");
    printf("==================================\n\n");
    
    test_regression_whitespace_after_identifier();
    test_regression_eof_handling();
    test_regression_null_handling();
    test_regression_function_keyword();
    test_regression_assignment_operators();
    test_regression_complex_whitespace();
    test_regression_comment_handling();
    test_regression_string_with_spaces();
    test_regression_number_identifier_separation();
    test_regression_operator_sequences();
    test_regression_full_program();
    
    printf("\n🎉 All regression tests passed!\n");
    return 0;
}
