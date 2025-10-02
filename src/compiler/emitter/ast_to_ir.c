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
    
    printf("DEBUG: AST compilation unit has %u children\n", ast->child_count);
    
    // For now, we'll translate the first function we find
    // In a full implementation, we'd handle multiple functions and classes
    for (uint32_t i = 0; i < ast->child_count; i++) {
        Ast* child = ast->children[i];
        printf("DEBUG: Child %u: kind=%d, identifier=%s\n", i, child->kind, child->identifier ? child->identifier : "NULL");
        
        if (child->kind == AST_CLASS) {
            printf("DEBUG: Found class '%s' with %u children\n", child->identifier ? child->identifier : "NULL", child->child_count);
            // Look for methods inside the class
            for (uint32_t j = 0; j < child->child_count; j++) {
                Ast* method = child->children[j];
                printf("DEBUG: Class child %u: kind=%d, identifier=%s\n", j, method->kind, method->identifier ? method->identifier : "NULL");
                if (method->kind == AST_METHOD) {
                    printf("DEBUG: Found method '%s'\n", method->identifier ? method->identifier : "NULL");
                    return ast_to_ir_translate_function(translator, method);
                }
            }
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
    
    // For now, default to addition
    // In a full implementation, we'd need to store the operator type in the AST
    IROp op = IR_ADD;
    
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
    
    // Translate callee (object.method or function name)
    IRValue callee = ast_to_ir_translate_expression(translator, ast->children[0]);
    
    // Translate arguments
    Ast* args_ast = ast->children[1];
    if (args_ast->kind != AST_ARGUMENTS) {
        ast_to_ir_translator_set_error(translator, "Expected arguments in method call");
        IRValue null_value = {0};
        return null_value;
    }
    
    // Count arguments
    uint32_t arg_count = args_ast->child_count;
    
    // Create call instruction
    IRInstruction* call_instruction = ir_builder_create_instruction(translator->ir_builder, IR_CALL);
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
    
    if (ast->child_count > 0) {
        // Return with value
        IRValue value = ast_to_ir_translate_expression(translator, ast->children[0]);
        IRInstruction* instruction = ir_builder_create_instruction(translator->ir_builder, IR_RETURN_VAL);
        if (instruction) {
            ir_instruction_add_operand(instruction, value);
            ir_builder_add_instruction(translator->ir_builder, instruction);
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
    }
    
    // Translate then block
    ir_builder_set_current_block(translator->ir_builder, then_block);
    translator->current_block = then_block;
    ast_to_ir_translate_statement(translator, ast->children[1]);
    
    // Jump to merge block from then block
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
    (void)ast_to_ir_translate_expression(translator, ast->children[0]); // match_value
    
    // Create match blocks
    IRBlock* match_exit = ir_builder_create_block(translator->ir_builder, "match_exit");
    if (!match_exit) return;
    
    // For now, implement a simple match that just translates the expression
    // Full implementation would handle pattern matching with cases
    for (uint32_t i = 1; i < ast->child_count; i++) {
        Ast* case_ast = ast->children[i];
        if (case_ast->kind == AST_CASE) {
            // Create case block
            IRBlock* case_block = ir_builder_create_block(translator->ir_builder, "case");
            if (!case_block) continue;
            
            // For now, just translate the case body
            // Full implementation would handle pattern matching
            ir_builder_set_current_block(translator->ir_builder, case_block);
            translator->current_block = case_block;
            
            // Translate case body
            for (uint32_t j = 0; j < case_ast->child_count; j++) {
                ast_to_ir_translate_statement(translator, case_ast->children[j]);
            }
            
            // Jump to exit
            ir_builder_add_jump(translator->ir_builder, match_exit);
        }
    }
    
    // Set current block to exit
    ir_builder_set_current_block(translator->ir_builder, match_exit);
    translator->current_block = match_exit;
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
    
    // Determine literal type based on the token kind
    switch (ast->literal.token.kind) {
        case TK_INT:
            value.type = IR_VALUE_I64;
            value.data.i64 = ast->literal.int_value;
            break;
        case TK_FLOAT:
            value.type = IR_VALUE_F64;
            value.data.f64 = ast->literal.float_value;
            break;
        case TK_STRING:
            value.type = IR_VALUE_STRING;
            // Store the string value directly - it will be processed during IR to bytecode translation
            value.data.string_id = (uint32_t)(uintptr_t)ast->literal.string_value;
            break;
        case TK_TRUE:
            value.type = IR_VALUE_BOOL;
            value.data.boolean = true;
            break;
        case TK_FALSE:
            value.type = IR_VALUE_BOOL;
            value.data.boolean = false;
            break;
        case TK_NULL:
            value.type = IR_VALUE_NULL;
            break;
        default:
            // Default to integer for unknown types
            value.type = IR_VALUE_I64;
            value.data.i64 = ast->literal.int_value;
            break;
    }
    
    return value;
}

// These functions are not used in the current simplified implementation
// They would be needed for a full implementation that stores operator types in AST
