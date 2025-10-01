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

// Test parsing simple class
void test_parse_simple_class() {
    printf("Testing simple class parsing...\n");
    
    const char* source = "class Program { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 1, "Should have one child");
    
    Ast* class_decl = ast->children[0];
    test_assert(class_decl->kind == AST_CLASS, "Child should be class declaration");
    test_assert(strcmp(class_decl->identifier, "Program") == 0, "Class name should be 'Program'");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Simple class parsing test passed\n");
}

// Test parsing class with method
void test_parse_class_with_method() {
    printf("Testing class with method parsing...\n");
    
    const char* source = "class Program { function main(): integer { return 42; } }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 1, "Should have one child");
    
    Ast* class_decl = ast->children[0];
    test_assert(class_decl->kind == AST_CLASS, "Child should be class declaration");
    test_assert(strcmp(class_decl->identifier, "Program") == 0, "Class name should be 'Program'");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Class with method parsing test passed\n");
}

// Test parsing domain declaration
void test_parse_domain_declaration() {
    printf("Testing domain declaration parsing...\n");
    
    const char* source = "domain app.hello; class Program { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 2, "Should have two children");
    
    Ast* domain_decl = ast->children[0];
    test_assert(domain_decl->kind == AST_DOMAIN, "First child should be domain declaration");
    
    Ast* class_decl = ast->children[1];
    test_assert(class_decl->kind == AST_CLASS, "Second child should be class declaration");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Domain declaration parsing test passed\n");
}

// Test parsing import declaration
void test_parse_import_declaration() {
    printf("Testing import declaration parsing...\n");
    
    const char* source = "import app.utils; class Program { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 2, "Should have two children");
    
    Ast* import_decl = ast->children[0];
    test_assert(import_decl->kind == AST_IMPORT, "First child should be import declaration");
    
    Ast* class_decl = ast->children[1];
    test_assert(class_decl->kind == AST_CLASS, "Second child should be class declaration");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Import declaration parsing test passed\n");
}

// Test parsing class with inheritance
void test_parse_class_inheritance() {
    printf("Testing class inheritance parsing...\n");
    
    const char* source = "class Program extends BaseClass { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 1, "Should have one child");
    
    Ast* class_decl = ast->children[0];
    test_assert(class_decl->kind == AST_CLASS, "Child should be class declaration");
    test_assert(strcmp(class_decl->identifier, "Program") == 0, "Class name should be 'Program'");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Class inheritance parsing test passed\n");
}

// Test parsing class with type parameters
void test_parse_class_type_parameters() {
    printf("Testing class type parameters parsing...\n");
    
    const char* source = "class Container[T] { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 1, "Should have one child");
    
    Ast* class_decl = ast->children[0];
    test_assert(class_decl->kind == AST_CLASS, "Child should be class declaration");
    test_assert(strcmp(class_decl->identifier, "Container") == 0, "Class name should be 'Container'");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Class type parameters parsing test passed\n");
}

// Test parsing error recovery
void test_parse_error_recovery() {
    printf("Testing error recovery...\n");
    
    const char* source = "class Program { function main() { return 42; } }"; // Missing type
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL even with errors");
    test_assert(parser->had_error == true, "Parser should have detected error");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Error recovery test passed\n");
}

// Test parsing empty compilation unit
void test_parse_empty_compilation_unit() {
    printf("Testing empty compilation unit...\n");
    
    const char* source = "";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 0, "Should have no children");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Empty compilation unit test passed\n");
}

// Test parsing qualified names
void test_parse_qualified_names() {
    printf("Testing qualified name parsing...\n");
    
    const char* source = "domain app.hello.world; class Program { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 2, "Should have two children");
    
    Ast* domain_decl = ast->children[0];
    test_assert(domain_decl->kind == AST_DOMAIN, "First child should be domain declaration");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Qualified name parsing test passed\n");
}

int main() {
    printf("Running He³ Parser Tests\n");
    printf("========================\n\n");
    
    test_parse_simple_class();
    test_parse_class_with_method();
    test_parse_domain_declaration();
    test_parse_import_declaration();
    test_parse_class_inheritance();
    test_parse_class_type_parameters();
    test_parse_error_recovery();
    test_parse_empty_compilation_unit();
    test_parse_qualified_names();
    
    printf("\n🎉 All parser tests passed!\n");
    return 0;
}
