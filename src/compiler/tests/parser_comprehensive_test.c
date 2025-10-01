#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../parser/parser.h"
#include "../lexer/lexer.h"

// Test helper functions
void test_assert(bool condition, const char* message) {
    if (!condition) {
        printf("FAIL: %s\n", message);
        exit(1);
    }
}

void test_expression_parsing() {
    printf("Testing expression parsing...\n");
    
    // Test basic arithmetic
    const char* source1 = "1 + 2 * 3";
    Lexer* lexer1 = lexer_create(source1);
    Parser* parser1 = parser_create(lexer1);
    Ast* ast1 = parse_expression(parser1);
    test_assert(ast1 != NULL, "Basic arithmetic expression should parse");
    test_assert(ast1->kind == AST_BINARY, "Should be binary expression");
    parser_destroy(parser1);
    lexer_destroy(lexer1);
    
    // Test method calls
    const char* source2 = "obj.method(arg1, arg2)";
    Lexer* lexer2 = lexer_create(source2);
    Parser* parser2 = parser_create(lexer2);
    Ast* ast2 = parse_expression(parser2);
    test_assert(ast2 != NULL, "Method call should parse");
    test_assert(ast2->kind == AST_CALL, "Should be call expression");
    parser_destroy(parser2);
    lexer_destroy(lexer2);
    
    // Test field access
    const char* source3 = "obj.field";
    Lexer* lexer3 = lexer_create(source3);
    Parser* parser3 = parser_create(lexer3);
    Ast* ast3 = parse_expression(parser3);
    test_assert(ast3 != NULL, "Field access should parse");
    test_assert(ast3->kind == AST_FIELD_ACCESS, "Should be field access");
    parser_destroy(parser3);
    lexer_destroy(lexer3);
    
    printf("✓ Expression parsing test passed\n");
}

void test_statement_parsing() {
    printf("Testing statement parsing...\n");
    
    // Test variable declaration
    const char* source1 = "var x: integer = 42;";
    Lexer* lexer1 = lexer_create(source1);
    Parser* parser1 = parser_create(lexer1);
    printf("Current token: %d\n", parser1->current.kind);
    Ast* ast1 = parse_statement(parser1);
    test_assert(ast1 != NULL, "Variable declaration should parse");
    if (ast1) {
        printf("AST kind: %d, expected: %d\n", ast1->kind, AST_VAR_DECL);
    }
    test_assert(ast1->kind == AST_VAR_DECL, "Should be variable declaration");
    parser_destroy(parser1);
    lexer_destroy(lexer1);
    
    // Test if statement
    const char* source2 = "if (x > 0) { return x; } else { return 0; }";
    Lexer* lexer2 = lexer_create(source2);
    Parser* parser2 = parser_create(lexer2);
    Ast* ast2 = parse_statement(parser2);
    test_assert(ast2 != NULL, "If statement should parse");
    test_assert(ast2->kind == AST_IF, "Should be if statement");
    parser_destroy(parser2);
    lexer_destroy(lexer2);
    
    // Test while loop
    const char* source3 = "while (i < 10) { i = i + 1; }";
    Lexer* lexer3 = lexer_create(source3);
    Parser* parser3 = parser_create(lexer3);
    Ast* ast3 = parse_statement(parser3);
    test_assert(ast3 != NULL, "While loop should parse");
    test_assert(ast3->kind == AST_WHILE, "Should be while statement");
    parser_destroy(parser3);
    lexer_destroy(lexer3);
    
    // Test for loop
    const char* source4 = "for (var i = 0; i < 10; i = i + 1) { print(i); }";
    Lexer* lexer4 = lexer_create(source4);
    Parser* parser4 = parser_create(lexer4);
    Ast* ast4 = parse_statement(parser4);
    test_assert(ast4 != NULL, "For loop should parse");
    test_assert(ast4->kind == AST_FOR, "Should be for statement");
    parser_destroy(parser4);
    lexer_destroy(lexer4);
    
    // Test return statement
    const char* source5 = "return 42;";
    Lexer* lexer5 = lexer_create(source5);
    Parser* parser5 = parser_create(lexer5);
    Ast* ast5 = parse_statement(parser5);
    test_assert(ast5 != NULL, "Return statement should parse");
    test_assert(ast5->kind == AST_RETURN, "Should be return statement");
    parser_destroy(parser5);
    lexer_destroy(lexer5);
    
    printf("✓ Statement parsing test passed\n");
}

void test_type_parsing() {
    printf("Testing type parsing...\n");
    
    // Test primitive types
    const char* source1 = "integer";
    Lexer* lexer1 = lexer_create(source1);
    Parser* parser1 = parser_create(lexer1);
    Ast* ast1 = parse_type(parser1);
    test_assert(ast1 != NULL, "Primitive type should parse");
    if (ast1) {
        printf("Type AST kind: %d, expected: %d\n", ast1->kind, AST_LITERAL);
    }
    test_assert(ast1->kind == AST_LITERAL, "Should be literal type");
    parser_destroy(parser1);
    lexer_destroy(lexer1);
    
    // Test qualified type names
    const char* source2 = "System.String";
    Lexer* lexer2 = lexer_create(source2);
    Parser* parser2 = parser_create(lexer2);
    Ast* ast2 = parse_type(parser2);
    test_assert(ast2 != NULL, "Qualified type should parse");
    test_assert(ast2->kind == AST_IDENTIFIER, "Should be identifier type");
    parser_destroy(parser2);
    lexer_destroy(lexer2);
    
    // Test nullable types
    const char* source3 = "?integer";
    Lexer* lexer3 = lexer_create(source3);
    Parser* parser3 = parser_create(lexer3);
    Ast* ast3 = parse_type(parser3);
    test_assert(ast3 != NULL, "Nullable type should parse");
    test_assert(ast3->kind == AST_NULLABLE_TYPE, "Should be nullable type");
    parser_destroy(parser3);
    lexer_destroy(lexer3);
    
    printf("✓ Type parsing test passed\n");
}

void test_pattern_parsing() {
    printf("Testing pattern parsing...\n");
    
    // Test literal patterns
    const char* source1 = "42";
    Lexer* lexer1 = lexer_create(source1);
    Parser* parser1 = parser_create(lexer1);
    Ast* ast1 = parse_pattern(parser1);
    test_assert(ast1 != NULL, "Literal pattern should parse");
    test_assert(ast1->kind == AST_LITERAL_PATTERN, "Should be literal pattern");
    parser_destroy(parser1);
    lexer_destroy(lexer1);
    
    // Test identifier patterns
    const char* source2 = "x";
    Lexer* lexer2 = lexer_create(source2);
    Parser* parser2 = parser_create(lexer2);
    Ast* ast2 = parse_pattern(parser2);
    test_assert(ast2 != NULL, "Identifier pattern should parse");
    test_assert(ast2->kind == AST_IDENTIFIER_PATTERN, "Should be identifier pattern");
    parser_destroy(parser2);
    lexer_destroy(lexer2);
    
    printf("✓ Pattern parsing test passed\n");
}

void test_compilation_unit() {
    printf("Testing compilation unit parsing...\n");
    
    const char* source = 
        "domain app.hello;\n"
        "\n"
        "class Program {\n"
        "  function main(): integer {\n"
        "    return 42;\n"
        "  }\n"
        "}";
    
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    Ast* ast = parse_compilation_unit(parser);
    
    test_assert(ast != NULL, "Compilation unit should parse");
    test_assert(ast->kind == AST_COMPUNIT, "Should be compilation unit");
    test_assert(ast->child_count > 0, "Should have children");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("✓ Compilation unit parsing test passed\n");
}

void test_error_recovery() {
    printf("Testing error recovery...\n");
    
    // Test with syntax error
    const char* source = "var x = ;"; // Missing expression
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    Ast* ast = parse_statement(parser);
    
    // Parser should handle error gracefully
    test_assert(parser->had_error == true, "Should detect syntax error");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("✓ Error recovery test passed\n");
}

int main() {
    printf("Running He³ Parser Comprehensive Tests\n");
    printf("=====================================\n\n");
    
    test_expression_parsing();
    test_statement_parsing();
    test_type_parsing();
    test_pattern_parsing();
    test_compilation_unit();
    test_error_recovery();
    
    printf("\n🎉 All parser comprehensive tests passed!\n");
    return 0;
}
