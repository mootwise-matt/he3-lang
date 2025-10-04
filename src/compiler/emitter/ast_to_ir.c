#include "ast_to_ir.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Built-in type IDs
#define TYPE_ID_INTEGER    1
#define TYPE_ID_FLOAT      2
#define TYPE_ID_BOOLEAN    3
#define TYPE_ID_STRING     4
#define TYPE_ID_OBJECT     5
#define TYPE_ID_VOID       6

// Symbol table entry
typedef struct SymbolEntry {
    char* name;
    uint32_t type_id;
    bool is_local;
    uint32_t local_index;
    uint32_t scope_depth;
} SymbolEntry;

// Simple symbol table
typedef struct SymbolTable {
    SymbolEntry* entries;
    uint32_t count;
    uint32_t capacity;
    uint32_t next_local_index;
} SymbolTable;

// Type table entry
typedef struct TypeEntry {
    char* name;
    uint32_t type_id;
    uint32_t size;
} TypeEntry;

// Simple type table
typedef struct TypeTable {
    TypeEntry* entries;
    uint32_t count;
    uint32_t capacity;
} TypeTable;

// AST to IR translator implementation
AstToIRTranslator* ast_to_ir_translator_create(void) {
    AstToIRTranslator* translator = malloc(sizeof(AstToIRTranslator));
    if (!translator) return NULL;
    
    translator->ir_builder = ir_builder_create();
    if (!translator->ir_builder) {
        free(translator);
        return NULL;
    }
    
    // Create symbol table
    translator->symbol_table = malloc(sizeof(SymbolTable));
    if (!translator->symbol_table) {
        ir_builder_destroy(translator->ir_builder);
        free(translator);
        return NULL;
    }
    
    translator->symbol_table->entries = NULL;
    translator->symbol_table->count = 0;
    translator->symbol_table->capacity = 0;
    translator->symbol_table->next_local_index = 0;
    
    // Create type table
    translator->type_table = malloc(sizeof(TypeTable));
    if (!translator->type_table) {
        free(translator->symbol_table);
        ir_builder_destroy(translator->ir_builder);
        free(translator);
        return NULL;
    }
    
    translator->type_table->entries = NULL;
    translator->type_table->count = 0;
    translator->type_table->capacity = 0;
    
    // Initialize built-in types
    ast_to_ir_add_type(translator, "integer", TYPE_ID_INTEGER, 8);
    ast_to_ir_add_type(translator, "float", TYPE_ID_FLOAT, 8);
    ast_to_ir_add_type(translator, "boolean", TYPE_ID_BOOLEAN, 1);
    ast_to_ir_add_type(translator, "string", TYPE_ID_STRING, 8);
    ast_to_ir_add_type(translator, "object", TYPE_ID_OBJECT, 8);
    ast_to_ir_add_type(translator, "void", TYPE_ID_VOID, 0);
    
    translator->current_function = NULL;
    translator->current_block = NULL;
    translator->current_scope_depth = 0;
    translator->error_message = NULL;
    translator->has_error = false;
    
    // Register built-in functions
    ast_to_ir_register_builtin_functions(translator);
    
    return translator;
}

void ast_to_ir_translator_destroy(AstToIRTranslator* translator) {
    if (!translator) return;
    
    if (translator->ir_builder) {
        ir_builder_destroy(translator->ir_builder);
    }
    
    if (translator->symbol_table) {
        for (uint32_t i = 0; i < translator->symbol_table->count; i++) {
            if (translator->symbol_table->entries[i].name) {
                free(translator->symbol_table->entries[i].name);
            }
        }
        free(translator->symbol_table->entries);
        free(translator->symbol_table);
    }
    
    if (translator->type_table) {
        for (uint32_t i = 0; i < translator->type_table->count; i++) {
            if (translator->type_table->entries[i].name) {
                free(translator->type_table->entries[i].name);
            }
        }
        free(translator->type_table->entries);
        free(translator->type_table);
    }
    
    if (translator->error_message) {
        free(translator->error_message);
    }
    
    free(translator);
}

// Type table helper functions
void ast_to_ir_add_type(AstToIRTranslator* translator, const char* name, uint32_t type_id, uint32_t size) {
    if (!translator || !translator->type_table) return;
    
    TypeTable* table = translator->type_table;
    
    // Resize if needed
    if (table->count >= table->capacity) {
        table->capacity = table->capacity == 0 ? 8 : table->capacity * 2;
        table->entries = realloc(table->entries, sizeof(TypeEntry) * table->capacity);
        if (!table->entries) return;
    }
    
    // Add entry
    table->entries[table->count].name = strdup(name);
    table->entries[table->count].type_id = type_id;
    table->entries[table->count].size = size;
    table->count++;
}

// Main translation functions
IRFunction* ast_to_ir_translate_compilation_unit(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->kind != AST_COMPUNIT) {
        ast_to_ir_translator_set_error(translator, "Expected compilation unit");
        return NULL;
    }
    
    
    // For now, we'll translate the first function we find
    // In a full implementation, we'd handle multiple functions and classes
    for (uint32_t i = 0; i < ast->child_count; i++) {
        Ast* child = ast->children[i];
        
        if (child->kind == AST_CLASS) {
            // Look for methods inside the class
            for (uint32_t j = 0; j < child->child_count; j++) {
                Ast* method = child->children[j];
                if (method->kind == AST_METHOD) {
                    return ast_to_ir_translate_function(translator, method);
                }
            }
        } else if (child->kind == AST_DOMAIN) {
            // Look for methods inside the domain
            for (uint32_t j = 0; j < child->child_count; j++) {
                Ast* domain_child = child->children[j];
                if (domain_child->kind == AST_METHOD) {
                    return ast_to_ir_translate_function(translator, domain_child);
                } else if (domain_child->kind == AST_CLASS) {
                    // Look for methods inside classes within the domain
                    for (uint32_t k = 0; k < domain_child->child_count; k++) {
                        Ast* method = domain_child->children[k];
                        if (method->kind == AST_METHOD) {
                            return ast_to_ir_translate_function(translator, method);
                        }
                    }
                }
            }
        } else if (child->kind == AST_METHOD) {
            // Handle standalone methods
            return ast_to_ir_translate_function(translator, child);
        }
    }
    
    ast_to_ir_translator_set_error(translator, "No function found in compilation unit");
    return NULL;
}

IRFunction* ast_to_ir_translate_function(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->kind != AST_METHOD) {
        ast_to_ir_translator_set_error(translator, "Expected function");
        return NULL;
    }
    
    // Get function name
    const char* function_name = "main"; // Default name
    if (ast->identifier) {
        function_name = ast->identifier;
    }
    
    // Parse parameters
    uint32_t param_count = 0;
    uint32_t* param_types = NULL;
    Ast* params_ast = NULL;
    
    // Find parameters AST
    for (uint32_t i = 0; i < ast->child_count; i++) {
        if (ast->children[i]->kind == AST_ARGUMENTS) {
            params_ast = ast->children[i];
            param_count = params_ast->child_count;
            break;
        }
    }
    
    // Allocate parameter types
    if (param_count > 0) {
        param_types = malloc(param_count * sizeof(uint32_t));
        if (!param_types) {
            ast_to_ir_translator_set_error(translator, "Failed to allocate parameter types");
            return NULL;
        }
        
        // Parse parameter types
        for (uint32_t i = 0; i < param_count; i++) {
            Ast* param = params_ast->children[i];
            if (param->child_count > 0) {
                param_types[i] = ast_to_ir_translate_type(translator, param->children[0]);
            } else {
                param_types[i] = TYPE_ID_INTEGER; // Default type
            }
        }
    }
    
    // Get return type
    uint32_t return_type = TYPE_ID_INTEGER; // Default return type
    for (uint32_t i = 0; i < ast->child_count; i++) {
        if (ast->children[i]->kind == AST_LITERAL) {
            // This is the return type
            return_type = ast_to_ir_translate_type(translator, ast->children[i]);
            break;
        }
    }
    
    // Create IR function
    IRFunction* function = ir_builder_create_function(translator->ir_builder, function_name, return_type);
    if (!function) {
        ast_to_ir_translator_set_error(translator, "Failed to create IR function");
        free(param_types);
        return NULL;
    }
    
    // Set static flag from AST
    function->is_static = ast->is_static;
    
    // Set parameter types
    if (param_count > 0) {
        function->param_count = param_count;
        function->param_types = param_types;
    }
    
    translator->current_function = function;
    
    // Create entry block
    IRBlock* entry_block = ir_builder_create_block(translator->ir_builder, "entry");
    if (!entry_block) {
        ast_to_ir_translator_set_error(translator, "Failed to create entry block");
        free(param_types);
        return NULL;
    }
    
    ir_builder_set_current_block(translator->ir_builder, entry_block);
    translator->current_block = entry_block;
    
    // Add parameters to symbol table
    if (params_ast) {
        for (uint32_t i = 0; i < param_count; i++) {
            Ast* param = params_ast->children[i];
            if (param->identifier) {
                ast_to_ir_add_symbol(translator, param->identifier, param_types[i], true);
            }
        }
    }
    
    // Translate function body
    for (uint32_t i = 0; i < ast->child_count; i++) {
        Ast* child = ast->children[i];
        if (child->kind == AST_BLOCK) {
            // Translate all statements in the block
            for (uint32_t j = 0; j < child->child_count; j++) {
                ast_to_ir_translate_statement(translator, child->children[j]);
            }
        } else if (child->kind != AST_ARGUMENTS && child->kind != AST_LITERAL) {
            ast_to_ir_translate_statement(translator, child);
        }
    }
    
    return function;
}

// Expression translation
IRValue ast_to_ir_translate_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    switch (ast->kind) {
        case AST_BINARY:
            return ast_to_ir_translate_binary_expression(translator, ast);
        case AST_UNARY:
            return ast_to_ir_translate_unary_expression(translator, ast);
        case AST_LITERAL:
            return ast_to_ir_translate_literal(translator, ast);
        case AST_IDENTIFIER:
            return ast_to_ir_translate_identifier(translator, ast);
        case AST_CALL:
            return ast_to_ir_translate_method_call(translator, ast);
        case AST_FIELD_ACCESS:
            return ast_to_ir_translate_field_access(translator, ast);
        case AST_INDEX_ACCESS:
            return ast_to_ir_translate_array_access(translator, ast);
        case AST_ASSIGN:
            return ast_to_ir_translate_assignment_expression(translator, ast);
        case AST_NEW:
            return ast_to_ir_translate_new_expression(translator, ast);
        case AST_SOME:
            return ast_to_ir_translate_some_expression(translator, ast);
        case AST_NONE:
            return ast_to_ir_translate_none_expression(translator, ast);
        case AST_OK:
            return ast_to_ir_translate_ok_expression(translator, ast);
        case AST_ERR:
            return ast_to_ir_translate_err_expression(translator, ast);
        default:
            ast_to_ir_translator_set_error(translator, "Unsupported expression type");
            IRValue null_value = {0};
            return null_value;
    }
}

IRValue ast_to_ir_translate_binary_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate left and right operands - these will add LOAD_LOCAL instructions to IR
    ast_to_ir_translate_expression(translator, ast->children[0]);
    ast_to_ir_translate_expression(translator, ast->children[1]);
    
    // Determine the operation based on the stored operator text
    IROp op = IR_ADD; // Default
    
    if (ast->text) {
        if (strcmp(ast->text, "PLUS") == 0) {
            op = IR_ADD;
        } else if (strcmp(ast->text, "MINUS") == 0) {
            op = IR_SUB;
        } else if (strcmp(ast->text, "STAR") == 0) {
            op = IR_MUL;
        } else if (strcmp(ast->text, "SLASH") == 0) {
            op = IR_DIV;
        } else if (strcmp(ast->text, "MODULO") == 0) {
            op = IR_MOD;
        } else if (strcmp(ast->text, "EQUAL") == 0) {
            op = IR_EQ;
        } else if (strcmp(ast->text, "NOT_EQUAL") == 0) {
            op = IR_NE;
        } else if (strcmp(ast->text, "LESS") == 0) {
            op = IR_LT;
        } else if (strcmp(ast->text, "LESS_EQUAL") == 0) {
            op = IR_LE;
        } else if (strcmp(ast->text, "GREATER") == 0) {
            op = IR_GT;
        } else if (strcmp(ast->text, "GREATER_EQUAL") == 0) {
            op = IR_GE;
        } else if (strcmp(ast->text, "AND") == 0) {
            op = IR_AND;
        } else if (strcmp(ast->text, "OR") == 0) {
            op = IR_OR;
        }
    }
    
    IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, op);
    if (!instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    // ADD instruction works with values on the stack, no operands needed
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_I64);
    ir_instruction_set_result(instruction, result);
    
    ir_builder_add_instruction(translator->ir_builder, instruction);
    
    return result;
}

IRValue ast_to_ir_translate_unary_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 1) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate operand
    IRValue operand = ast_to_ir_translate_expression(translator, ast->children[0]);
    
    // For now, default to negation
    // In a full implementation, we'd need to store the operator type in the AST
    IROp op = IR_NEG;
    
    IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, op);
    if (!instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    ir_instruction_add_operand(instruction, operand);
    
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_I64);
    ir_instruction_set_result(instruction, result);
    
    ir_builder_add_instruction(translator->ir_builder, instruction);
    
    return result;
}

IRValue ast_to_ir_translate_literal(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Create load constant instruction
    IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, IR_LOAD_CONST);
    if (!instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    IRValue literal_value = ast_to_ir_create_literal_value(ast);
    ir_instruction_add_operand(instruction, literal_value);
    
    IRValue result = ast_to_ir_create_temp_value(translator, literal_value.type);
    ir_instruction_set_result(instruction, result);
    
    ir_builder_add_instruction(translator->ir_builder, instruction);
    
    return result;
}

IRValue ast_to_ir_translate_identifier(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || !ast->identifier) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Look up symbol
    uint32_t symbol_id = ast_to_ir_find_symbol(translator, ast->identifier);
    if (symbol_id == 0) {
        ast_to_ir_translator_set_error(translator, "Undefined symbol");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Create load instruction
    IROp op = ast_to_ir_is_local_symbol(translator, ast->identifier) ? IR_LOAD_LOCAL : IR_LOAD_STATIC;
    IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, op);
    if (!instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    IRValue symbol_value = {0};
    symbol_value.type = IR_VALUE_TEMP;
    symbol_value.data.temp_id = symbol_id - 1; // Convert back to 0-based index
    ir_instruction_add_operand(instruction, symbol_value);
    
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_I64);
    ir_instruction_set_result(instruction, result);
    
    ir_builder_add_instruction(translator->ir_builder, instruction);
    
    return result;
}

IRValue ast_to_ir_translate_method_call(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Parse method call: callee(args)
    if (ast->child_count < 2) {
        ast_to_ir_translator_set_error(translator, "Method call must have callee and arguments");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate arguments
    Ast* args_ast = ast->children[1];
    if (args_ast->kind != AST_ARGUMENTS) {
        ast_to_ir_translator_set_error(translator, "Expected arguments in method call");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Count arguments
    uint32_t arg_count = args_ast->child_count;
    
    // Determine if this is a static method call
    // Check if the callee is a field access (e.g., Sys.print or option.is_some)
    bool is_static_call = false;
    bool is_sys_call = false;
    bool is_option_call = false;
    if (ast->children[0]->kind == AST_FIELD_ACCESS) {
        // This is a field access like Sys.print or option.is_some
        // Check if it's a Sys method call or Option method call
        Ast* field_access = ast->children[0];
        // The field access structure is: identifier = method name, child[0] = object
        if (field_access->child_count >= 1 && field_access->identifier) {
            Ast* object = field_access->children[0];
            const char* method_name = field_access->identifier;
            if (object->kind == AST_IDENTIFIER && object->identifier) {
                if (strcmp(object->identifier, "Sys") == 0) {
                    // This is a Sys method call - treat as static
                    is_static_call = true;
                    is_sys_call = true;
                } else if (strcmp(method_name, "is_some") == 0 || strcmp(method_name, "unwrap") == 0) {
                    // This is an Option method call
                    is_option_call = true;
                }
            }
        }
    }
    
    // Handle Option method calls directly
    if (is_option_call) {
        Ast* field_access = ast->children[0];
        const char* method_name = field_access->identifier;
        
        // First, load the Option variable onto the stack
        ast_to_ir_translate_expression(translator, field_access->children[0]);
        
        // Then generate the appropriate Option opcode
        if (strcmp(method_name, "is_some") == 0) {
            ir_builder_add_option_is_some(translator->ir_builder);
        } else if (strcmp(method_name, "unwrap") == 0) {
            ir_builder_add_option_unwrap(translator->ir_builder);
        }
        
        // Create result value
        IRValue result = {0};
        return result;
    }
    
    // Translate callee (object.method or function name)
    IRValue callee;
    if (is_sys_call) {
        // For Sys calls, create a special marker value
        Ast* field_access = ast->children[0];
        const char* method_name = field_access->identifier;
        
        // Store the method name as a special marker
        // We'll detect this in bytecode generation by checking if it's a static call
        callee.type = IR_VALUE_I64;
        if (strcmp(method_name, "print") == 0) {
            callee.data.i64 = 2; // Sys.print method ID
        } else if (strcmp(method_name, "println") == 0) {
            callee.data.i64 = 3; // Sys.println method ID
        } else if (strcmp(method_name, "currentTimeMillis") == 0) {
            callee.data.i64 = 12; // Sys.currentTimeMillis method ID
        } else {
            callee.data.i64 = 0; // Unknown method
        }
    } else {
        callee = ast_to_ir_translate_expression(translator, ast->children[0]);
    }
    
    // Create call instruction
    IROp call_op = is_static_call ? IR_CALL_STATIC : IR_CALL;
    IRInstruction* call_instruction = ir_builder_create_instruction(translator->ir_builder, call_op);
    if (!call_instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create call instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Add callee as first operand
    ir_instruction_add_operand(call_instruction, callee);
    
    // Add argument count
    IRValue arg_count_value = {0};
    arg_count_value.type = IR_VALUE_I64;
    arg_count_value.data.i64 = arg_count;
    ir_instruction_add_operand(call_instruction, arg_count_value);
    
    // Add each argument
    for (uint32_t i = 0; i < arg_count; i++) {
        IRValue arg = ast_to_ir_translate_expression(translator, args_ast->children[i]);
        ir_instruction_add_operand(call_instruction, arg);
    }
    
    // Create result value
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_I64);
    ir_instruction_set_result(call_instruction, result);
    
    // Add instruction to current block
    ir_builder_add_instruction(translator->ir_builder, call_instruction);
    
    return result;
}

IRValue ast_to_ir_translate_field_access(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Parse field access: object.field
    IRValue object = ast_to_ir_translate_expression(translator, ast->children[0]);
    
    // Check if this is a method call on an Option type
    if (ast->children[0]->kind == AST_IDENTIFIER && ast->identifier) {
        // This is a method call like option.is_some or option.unwrap
        if (strcmp(ast->identifier, "is_some") == 0) {
            // Generate IR_OPTION_IS_SOME instruction
            ir_builder_add_option_is_some(translator->ir_builder);
            IRValue result = {0};
            return result;
        } else if (strcmp(ast->identifier, "unwrap") == 0) {
            // Generate IR_OPTION_UNWRAP instruction
            ir_builder_add_option_unwrap(translator->ir_builder);
            IRValue result = {0};
            return result;
        }
    }
    
    // Default field access handling
    IRValue field = ast_to_ir_translate_expression(translator, ast->children[1]);
    
    // Create field access instruction
    IRInstruction* field_instruction = ir_builder_create_instruction(translator->ir_builder, IR_LOAD_FIELD);
    if (!field_instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create field access instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Add object and field as operands
    ir_instruction_add_operand(field_instruction, object);
    ir_instruction_add_operand(field_instruction, field);
    
    // Create result value
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_I64);
    ir_instruction_set_result(field_instruction, result);
    
    // Add instruction to current block
    ir_builder_add_instruction(translator->ir_builder, field_instruction);
    
    return result;
}

IRValue ast_to_ir_translate_array_access(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Parse array access: array[index]
    IRValue array = ast_to_ir_translate_expression(translator, ast->children[0]);
    IRValue index = ast_to_ir_translate_expression(translator, ast->children[1]);
    
    // Create array access instruction
    IRInstruction* array_instruction = ir_builder_create_instruction(translator->ir_builder, IR_LOAD_ARRAY);
    if (!array_instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create array access instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Add array and index as operands
    ir_instruction_add_operand(array_instruction, array);
    ir_instruction_add_operand(array_instruction, index);
    
    // Create result value
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_I64);
    ir_instruction_set_result(array_instruction, result);
    
    // Add instruction to current block
    ir_builder_add_instruction(translator->ir_builder, array_instruction);
    
    return result;
}

// Statement translation
void ast_to_ir_translate_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) return;
    
    switch (ast->kind) {
        case AST_BLOCK:
            // Translate all statements in the block
            for (uint32_t i = 0; i < ast->child_count; i++) {
                ast_to_ir_translate_statement(translator, ast->children[i]);
            }
            break;
        case AST_VAR_DECL:
            ast_to_ir_translate_declaration(translator, ast);
            break;
        case AST_ASSIGN:
            ast_to_ir_translate_assignment(translator, ast);
            break;
        case AST_EXPR_STMT:
            ast_to_ir_translate_expression_statement(translator, ast);
            break;
        case AST_RETURN:
            ast_to_ir_translate_return_statement(translator, ast);
            break;
        case AST_IF:
            ast_to_ir_translate_if_statement(translator, ast);
            break;
        case AST_WHILE:
            ast_to_ir_translate_while_statement(translator, ast);
            break;
        case AST_FOR:
            ast_to_ir_translate_for_statement(translator, ast);
            break;
        case AST_MATCH:
            ast_to_ir_translate_match_statement(translator, ast);
            break;
        default:
            // Ignore unsupported statement types for now
            break;
    }
}

void ast_to_ir_translate_declaration(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || !ast->identifier) return;
    
    // Get variable type
    uint32_t type_id = TYPE_ID_INTEGER; // Default type
    if (ast->child_count > 0) {
        type_id = ast_to_ir_translate_type(translator, ast->children[0]);
    }
    
    // Add symbol to table
    ast_to_ir_add_symbol(translator, ast->identifier, type_id, true);
    
    // If there's an initializer, translate it
    if (ast->child_count > 1) {
        // Translate the initializer expression (this will push the value onto the stack)
        ast_to_ir_translate_expression(translator, ast->children[1]);
        
        // Store to local variable
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, IR_STORE_LOCAL);
        if (instruction) {
            IRValue symbol_value = {0};
            symbol_value.type = IR_VALUE_TEMP;
            uint32_t symbol_id = ast_to_ir_find_symbol(translator, ast->identifier);
            symbol_value.data.temp_id = symbol_id > 0 ? symbol_id - 1 : 0; // Convert back to 0-based index
            ir_instruction_add_operand(instruction, symbol_value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
        }
    } else {
        // Initialize with default value - first push the value, then store it
        IRValue default_value = {0};
        default_value.type = IR_VALUE_I64;
        default_value.data.i64 = 0;
        
        // Push the default value
        IRInstruction* push_instruction = ir_builder_create_instruction(translator->ir_builder, IR_LOAD_CONST);
        if (push_instruction) {
            ir_instruction_add_operand(push_instruction, default_value);
            ir_builder_add_instruction(translator->ir_builder, push_instruction);
        }
        
        // Store to local variable
        IRInstruction* store_instruction = ir_builder_create_instruction(translator->ir_builder, IR_STORE_LOCAL);
        if (store_instruction) {
            IRValue symbol_value = {0};
            symbol_value.type = IR_VALUE_TEMP;
            uint32_t symbol_id = ast_to_ir_find_symbol(translator, ast->identifier);
            symbol_value.data.temp_id = symbol_id > 0 ? symbol_id - 1 : 0; // Convert back to 0-based index
            ir_instruction_add_operand(store_instruction, symbol_value);
            ir_builder_add_instruction(translator->ir_builder, store_instruction);
        }
    }
}

void ast_to_ir_translate_assignment(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) return;
    
    // Translate right-hand side
    IRValue value = ast_to_ir_translate_expression(translator, ast->children[1]);
    
    // Determine if this is a local variable or field access
    Ast* left_side = ast->children[0];
    IROp store_op = IR_STORE_LOCAL; // Default to local
    
    if (left_side->kind == AST_IDENTIFIER) {
        // Simple variable assignment
        uint32_t symbol_id = ast_to_ir_find_symbol(translator, left_side->identifier);
        if (symbol_id == 0) {
            ast_to_ir_translator_set_error(translator, "Undefined variable in assignment");
            return;
        }
        
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, store_op);
        if (instruction) {
            IRValue symbol_value = {0};
            symbol_value.type = IR_VALUE_TEMP;
            symbol_value.data.temp_id = symbol_id - 1; // Convert back to 0-based index
            ir_instruction_add_operand(instruction, symbol_value);
            ir_instruction_add_operand(instruction, value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
        }
    } else if (left_side->kind == AST_FIELD_ACCESS) {
        // Field assignment - for now, treat as local
        // Full implementation would handle object field access
        store_op = IR_STORE_FIELD;
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, store_op);
        if (instruction) {
            // Translate the object and field name
            IRValue object = ast_to_ir_translate_expression(translator, left_side->children[0]);
            IRValue field = ast_to_ir_translate_expression(translator, left_side->children[1]);
            ir_instruction_add_operand(instruction, object);
            ir_instruction_add_operand(instruction, field);
            ir_instruction_add_operand(instruction, value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
        }
    } else if (left_side->kind == AST_INDEX_ACCESS) {
        // Array assignment - for now, treat as local
        // Full implementation would handle array indexing
        store_op = IR_STORE_ARRAY;
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, store_op);
        if (instruction) {
            // Translate the array and index
            IRValue array = ast_to_ir_translate_expression(translator, left_side->children[0]);
            IRValue index = ast_to_ir_translate_expression(translator, left_side->children[1]);
            ir_instruction_add_operand(instruction, array);
            ir_instruction_add_operand(instruction, index);
            ir_instruction_add_operand(instruction, value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
        }
    }
}

IRValue ast_to_ir_translate_assignment_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate right-hand side
    IRValue value = ast_to_ir_translate_expression(translator, ast->children[1]);
    
    // Determine if this is a local variable or field access
    Ast* left_side = ast->children[0];
    IROp store_op = IR_STORE_LOCAL; // Default to local
    
    if (left_side->kind == AST_IDENTIFIER) {
        // Simple variable assignment
        uint32_t symbol_id = ast_to_ir_find_symbol(translator, left_side->identifier);
        if (symbol_id == 0) {
            ast_to_ir_translator_set_error(translator, "Undefined variable in assignment");
            IRValue null_value = {0};
            return null_value;
        }
        
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, store_op);
        if (instruction) {
            IRValue symbol_value = {0};
            symbol_value.type = IR_VALUE_TEMP;
            symbol_value.data.temp_id = symbol_id - 1; // Convert back to 0-based index
            ir_instruction_add_operand(instruction, symbol_value);
            ir_instruction_add_operand(instruction, value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
        }
        
        // Return the assigned value
        return value;
    } else if (left_side->kind == AST_FIELD_ACCESS) {
        // Field assignment - for now, treat as local
        // Full implementation would handle object field access
        ast_to_ir_translator_set_error(translator, "Field assignment not yet implemented");
        IRValue null_value = {0};
        return null_value;
    }
    
    IRValue null_value = {0};
    return null_value;
}

void ast_to_ir_translate_expression_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 1) return;
    
    // Just translate the expression (result is discarded)
    ast_to_ir_translate_expression(translator, ast->children[0]);
}

void ast_to_ir_translate_return_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) return;
    
    printf("DEBUG: Translating return statement in block %u\n", translator->ir_builder->current_block->id);
    if (ast->child_count > 0) {
        // Return with value
        IRValue value = ast_to_ir_translate_expression(translator, ast->children[0]);
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, IR_RETURN_VAL);
        if (instruction) {
            ir_instruction_add_operand(instruction, value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
            printf("DEBUG: Added return instruction to block %u\n", translator->ir_builder->current_block->id);
        }
    } else {
        // Return without value (void return)
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, IR_RETURN);
        if (instruction) {
            ir_builder_add_instruction(translator->ir_builder, instruction);
        }
    }
}

void ast_to_ir_translate_if_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) return;
    
    // Translate condition
    IRValue condition = ast_to_ir_translate_expression(translator, ast->children[0]);
    
    // Create then block
    IRBlock* then_block = ir_builder_create_block(translator->ir_builder, "then");
    if (!then_block) return;
    
    // Create else block (if needed)
    IRBlock* else_block = NULL;
    if (ast->child_count > 2) {
        else_block = ir_builder_create_block(translator->ir_builder, "else");
        if (!else_block) return;
    }
    
    // Create merge block
    IRBlock* merge_block = ir_builder_create_block(translator->ir_builder, "merge");
    if (!merge_block) return;
    
    // Add conditional jump
    IRInstruction* jump_instruction = ir_builder_create_instruction(translator->ir_builder, IR_JMPF);
    if (jump_instruction) {
        ir_instruction_add_operand(jump_instruction, condition);
        ir_instruction_set_target(jump_instruction, else_block ? else_block->id : merge_block->id);
        ir_builder_add_instruction(translator->ir_builder, jump_instruction);
        // printf("DEBUG: Added IR_JMPF to block %u, target=%u\n", 
        //        translator->ir_builder->current_block->id, 
        //        else_block ? else_block->id : merge_block->id);
    }
    
    // Translate then block
    ir_builder_set_current_block(translator->ir_builder, then_block);
    translator->current_block = then_block;
    // printf("DEBUG: Translating then block %u statement (AST kind=%d, child_count=%d)\n", 
    //        then_block->id, ast->children[1]->kind, ast->children[1]->child_count);
    ast_to_ir_translate_statement(translator, ast->children[1]);
    // printf("DEBUG: Then block %u now has %u instructions\n", then_block->id, then_block->instruction_count);
    
    // Jump to merge block from then block
    // printf("DEBUG: Adding jump from then block %u to merge block %u\n", 
    //        translator->ir_builder->current_block->id, merge_block->id);
    ir_builder_add_jump(translator->ir_builder, merge_block);
    
    // Translate else block if it exists
    if (else_block) {
        ir_builder_set_current_block(translator->ir_builder, else_block);
        translator->current_block = else_block;
        ast_to_ir_translate_statement(translator, ast->children[2]);
        ir_builder_add_jump(translator->ir_builder, merge_block);
    }
    
    // Set current block to merge block
    ir_builder_set_current_block(translator->ir_builder, merge_block);
    translator->current_block = merge_block;
}

void ast_to_ir_translate_while_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 2) return;
    
    // Create loop header block
    IRBlock* loop_header = ir_builder_create_block(translator->ir_builder, "loop_header");
    if (!loop_header) return;
    
    // Create loop body block
    IRBlock* loop_body = ir_builder_create_block(translator->ir_builder, "loop_body");
    if (!loop_body) return;
    
    // Create loop exit block
    IRBlock* loop_exit = ir_builder_create_block(translator->ir_builder, "loop_exit");
    if (!loop_exit) return;
    
    // Jump to loop header
    ir_builder_add_jump(translator->ir_builder, loop_header);
    
    // Set current block to loop header
    ir_builder_set_current_block(translator->ir_builder, loop_header);
    translator->current_block = loop_header;
    
    // Translate condition
    IRValue condition = ast_to_ir_translate_expression(translator, ast->children[0]);
    
    // Add conditional jump to loop body or exit
    IRInstruction* jump_instruction = ir_builder_create_instruction(translator->ir_builder, IR_JMPF);
    if (jump_instruction) {
        ir_instruction_add_operand(jump_instruction, condition);
        ir_instruction_set_target(jump_instruction, loop_exit->id);
        ir_builder_add_instruction(translator->ir_builder, jump_instruction);
    }
    
    // Jump to loop body
    ir_builder_add_jump(translator->ir_builder, loop_body);
    
    // Translate loop body
    ir_builder_set_current_block(translator->ir_builder, loop_body);
    translator->current_block = loop_body;
    ast_to_ir_translate_statement(translator, ast->children[1]);
    
    // Jump back to loop header
    ir_builder_add_jump(translator->ir_builder, loop_header);
    
    // Set current block to loop exit
    ir_builder_set_current_block(translator->ir_builder, loop_exit);
    translator->current_block = loop_exit;
}

void ast_to_ir_translate_for_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) return;
    
    // Create for loop blocks
    IRBlock* init_block = ir_builder_create_block(translator->ir_builder, "for_init");
    if (!init_block) return;
    
    IRBlock* condition_block = ir_builder_create_block(translator->ir_builder, "for_condition");
    if (!condition_block) return;
    
    IRBlock* body_block = ir_builder_create_block(translator->ir_builder, "for_body");
    if (!body_block) return;
    
    IRBlock* increment_block = ir_builder_create_block(translator->ir_builder, "for_increment");
    if (!increment_block) return;
    
    IRBlock* exit_block = ir_builder_create_block(translator->ir_builder, "for_exit");
    if (!exit_block) return;
    
    // Jump to initialization
    ir_builder_add_jump(translator->ir_builder, init_block);
    
    // Translate initialization (if present)
    ir_builder_set_current_block(translator->ir_builder, init_block);
    translator->current_block = init_block;
    if (ast->child_count > 0 && ast->children[0]) {
        ast_to_ir_translate_statement(translator, ast->children[0]);
    }
    
    // Jump to condition
    ir_builder_add_jump(translator->ir_builder, condition_block);
    
    // Translate condition
    ir_builder_set_current_block(translator->ir_builder, condition_block);
    translator->current_block = condition_block;
    if (ast->child_count > 1 && ast->children[1]) {
        IRValue condition = ast_to_ir_translate_expression(translator, ast->children[1]);
        
        // Add conditional jump to body or exit
        IRInstruction* jump_instruction = ir_builder_create_instruction(translator->ir_builder, IR_JMPF);
        if (jump_instruction) {
            ir_instruction_add_operand(jump_instruction, condition);
            ir_instruction_set_target(jump_instruction, exit_block->id);
            ir_builder_add_instruction(translator->ir_builder, jump_instruction);
        }
    }
    
    // Jump to body
    ir_builder_add_jump(translator->ir_builder, body_block);
    
    // Translate body
    ir_builder_set_current_block(translator->ir_builder, body_block);
    translator->current_block = body_block;
    if (ast->child_count > 2 && ast->children[2]) {
        ast_to_ir_translate_statement(translator, ast->children[2]);
    }
    
    // Jump to increment
    ir_builder_add_jump(translator->ir_builder, increment_block);
    
    // Translate increment (if present)
    ir_builder_set_current_block(translator->ir_builder, increment_block);
    translator->current_block = increment_block;
    if (ast->child_count > 3 && ast->children[3]) {
        ast_to_ir_translate_statement(translator, ast->children[3]);
    }
    
    // Jump back to condition
    ir_builder_add_jump(translator->ir_builder, condition_block);
    
    // Set current block to exit
    ir_builder_set_current_block(translator->ir_builder, exit_block);
    translator->current_block = exit_block;
}

void ast_to_ir_translate_match_statement(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || ast->child_count < 1) return;
    
    // Translate the expression being matched
    printf("DEBUG: ast_to_ir_translate_match_statement: translating match expression kind=%d, identifier='%s'\n", 
           ast->children[0]->kind, ast->children[0]->identifier ? ast->children[0]->identifier : "NULL");
    (void)ast_to_ir_translate_expression(translator, ast->children[0]); // match_value
    
    // For now, implement a simple match that just unwraps the Option
    // This is a temporary solution to get match statements working
    printf("DEBUG: ast_to_ir_translate_match_statement: using simple unwrap approach\n");
    
    // Just unwrap the Option value directly
    ir_builder_add_option_unwrap(translator->ir_builder);
}

// Type translation
uint32_t ast_to_ir_translate_type(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) return TYPE_ID_INTEGER;
    
    if (ast->kind == AST_TYPE) {
        return ast_to_ir_get_builtin_type_id(ast->identifier);
    }
    
    return TYPE_ID_INTEGER; // Default type
}

uint32_t ast_to_ir_get_builtin_type_id(const char* type_name) {
    if (!type_name) return TYPE_ID_INTEGER;
    
    if (strcmp(type_name, "integer") == 0) return TYPE_ID_INTEGER;
    if (strcmp(type_name, "float") == 0) return TYPE_ID_FLOAT;
    if (strcmp(type_name, "boolean") == 0) return TYPE_ID_BOOLEAN;
    if (strcmp(type_name, "string") == 0) return TYPE_ID_STRING;
    if (strcmp(type_name, "object") == 0) return TYPE_ID_OBJECT;
    if (strcmp(type_name, "void") == 0) return TYPE_ID_VOID;
    
    return TYPE_ID_INTEGER; // Default
}

// Symbol table functions
void ast_to_ir_add_symbol(AstToIRTranslator* translator, const char* name, uint32_t type_id, bool is_local) {
    if (!translator || !translator->symbol_table || !name) return;
    
    SymbolTable* table = translator->symbol_table;
    
    // Resize if needed
    if (table->count >= table->capacity) {
        table->capacity = table->capacity == 0 ? 8 : table->capacity * 2;
        table->entries = realloc(table->entries, sizeof(SymbolEntry) * table->capacity);
        if (!table->entries) return;
    }
    
    // Add entry
    table->entries[table->count].name = strdup(name);
    table->entries[table->count].type_id = type_id;
    table->entries[table->count].is_local = is_local;
    table->entries[table->count].local_index = is_local ? table->next_local_index++ : 0;
    table->entries[table->count].scope_depth = translator->current_scope_depth;
    table->count++;
    
    // Update IR function local count if this is a local variable
    if (is_local && translator->ir_builder && translator->ir_builder->current_function) {
        translator->ir_builder->current_function->local_count = table->next_local_index;
    }
}

uint32_t ast_to_ir_find_symbol(AstToIRTranslator* translator, const char* name) {
    if (!translator || !translator->symbol_table || !name) return 0;
    
    SymbolTable* table = translator->symbol_table;
    
    // Search from most recent to oldest (for proper scoping)
    for (int32_t i = table->count - 1; i >= 0; i--) {
        if (strcmp(table->entries[i].name, name) == 0) {
            return table->entries[i].local_index + 1; // Return 1-based index (0 = not found)
        }
    }
    
    return 0; // Not found
}

bool ast_to_ir_is_local_symbol(AstToIRTranslator* translator, const char* name) {
    if (!translator || !translator->symbol_table || !name) return false;
    
    SymbolTable* table = translator->symbol_table;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            return table->entries[i].is_local;
        }
    }
    
    return false;
}

// Error handling
void ast_to_ir_translator_set_error(AstToIRTranslator* translator, const char* message) {
    if (!translator) return;
    
    if (translator->error_message) {
        free(translator->error_message);
    }
    
    translator->error_message = message ? strdup(message) : NULL;
    translator->has_error = true;
}

const char* ast_to_ir_translator_get_error(AstToIRTranslator* translator) {
    if (!translator) return NULL;
    return translator->error_message;
}

IRValue ast_to_ir_translate_new_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast || !ast->identifier) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Find constructor arguments
    Ast* arguments = NULL;
    for (uint32_t i = 0; i < ast->child_count; i++) {
        if (ast->children[i]->kind == AST_ARGUMENTS) {
            arguments = ast->children[i];
            break;
        }
    }
    
    // Translate constructor arguments
    if (arguments) {
        for (uint32_t i = 0; i < arguments->child_count; i++) {
            ast_to_ir_translate_expression(translator, arguments->children[i]);
        }
    }
    
    // Create NEW instruction
    IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, IR_NEW);
    if (!instruction) {
        ast_to_ir_translator_set_error(translator, "Failed to create NEW instruction");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Add class name as string operand
    IRValue class_name_value = {0};
    class_name_value.type = IR_VALUE_STRING;
    class_name_value.data.string_value = ast->identifier;
    ir_instruction_add_operand(instruction, class_name_value);
    
    // Add argument count as operand
    IRValue arg_count_value = {0};
    arg_count_value.type = IR_VALUE_I64;
    arg_count_value.data.i64 = arguments ? arguments->child_count : 0;
    ir_instruction_add_operand(instruction, arg_count_value);
    
    // Create result value
    IRValue result = ast_to_ir_create_temp_value(translator, IR_VALUE_OBJECT);
    ir_instruction_set_result(instruction, result);
    
    ir_builder_add_instruction(translator->ir_builder, instruction);
    
    return result;
}

bool ast_to_ir_translator_has_error(AstToIRTranslator* translator) {
    if (!translator) return false;
    return translator->has_error;
}

// Utility functions
IRValue ast_to_ir_create_temp_value(AstToIRTranslator* translator, IRValueType type) {
    return ir_builder_create_temp_value(translator->ir_builder, type);
}

IRValue ast_to_ir_create_literal_value(Ast* ast) {
    IRValue value = {0};
    
    if (!ast) return value;
    
    // Check which literal value is set - prioritize by type
    if (ast->text && ast->text[0] != '\0') {
        // String literal (has text content)
        value.type = IR_VALUE_STRING;
        // Store the string content directly instead of casting pointer to integer
        value.data.string_value = ast->text;
        return value;
    } else if (ast->literal.int_value != 0) {
        // Integer literal (check this before float to avoid union confusion)
        value.type = IR_VALUE_I64;
        value.data.i64 = ast->literal.int_value;
        return value;
    } else if (ast->literal.float_value != 0.0) {
        // Float literal
        value.type = IR_VALUE_F64;
        value.data.f64 = ast->literal.float_value;
        return value;
    } else if (ast->literal.bool_value) {
        // Boolean literal
        value.type = IR_VALUE_BOOL;
        value.data.boolean = true;
        return value;
    } else {
        // Default to integer 0
        value.type = IR_VALUE_I64;
        value.data.i64 = 0;
        return value;
    }
}

// Register built-in functions in the symbol table
void ast_to_ir_register_builtin_functions(AstToIRTranslator* translator) {
    if (!translator || !translator->symbol_table) return;
    
    // Register print function
    ast_to_ir_add_symbol(translator, "print", TYPE_ID_VOID, false);
    
    // Register other built-in functions as needed
    ast_to_ir_add_symbol(translator, "println", TYPE_ID_VOID, false);
    ast_to_ir_add_symbol(translator, "readLine", TYPE_ID_STRING, false);
    ast_to_ir_add_symbol(translator, "fileExists", TYPE_ID_BOOLEAN, false);
    ast_to_ir_add_symbol(translator, "readFile", TYPE_ID_STRING, false);
    ast_to_ir_add_symbol(translator, "writeFile", TYPE_ID_VOID, false);
    ast_to_ir_add_symbol(translator, "currentTimeMillis", TYPE_ID_INTEGER, false);
    ast_to_ir_add_symbol(translator, "exit", TYPE_ID_VOID, false);
    ast_to_ir_add_symbol(translator, "argc", TYPE_ID_INTEGER, false);
    ast_to_ir_add_symbol(translator, "argv", TYPE_ID_STRING, false);
    ast_to_ir_add_symbol(translator, "workingDir", TYPE_ID_STRING, false);
    ast_to_ir_add_symbol(translator, "changeDir", TYPE_ID_VOID, false);
    ast_to_ir_add_symbol(translator, "envGet", TYPE_ID_STRING, false);
    ast_to_ir_add_symbol(translator, "envSet", TYPE_ID_VOID, false);
}

// Option/Result expression translation
IRValue ast_to_ir_translate_some_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate the value being wrapped
    if (ast->child_count > 0) {
        ast_to_ir_translate_expression(translator, ast->children[0]);
    }
    
    // Add IR instruction to create Some(value)
    ir_builder_add_option_some(translator->ir_builder);
    
    IRValue result = {0};
    return result;
}

IRValue ast_to_ir_translate_none_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Add IR instruction to create None
    ir_builder_add_option_none(translator->ir_builder);
    
    IRValue result = {0};
    return result;
}

IRValue ast_to_ir_translate_ok_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate the value being wrapped
    if (ast->child_count > 0) {
        ast_to_ir_translate_expression(translator, ast->children[0]);
    }
    
    // Add IR instruction to create Ok(value)
    ir_builder_add_result_ok(translator->ir_builder);
    
    IRValue result = {0};
    return result;
}

IRValue ast_to_ir_translate_err_expression(AstToIRTranslator* translator, Ast* ast) {
    if (!translator || !ast) {
        IRValue null_value = {0};
        return null_value;
    }
    
    // Translate the error value being wrapped
    if (ast->child_count > 0) {
        ast_to_ir_translate_expression(translator, ast->children[0]);
    }
    
    // Add IR instruction to create Err(error)
    ir_builder_add_result_err(translator->ir_builder);
    
    IRValue result = {0};
    return result;
}

// These functions are not used in the current simplified implementation
// They would be needed for a full implementation that stores operator types in AST

