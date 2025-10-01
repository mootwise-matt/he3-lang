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

// Regression test: Simple class structure
void test_regression_simple_class() {
    printf("Regression: Simple class structure...\n");
    
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
    printf("✓ Simple class structure test passed\n");
}

// Regression test: Domain declaration
void test_regression_domain_declaration() {
    printf("Regression: Domain declaration...\n");
    
    const char* source = "domain app.hello;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 1, "Should have one child");
    
    Ast* domain_decl = ast->children[0];
    test_assert(domain_decl->kind == AST_DOMAIN, "Child should be domain declaration");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Domain declaration test passed\n");
}

// Regression test: Import declaration
void test_regression_import_declaration() {
    printf("Regression: Import declaration...\n");
    
    const char* source = "import app.utils;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 1, "Should have one child");
    
    Ast* import_decl = ast->children[0];
    test_assert(import_decl->kind == AST_IMPORT, "Child should be import declaration");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Import declaration test passed\n");
}

// Regression test: Class with inheritance
void test_regression_class_inheritance() {
    printf("Regression: Class inheritance...\n");
    
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
    printf("✓ Class inheritance test passed\n");
}

// Regression test: Class with type parameters
void test_regression_class_type_parameters() {
    printf("Regression: Class type parameters...\n");
    
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
    printf("✓ Class type parameters test passed\n");
}

// Regression test: Multiple classes
void test_regression_multiple_classes() {
    printf("Regression: Multiple classes...\n");
    
    const char* source = "class Program { } class Utils { }";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 2, "Should have two children");
    
    Ast* class1 = ast->children[0];
    test_assert(class1->kind == AST_CLASS, "First child should be class declaration");
    test_assert(strcmp(class1->identifier, "Program") == 0, "First class name should be 'Program'");
    
    Ast* class2 = ast->children[1];
    test_assert(class2->kind == AST_CLASS, "Second child should be class declaration");
    test_assert(strcmp(class2->identifier, "Utils") == 0, "Second class name should be 'Utils'");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Multiple classes test passed\n");
}

// Regression test: Complex qualified names
void test_regression_complex_qualified_names() {
    printf("Regression: Complex qualified names...\n");
    
    const char* source = "domain app.core.utils; import app.core.utils.StringUtils;";
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 2, "Should have two children");
    
    Ast* domain_decl = ast->children[0];
    test_assert(domain_decl->kind == AST_DOMAIN, "First child should be domain declaration");
    
    Ast* import_decl = ast->children[1];
    test_assert(import_decl->kind == AST_IMPORT, "Second child should be import declaration");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Complex qualified names test passed\n");
}

// Regression test: Empty compilation unit
void test_regression_empty_compilation_unit() {
    printf("Regression: Empty compilation unit...\n");
    
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

// Regression test: Error recovery
void test_regression_error_recovery() {
    printf("Regression: Error recovery...\n");
    
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

// Regression test: Full He³ program structure
void test_regression_full_program_structure() {
    printf("Regression: Full He³ program structure...\n");
    
    const char* source = 
        "domain app.hello;\n"
        "import app.utils;\n"
        "\n"
        "class Program {\n"
        "  function main(): integer {\n"
        "    return 42;\n"
        "  }\n"
        "}\n"
        "\n"
        "class Utils {\n"
        "  function helper(): string {\n"
        "    return \"hello\";\n"
        "  }\n"
        "}";
    
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    
    Ast* ast = parse_compilation_unit(parser);
    test_assert(ast != NULL, "AST should not be NULL");
    test_assert(ast->kind == AST_COMPUNIT, "Root should be compilation unit");
    test_assert(ast->child_count == 4, "Should have four children");
    
    // Check domain declaration
    Ast* domain_decl = ast->children[0];
    test_assert(domain_decl->kind == AST_DOMAIN, "First child should be domain declaration");
    
    // Check import declaration
    Ast* import_decl = ast->children[1];
    test_assert(import_decl->kind == AST_IMPORT, "Second child should be import declaration");
    
    // Check first class
    Ast* class1 = ast->children[2];
    test_assert(class1->kind == AST_CLASS, "Third child should be class declaration");
    test_assert(strcmp(class1->identifier, "Program") == 0, "First class name should be 'Program'");
    
    // Check second class
    Ast* class2 = ast->children[3];
    test_assert(class2->kind == AST_CLASS, "Fourth child should be class declaration");
    test_assert(strcmp(class2->identifier, "Utils") == 0, "Second class name should be 'Utils'");
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("✓ Full He³ program structure test passed\n");
}

// Regression test: Class with implements
void test_regression_class_implements() {
    printf("Regression: Class implements...\n");
    
    const char* source = "class Program implements Runnable { }";
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
    printf("✓ Class implements test passed\n");
}

// Regression test: Class with both extends and implements
void test_regression_class_extends_implements() {
    printf("Regression: Class extends and implements...\n");
    
    const char* source = "class Program extends BaseClass implements Runnable { }";
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
    printf("✓ Class extends and implements test passed\n");
}

int main() {
    printf("Running He³ Parser Regression Tests\n");
    printf("===================================\n\n");
    
    test_regression_simple_class();
    test_regression_domain_declaration();
    test_regression_import_declaration();
    test_regression_class_inheritance();
    test_regression_class_type_parameters();
    test_regression_multiple_classes();
    test_regression_complex_qualified_names();
    test_regression_empty_compilation_unit();
    test_regression_error_recovery();
    test_regression_full_program_structure();
    test_regression_class_implements();
    test_regression_class_extends_implements();
    
    printf("\n🎉 All parser regression tests passed!\n");
    return 0;
}
