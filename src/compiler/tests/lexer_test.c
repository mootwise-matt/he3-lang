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

void test_lexer_basic_tokens() {
    printf("Testing basic tokens...\n");
    
    const char* source = "class Program { function main(): integer { return 42; } }";
    Lexer* lexer = lexer_create(source);
    
    // Expected tokens
    TokenKind expected[] = {
        TK_CLASS, TK_IDENTIFIER, TK_LBRACE, TK_FUNCTION, TK_IDENTIFIER,
        TK_LPAREN, TK_RPAREN, TK_COLON, TK_IDENTIFIER, TK_LBRACE,
        TK_RETURN, TK_INT, TK_SEMI, TK_RBRACE, TK_RBRACE, TK_EOF
    };
    
    for (int i = 0; i < 16; i++) {
        Token token = lexer_next_token(lexer);
        if (token.kind != expected[i]) {
            printf("Token %d: Expected %d, Got %d\n", i, expected[i], token.kind);
        }
        test_assert(token.kind == expected[i], 
                   "Token kind mismatch");
        
        if (token.kind == TK_INT) {
            test_assert(token.literal.int_value == 42, 
                       "Integer literal value incorrect");
        }
    }
    
    lexer_destroy(lexer);
    printf("✓ Basic tokens test passed\n");
}

void test_lexer_strings() {
    printf("Testing string literals...\n");
    
    const char* source = "\"hello world\" \"test\"";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_STRING, "Expected string token");
    test_assert(strncmp(token1.literal.string_value, "hello world", 11) == 0, 
               "String content incorrect");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_STRING, "Expected string token");
    test_assert(strncmp(token2.literal.string_value, "test", 4) == 0, 
               "String content incorrect");
    
    lexer_destroy(lexer);
    printf("✓ String literals test passed\n");
}

void test_lexer_numbers() {
    printf("Testing number literals...\n");
    
    const char* source = "42 3.14 0 1.0";
    Lexer* lexer = lexer_create(source);
    
    Token int_token = lexer_next_token(lexer);
    test_assert(int_token.kind == TK_INT, "Expected integer token");
    test_assert(int_token.literal.int_value == 42, "Integer value incorrect");
    
    Token float_token = lexer_next_token(lexer);
    test_assert(float_token.kind == TK_FLOAT, "Expected float token");
    test_assert(float_token.literal.float_value == 3.14, "Float value incorrect");
    
    Token zero_token = lexer_next_token(lexer);
    test_assert(zero_token.kind == TK_INT, "Expected integer token");
    test_assert(zero_token.literal.int_value == 0, "Zero value incorrect");
    
    Token one_float_token = lexer_next_token(lexer);
    test_assert(one_float_token.kind == TK_FLOAT, "Expected float token");
    test_assert(one_float_token.literal.float_value == 1.0, "Float value incorrect");
    
    lexer_destroy(lexer);
    printf("✓ Number literals test passed\n");
}

void test_lexer_keywords() {
    printf("Testing keywords...\n");
    
    const char* source = "class function return var let if while for match";
    Lexer* lexer = lexer_create(source);
    
    TokenKind expected[] = {
        TK_CLASS, TK_FUNCTION, TK_RETURN, TK_VAR, TK_LET, 
        TK_IF, TK_WHILE, TK_FOR, TK_MATCH, TK_EOF
    };
    
    for (int i = 0; i < 10; i++) {
        Token token = lexer_next_token(lexer);
        test_assert(token.kind == expected[i], "Keyword token incorrect");
    }
    
    lexer_destroy(lexer);
    printf("✓ Keywords test passed\n");
}

void test_lexer_operators() {
    printf("Testing operators...\n");
    
    const char* source = "+ - * / % == != < <= > >= && || ! = := ->";
    Lexer* lexer = lexer_create(source);
    
    TokenKind expected[] = {
        TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE, TK_MODULO,
        TK_EQUAL, TK_NOT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_GREATER, TK_GREATER_EQUAL,
        TK_AND, TK_OR, TK_NOT, TK_ASSIGN, TK_ASSIGN, TK_ARROW, TK_EOF
    };
    
    for (int i = 0; i < 17; i++) {
        Token token = lexer_next_token(lexer);
        test_assert(token.kind == expected[i], "Operator token incorrect");
    }
    
    lexer_destroy(lexer);
    printf("✓ Operators test passed\n");
}

void test_lexer_comments() {
    printf("Testing comments...\n");
    
    const char* source = "// This is a comment\nclass Program { /* block comment */ }";
    Lexer* lexer = lexer_create(source);
    
    Token token1 = lexer_next_token(lexer);
    test_assert(token1.kind == TK_CLASS, "Expected class token after comment");
    
    Token token2 = lexer_next_token(lexer);
    test_assert(token2.kind == TK_IDENTIFIER, "Expected identifier token");
    
    lexer_destroy(lexer);
    printf("✓ Comments test passed\n");
}

int main() {
    printf("Running He³ Lexer Tests\n");
    printf("======================\n\n");
    
    test_lexer_basic_tokens();
    test_lexer_strings();
    test_lexer_numbers();
    test_lexer_keywords();
    test_lexer_operators();
    test_lexer_comments();
    
    printf("\n🎉 All lexer tests passed!\n");
    return 0;
}
