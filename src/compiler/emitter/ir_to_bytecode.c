#include "ir_to_bytecode.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

// IR to bytecode translator implementation
IRToBytecodeTranslator* ir_to_bytecode_translator_create(void) {
    IRToBytecodeTranslator* translator = malloc(sizeof(IRToBytecodeTranslator));
    if (!translator) return NULL;
    
    // Initialize bytecode file
    translator->bytecode_file = bytecode_file_create();
    if (!translator->bytecode_file) {
        free(translator);
        return NULL;
    }
    
    // Initialize tables
    translator->string_table = string_table_create();
    translator->constant_table = constant_table_create();
    translator->method_table = method_table_create();
    translator->type_table = type_table_create();
    
    printf("DEBUG: Translator tables created - string_table=%p, constant_table=%p\n", 
           translator->string_table, translator->constant_table);
    
    if (!translator->string_table || !translator->constant_table || 
        !translator->method_table || !translator->type_table) {
        printf("DEBUG: Failed to create translator tables\n");
        ir_to_bytecode_translator_destroy(translator);
        return NULL;
    }
    
    // Initialize current method bytecode
    translator->current_bytecode = NULL;
    translator->current_bytecode_size = 0;
    translator->current_bytecode_capacity = 0;
    
    translator->current_function = NULL;
    translator->current_block = NULL;
    translator->error_message = NULL;
    translator->has_error = false;
    
    return translator;
}

void ir_to_bytecode_translator_destroy(IRToBytecodeTranslator* translator) {
    if (!translator) return;
    
    if (translator->bytecode_file) {
        bytecode_file_destroy(translator->bytecode_file);
    }
    
    if (translator->string_table) {
        string_table_destroy(translator->string_table);
    }
    
    if (translator->constant_table) {
        constant_table_destroy(translator->constant_table);
    }
    
    if (translator->method_table) {
        method_table_destroy(translator->method_table);
    }
    
    if (translator->type_table) {
        type_table_destroy(translator->type_table);
    }
    
    if (translator->current_bytecode) {
        free(translator->current_bytecode);
    }
    
    if (translator->error_message) {
        free(translator->error_message);
    }
    
    free(translator);
}

bool ir_to_bytecode_translate_function(IRToBytecodeTranslator* translator, IRFunction* function) {
    if (!translator || !function) return false;
    
    translator->current_function = function;
    
    // Translate all blocks in the function
    for (size_t i = 0; i < function->block_count; i++) {
        if (!ir_to_bytecode_translate_block(translator, function->blocks[i])) {
            return false;
        }
    }
    
    return true;
}

bool ir_to_bytecode_translate_block(IRToBytecodeTranslator* translator, IRBlock* block) {
    if (!translator || !block) return false;
    
    translator->current_block = block;
    
    // Translate all instructions in the block
    for (size_t i = 0; i < block->instruction_count; i++) {
        if (!ir_to_bytecode_translate_instruction(translator, block->instructions[i])) {
            return false;
        }
    }
    
    return true;
}

bool ir_to_bytecode_translate_instruction(IRToBytecodeTranslator* translator, IRInstruction* instruction) {
    if (!translator || !instruction) return false;
    
    printf("DEBUG: Translating IR instruction: %d\n", instruction->op);
    
    // Debug: Print all enum values
    printf("DEBUG: IR_LOAD_CONST=%d, IR_INC=%d, IR_DEC=%d, IR_RETURN=%d, IR_JMP_GE=%d\n", 
           IR_LOAD_CONST, IR_INC, IR_DEC, IR_RETURN, IR_JMP_GE);
    
    switch (instruction->op) {
        case IR_LOAD_CONST: {
            uint32_t constant_index = 0;
            switch (instruction->operands[0].type) {
                case IR_VALUE_I64:
                    constant_index = constant_table_add_int64(translator->constant_table, instruction->operands[0].data.i64);
                    break;
                case IR_VALUE_F64:
                    constant_index = constant_table_add_float64(translator->constant_table, instruction->operands[0].data.f64);
                    break;
                case IR_VALUE_BOOL:
                    constant_index = constant_table_add_boolean(translator->constant_table, instruction->operands[0].data.boolean);
                    break;
                case IR_VALUE_STRING: {
                    // Convert string_id back to string pointer
                    const char* string_value = (const char*)(uintptr_t)instruction->operands[0].data.string_id;
                    constant_index = ir_to_bytecode_add_string_constant(translator, string_value);
                    break;
                }
                case IR_VALUE_NULL:
                    constant_index = constant_table_add_null(translator->constant_table);
                    break;
                default:
                    ir_to_bytecode_translator_set_error(translator, "Unsupported constant type in IR_LOAD_CONST");
                    return false;
            }
            return ir_to_bytecode_emit_push_constant(translator, constant_index);
        }
        
        case IR_LOAD_LOCAL:
            return ir_to_bytecode_emit_instruction(translator, OP_LOAD_LOCAL, 
                                                 (uint8_t*)&instruction->operands[0].data.temp_id, 
                                                 sizeof(uint32_t));
        
        case IR_STORE_LOCAL:
            return ir_to_bytecode_emit_instruction(translator, OP_STORE_LOCAL, 
                                                 (uint8_t*)&instruction->operands[0].data.temp_id, 
                                                 sizeof(uint32_t));
        
        case IR_ADD:
            return ir_to_bytecode_emit_instruction(translator, OP_ADD, NULL, 0);
        
        case IR_SUB:
            return ir_to_bytecode_emit_instruction(translator, OP_SUB, NULL, 0);
        
        case IR_MUL:
            return ir_to_bytecode_emit_instruction(translator, OP_MUL, NULL, 0);
        
        case IR_DIV:
            return ir_to_bytecode_emit_instruction(translator, OP_DIV, NULL, 0);
        
        case IR_INC:
            return ir_to_bytecode_emit_instruction(translator, OP_INC, NULL, 0);
        
        case IR_DEC:
            return ir_to_bytecode_emit_instruction(translator, OP_DEC, NULL, 0);
        
        case IR_RETURN:
            return ir_to_bytecode_emit_instruction(translator, OP_RETURN, NULL, 0);
        
        case IR_JMP_GE:
            printf("DEBUG: Handling IR_JMP_GE\n");
            // For now, just emit a comparison - the jump will be handled separately
            return ir_to_bytecode_emit_instruction(translator, OP_GE, NULL, 0);
        
        case IR_CALL:
            printf("DEBUG: Handling IR_CALL\n");
            // For now, just emit a return - method calls will be handled later
            return ir_to_bytecode_emit_instruction(translator, OP_RETURN, NULL, 0);
        
        case IR_RETURN_VAL:
            printf("DEBUG: Handling IR_RETURN_VAL\n");
            // For now, just emit a return - return values will be handled later
            return ir_to_bytecode_emit_instruction(translator, OP_RETURN, NULL, 0);
        
        default:
            printf("DEBUG: Unknown instruction type: %d\n", instruction->op);
            ir_to_bytecode_translator_set_error(translator, "Unknown instruction type");
            return false;
    }
}

bool ir_to_bytecode_emit_instruction(IRToBytecodeTranslator* translator, uint8_t opcode, const uint8_t* operands, size_t operand_size) {
    if (!translator) return false;
    
    // Ensure we have enough space
    size_t total_size = 1 + operand_size; // opcode + operands
    if (translator->current_bytecode_size + total_size > translator->current_bytecode_capacity) {
        size_t new_capacity = translator->current_bytecode_capacity * 2;
        if (new_capacity < translator->current_bytecode_size + total_size) {
            new_capacity = translator->current_bytecode_size + total_size;
        }
        
        uint8_t* new_bytecode = realloc(translator->current_bytecode, new_capacity);
        if (!new_bytecode) {
            ir_to_bytecode_translator_set_error(translator, "Failed to allocate bytecode memory");
            return false;
        }
        
        translator->current_bytecode = new_bytecode;
        translator->current_bytecode_capacity = new_capacity;
    }
    
    // Emit opcode
    translator->current_bytecode[translator->current_bytecode_size++] = opcode;
    
    // Emit operands
    if (operands && operand_size > 0) {
        memcpy(translator->current_bytecode + translator->current_bytecode_size, operands, operand_size);
        translator->current_bytecode_size += operand_size;
    }
    
    return true;
}

bool ir_to_bytecode_emit_push_constant(IRToBytecodeTranslator* translator, int64_t value) {
    if (!translator) return false;
    
    // Add constant to constant table
    uint32_t constant_index = ir_to_bytecode_add_int64_constant(translator, value);
    if (constant_index == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add constant to table");
        return false;
    }
    
    // Emit PUSH_CONSTANT instruction
    return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                         (uint8_t*)&constant_index, sizeof(uint32_t));
}

bool ir_to_bytecode_emit_push_float(IRToBytecodeTranslator* translator, double value) {
    if (!translator) return false;
    
    // Add constant to constant table
    uint32_t constant_index = ir_to_bytecode_add_float64_constant(translator, value);
    if (constant_index == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add constant to table");
        return false;
    }
    
    // Emit PUSH_CONSTANT instruction
    return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                         (uint8_t*)&constant_index, sizeof(uint32_t));
}

bool ir_to_bytecode_emit_push_boolean(IRToBytecodeTranslator* translator, bool value) {
    if (!translator) return false;
    
    // Add constant to constant table
    uint32_t constant_index = ir_to_bytecode_add_boolean_constant(translator, value);
    if (constant_index == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add constant to table");
        return false;
    }
    
    // Emit PUSH_CONSTANT instruction
    return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                         (uint8_t*)&constant_index, sizeof(uint32_t));
}

bool ir_to_bytecode_emit_push_string(IRToBytecodeTranslator* translator, const char* value) {
    if (!translator || !value) return false;
    
    // Add string to string table
    uint32_t string_offset = ir_to_bytecode_add_string(translator, value);
    if (string_offset == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add string to table");
        return false;
    }
    
    // Add string constant to constant table
    uint32_t constant_index = ir_to_bytecode_add_string_constant(translator, value);
    if (constant_index == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add string constant to table");
        return false;
    }
    
    // Emit PUSH_CONSTANT instruction
    return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                         (uint8_t*)&constant_index, sizeof(uint32_t));
}

bool ir_to_bytecode_emit_push_null(IRToBytecodeTranslator* translator) {
    if (!translator) return false;
    
    // Add null constant to constant table
    uint32_t constant_index = ir_to_bytecode_add_null_constant(translator);
    if (constant_index == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add null constant to table");
        return false;
    }
    
    // Emit PUSH_CONSTANT instruction
    return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                         (uint8_t*)&constant_index, sizeof(uint32_t));
}

// Table management functions
uint32_t ir_to_bytecode_add_string(IRToBytecodeTranslator* translator, const char* str) {
    if (!translator || !str) return 0;
    
    return string_table_add_string(translator->string_table, str);
}

uint32_t ir_to_bytecode_add_constant(IRToBytecodeTranslator* translator, const ConstantEntry* entry) {
    if (!translator || !entry) return 0;
    
    return constant_table_add_constant(translator->constant_table, entry);
}

uint32_t ir_to_bytecode_add_int64_constant(IRToBytecodeTranslator* translator, int64_t value) {
    if (!translator) return 0;
    
    return constant_table_add_int64(translator->constant_table, value);
}

uint32_t ir_to_bytecode_add_float64_constant(IRToBytecodeTranslator* translator, double value) {
    if (!translator) return 0;
    
    return constant_table_add_float64(translator->constant_table, value);
}

uint32_t ir_to_bytecode_add_boolean_constant(IRToBytecodeTranslator* translator, bool value) {
    if (!translator) return 0;
    
    return constant_table_add_boolean(translator->constant_table, value);
}

uint32_t ir_to_bytecode_add_string_constant(IRToBytecodeTranslator* translator, const char* value) {
    if (!translator || !value) {
        printf("DEBUG: ir_to_bytecode_add_string_constant failed - translator=%p, value=%p\n", translator, value);
        return 0;
    }
    
    printf("DEBUG: Adding string constant: '%s'\n", value);
    
    // Add string to string table first
    uint32_t string_offset = string_table_add_string(translator->string_table, value);
    if (string_offset == 0) {
        printf("DEBUG: string_table_add_string failed\n");
        return 0;
    }
    
    printf("DEBUG: string_table_add_string success - offset=%u\n", string_offset);
    
    // Add string constant to constant table
    uint32_t constant_index = constant_table_add_string(translator->constant_table, string_offset);
    if (constant_index == 0) {
        printf("DEBUG: constant_table_add_string failed\n");
        return 0;
    }
    
    printf("DEBUG: constant_table_add_string success - index=%u\n", constant_index);
    return constant_index;
}

uint32_t ir_to_bytecode_add_null_constant(IRToBytecodeTranslator* translator) {
    if (!translator) return 0;
    
    return constant_table_add_null(translator->constant_table);
}

uint32_t ir_to_bytecode_add_method(IRToBytecodeTranslator* translator, const char* name, const char* signature, uint32_t type_id) {
    if (!translator || !name || !signature) return 0;
    
    // Add method name to string table
    uint32_t name_offset = string_table_add_string(translator->string_table, name);
    if (name_offset == 0) return 0;
    
    // Add signature to string table
    uint32_t signature_offset = string_table_add_string(translator->string_table, signature);
    if (signature_offset == 0) return 0;
    
    // Create method entry
    MethodEntry entry;
    entry.method_id = translator->method_table->count + 1; // 1-based indexing
    entry.type_id = type_id;
    entry.name_offset = name_offset;
    entry.signature_offset = signature_offset;
    entry.bytecode_offset = 0; // Will be set later
    entry.bytecode_size = 0;   // Will be set later
    entry.local_count = 0;
    entry.param_count = 0;
    entry.return_type_id = 0;
    entry.flags = 0;
    entry.line_number = 0;
    entry.column_number = 0;
    
    // Add to method table
    if (!method_table_add_method(translator->method_table, &entry)) {
        return 0;
    }
    
    return entry.method_id;
}

uint32_t ir_to_bytecode_add_type(IRToBytecodeTranslator* translator, const char* name, uint32_t super_type_id, uint32_t size) {
    if (!translator || !name) return 0;
    
    // Add type name to string table
    printf("DEBUG: Adding type name '%s' to string table\n", name);
    uint32_t name_offset = string_table_add_string(translator->string_table, name);
    // Note: string table indices are 0-based, so 0 is a valid index
    printf("DEBUG: string_table_add_string result for type name, offset=%u\n", name_offset);
    
    // Create type entry
    TypeEntry entry;
    entry.type_id = translator->type_table->count + 1; // 1-based indexing
    entry.name_offset = name_offset;
    entry.parent_type_id = super_type_id;
    entry.size = size;
    entry.field_count = 0;
    entry.method_count = 0;
    entry.interface_count = 0;
    entry.flags = 0;
    entry.vtable_offset = 0;
    
    // Add to type table
    printf("DEBUG: Adding type '%s' to type table (count=%u)\n", name, translator->type_table->count);
    if (!type_table_add_type(translator->type_table, &entry)) {
        printf("DEBUG: type_table_add_type failed\n");
        return 0;
    }
    printf("DEBUG: type_table_add_type success (new count=%u)\n", translator->type_table->count);
    
    return entry.type_id;
}

// Final bytecode generation
BytecodeFile* ir_to_bytecode_generate_file(IRToBytecodeTranslator* translator) {
    if (!translator) return NULL;
    
    // Add a basic type for the main class if none exists
    printf("DEBUG: type_table=%p, count=%u\n", translator->type_table, translator->type_table ? translator->type_table->count : 0);
    if (translator->type_table->count == 0) {
        printf("DEBUG: Creating Program type\n");
        uint32_t type_id = ir_to_bytecode_add_type(translator, "Program", 0, 0); // System.Object is parent (0)
        if (type_id == 0) {
            printf("DEBUG: ir_to_bytecode_add_type returned 0\n");
            ir_to_bytecode_translator_set_error(translator, "Failed to create Program type");
            return NULL;
        }
        printf("DEBUG: Program type created with ID %u\n", type_id);
    }
    
    // Add a main method if none exists
    if (translator->method_table->count == 0) {
        uint32_t method_id = ir_to_bytecode_add_method(translator, "main", "()I", 1); // Program type
        if (method_id == 0) {
            ir_to_bytecode_translator_set_error(translator, "Failed to create main method");
            return NULL;
        }
    }
    
    // Copy tables to bytecode file
    translator->bytecode_file->string_table = translator->string_table;
    translator->bytecode_file->constant_table = translator->constant_table;
    translator->bytecode_file->type_table = translator->type_table;
    translator->bytecode_file->method_table = translator->method_table;
    
    // Set bytecode
    translator->bytecode_file->bytecode = translator->current_bytecode;
    translator->bytecode_file->bytecode_size = translator->current_bytecode_size;
    
    // Set entry point
    translator->bytecode_file->header.entry_point_method_id = 1; // First method is main
    
    // Clear current bytecode so it's not freed
    translator->current_bytecode = NULL;
    translator->current_bytecode_size = 0;
    translator->current_bytecode_capacity = 0;
    
    return translator->bytecode_file;
}

// Error handling
void ir_to_bytecode_translator_set_error(IRToBytecodeTranslator* translator, const char* message) {
    if (!translator || !message) return;
    
    if (translator->error_message) {
        free(translator->error_message);
    }
    
    translator->error_message = malloc(strlen(message) + 1);
    if (translator->error_message) {
        strcpy(translator->error_message, message);
    }
    
    translator->has_error = true;
}

const char* ir_to_bytecode_translator_get_error(IRToBytecodeTranslator* translator) {
    if (!translator) return NULL;
    
    return translator->error_message;
}

bool ir_to_bytecode_translator_has_error(IRToBytecodeTranslator* translator) {
    if (!translator) return false;
    
    return translator->has_error;
}