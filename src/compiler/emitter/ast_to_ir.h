#pragma once

#include "../parser/parser.h"
#include "../ir/ir.h"

// Forward declarations
struct AstToIRTranslator;
struct SymbolTable;
struct TypeTable;

// AST to IR translator
typedef struct AstToIRTranslator {
    IRBuilder* ir_builder;          // IR builder
    struct SymbolTable* symbol_table;      // Symbol table for name resolution
    struct TypeTable* type_table;          // Type table for type information
    
    // Current context
    struct IRFunction* current_function;  // Current function being translated
    struct IRBlock* current_block;        // Current block being translated
    uint32_t current_scope_depth;         // Current scope depth
    
    // Error handling
    char* error_message;
    bool has_error;
} AstToIRTranslator;

// Translator functions
AstToIRTranslator* ast_to_ir_translator_create(void);
void ast_to_ir_translator_destroy(AstToIRTranslator* translator);

// Main translation functions
IRFunction* ast_to_ir_translate_compilation_unit(AstToIRTranslator* translator, Ast* ast);
IRFunction* ast_to_ir_translate_function(AstToIRTranslator* translator, Ast* ast);
IRFunction* ast_to_ir_translate_method(AstToIRTranslator* translator, Ast* ast);
IRFunction* ast_to_ir_translate_constructor(AstToIRTranslator* translator, Ast* ast);

// Expression translation
IRValue ast_to_ir_translate_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_binary_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_some_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_none_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_ok_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_err_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_unary_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_literal(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_identifier(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_method_call(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_field_access(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_array_access(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_assignment_expression(AstToIRTranslator* translator, Ast* ast);
IRValue ast_to_ir_translate_new_expression(AstToIRTranslator* translator, Ast* ast);

// Statement translation
void ast_to_ir_translate_statement(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_declaration(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_assignment(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_expression_statement(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_return_statement(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_if_statement(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_while_statement(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_for_statement(AstToIRTranslator* translator, Ast* ast);
void ast_to_ir_translate_match_statement(AstToIRTranslator* translator, Ast* ast);

// Type translation
uint32_t ast_to_ir_translate_type(AstToIRTranslator* translator, Ast* ast);
uint32_t ast_to_ir_get_builtin_type_id(const char* type_name);

// Symbol table functions
void ast_to_ir_add_symbol(AstToIRTranslator* translator, const char* name, uint32_t type_id, bool is_local);
void ast_to_ir_add_type(AstToIRTranslator* translator, const char* name, uint32_t type_id, uint32_t size);
uint32_t ast_to_ir_find_symbol(AstToIRTranslator* translator, const char* name);
bool ast_to_ir_is_local_symbol(AstToIRTranslator* translator, const char* name);

// Error handling
void ast_to_ir_translator_set_error(AstToIRTranslator* translator, const char* message);
const char* ast_to_ir_translator_get_error(AstToIRTranslator* translator);
bool ast_to_ir_translator_has_error(AstToIRTranslator* translator);

// Utility functions
IRValue ast_to_ir_create_temp_value(AstToIRTranslator* translator, IRValueType type);
IRValue ast_to_ir_create_literal_value(Ast* ast);
bool ast_to_ir_is_arithmetic_operation(AstKind kind);
bool ast_to_ir_is_comparison_operation(AstKind kind);
bool ast_to_ir_is_logical_operation(AstKind kind);
IROp ast_to_ir_get_arithmetic_op(AstKind kind);
IROp ast_to_ir_get_comparison_op(AstKind kind);
IROp ast_to_ir_get_logical_op(AstKind kind);

// Built-in function registration
void ast_to_ir_register_builtin_functions(AstToIRTranslator* translator);
