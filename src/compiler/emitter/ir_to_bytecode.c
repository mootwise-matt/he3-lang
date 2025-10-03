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
    
    
    if (!translator->string_table || !translator->constant_table || 
        !translator->method_table || !translator->type_table) {
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
    
    
    // Debug: Print all enum values
    
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
                    // Use string_value directly
                    const char* string_value = instruction->operands[0].data.string_value;
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
            return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                                 (uint8_t*)&constant_index, sizeof(uint32_t));
        }
        
        case IR_LOAD_STATIC:
            // For built-in functions, we'll push a special built-in function reference
            // This will be handled by the VM when the function is called
            uint32_t builtin_id = 0; // Built-in function ID (0 = print)
            return ir_to_bytecode_emit_instruction(translator, OP_PUSH_CONSTANT, 
                                                 (uint8_t*)&builtin_id, sizeof(uint32_t));
        
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
            // For now, just emit a comparison - the jump will be handled separately
            return ir_to_bytecode_emit_instruction(translator, OP_GE, NULL, 0);
        
        case IR_CALL: {
            // IR_CALL has: callee, argument count, and arguments
            if (instruction->operand_count < 2) {
                ir_to_bytecode_translator_set_error(translator, "IR_CALL requires callee and argument count");
                return false;
            }
            
            // Get argument count from second operand
            uint32_t arg_count = (uint32_t)instruction->operands[1].data.i64;
            
            // Check if the callee is a built-in function
            // For built-in functions, we'll use method_id 0
            // For regular functions, we'd look up the actual method ID
            uint32_t method_id = 0; // Built-in function ID
            
            // Emit CALL instruction with method ID
            return ir_to_bytecode_emit_instruction(translator, OP_CALL, 
                                                 (uint8_t*)&method_id, sizeof(uint32_t));
        }
        
        case IR_CALL_STATIC: {
            // IR_CALL_STATIC has: callee, argument count, and arguments
            if (instruction->operand_count < 2) {
                ir_to_bytecode_translator_set_error(translator, "IR_CALL_STATIC requires callee and argument count");
                return false;
            }
            
            // Get argument count from second operand
            uint32_t arg_count = (uint32_t)instruction->operands[1].data.i64;
            
            // For static method calls, we need to look up the method ID
            uint32_t method_id = 0; // Default to built-in function ID
            
            // For static calls, the method ID is already stored in the callee
            if (instruction->operands[0].type == IR_VALUE_I64) {
                method_id = (uint32_t)instruction->operands[0].data.i64;
            }
            
            // Emit CALL_STATIC instruction with method ID
            return ir_to_bytecode_emit_instruction(translator, OP_CALL_STATIC, 
                                                 (uint8_t*)&method_id, sizeof(uint32_t));
        }
        
        case IR_RETURN_VAL:
            // Push the return value onto the stack first
            if (instruction->operand_count > 0) {
                // The return value is already on the stack from the expression evaluation
                // Just emit the return instruction
                return ir_to_bytecode_emit_instruction(translator, OP_RETURN, NULL, 0);
            } else {
                // No return value, just return
                return ir_to_bytecode_emit_instruction(translator, OP_RETURN, NULL, 0);
            }
        
        case IR_NEW: {
            // IR_NEW has: class name (string), argument count (i64)
            if (instruction->operand_count < 2) {
                ir_to_bytecode_translator_set_error(translator, "IR_NEW requires class name and argument count");
                return false;
            }
            
            // Get class name from first operand
            const char* class_name = instruction->operands[0].data.string_value;
            uint32_t class_name_offset = ir_to_bytecode_add_string_constant(translator, class_name);
            if (class_name_offset == 0) {
                ir_to_bytecode_translator_set_error(translator, "Failed to add class name to string table");
                return false;
            }
            
            // Get argument count from second operand
            uint32_t arg_count = (uint32_t)instruction->operands[1].data.i64;
            
            // For now, we'll use a placeholder type ID (0 = System.Object)
            // In a full implementation, we'd look up the actual type ID
            uint32_t type_id = 0;
            
            // Emit NEW_OBJECT instruction with type ID
            return ir_to_bytecode_emit_instruction(translator, OP_NEW_OBJECT, 
                                                 (uint8_t*)&type_id, sizeof(uint32_t));
        }
        
        case IR_LOAD_FIELD: {
            // IR_LOAD_FIELD has: object, field
            if (instruction->operand_count < 2) {
                ir_to_bytecode_translator_set_error(translator, "IR_LOAD_FIELD requires object and field");
                return false;
            }
            
            // For now, we'll use a placeholder field ID (0)
            // In a full implementation, we'd look up the actual field ID
            uint32_t field_id = 0;
            
            // Emit LOAD_FIELD instruction with field ID
            return ir_to_bytecode_emit_instruction(translator, OP_LOAD_FIELD, 
                                                 (uint8_t*)&field_id, sizeof(uint32_t));
        }
        
        default:
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
        return 0;
    }
    
    
    // Add string to string table first
    uint32_t string_offset = string_table_add_string(translator->string_table, value);
    if (string_offset == 0) {
        return 0;
    }
    
    
    // Add string constant to constant table
    uint32_t constant_index = constant_table_add_string(translator->constant_table, string_offset);
    if (constant_index == 0) {
        return 0;
    }
    
    return constant_index;
}

uint32_t ir_to_bytecode_add_null_constant(IRToBytecodeTranslator* translator) {
    if (!translator) return 0;
    
    return constant_table_add_null(translator->constant_table);
}

uint32_t ir_to_bytecode_add_method(IRToBytecodeTranslator* translator, const char* name, const char* signature, uint32_t type_id) {
    if (!translator || !name || !signature) return 0;
    
    // Add method name to string table
    uint32_t name_index = string_table_add_string(translator->string_table, name);
    // Note: 0 is a valid string table index, so we don't check for 0 here
    
    // Add signature to string table
    uint32_t signature_index = string_table_add_string(translator->string_table, signature);
    // Note: 0 is a valid string table index, so we don't check for 0 here
    
    // Convert indices to offsets
    uint32_t name_offset = translator->string_table->entries[name_index].offset;
    uint32_t signature_offset = translator->string_table->entries[signature_index].offset;
    
    // Get local count from current function if available
    uint32_t local_count = 0;
    if (translator->current_function) {
        local_count = translator->current_function->local_count;
    } else {
    }
    
    // Create method entry
    MethodEntry entry;
    entry.method_id = translator->method_table->count + 1; // 1-based indexing
    entry.type_id = type_id;
    entry.name_offset = name_offset;
    entry.signature_offset = signature_offset;
    entry.bytecode_offset = 0; // Will be set later
    entry.bytecode_size = 0;   // Will be set later
    entry.local_count = local_count;
    entry.param_count = 0;
    entry.return_type_id = 0;
    
    // Set flags based on current function properties
    entry.flags = 0;
    if (translator->current_function && translator->current_function->is_static) {
        entry.flags |= METHOD_FLAG_STATIC;
    }
    if (translator->current_function && translator->current_function->is_virtual) {
        entry.flags |= METHOD_FLAG_VIRTUAL;
    }
    if (translator->current_function && translator->current_function->is_async) {
        entry.flags |= METHOD_FLAG_ASYNC;
    }
    
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
    uint32_t name_index = string_table_add_string(translator->string_table, name);
    // Note: string table indices are 0-based, so 0 is a valid index
    
    // Convert index to offset
    uint32_t name_offset = translator->string_table->entries[name_index].offset;
    
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
    if (!type_table_add_type(translator->type_table, &entry)) {
        return 0;
    }
    
    return entry.type_id;
}

// Final bytecode generation
BytecodeFile* ir_to_bytecode_generate_file(IRToBytecodeTranslator* translator) {
    if (!translator) return NULL;
    
    // Add a basic type for the main class if none exists
    if (translator->type_table->count == 0) {
        uint32_t type_id = ir_to_bytecode_add_type(translator, "Program", 0, 0); // System.Object is parent (0)
        if (type_id == 0) {
            ir_to_bytecode_translator_set_error(translator, "Failed to create Program type");
            return NULL;
        }
    }
    
    // Method should already be added before function translation
    
    // Copy tables to bytecode file
    translator->bytecode_file->string_table = translator->string_table;
    translator->bytecode_file->constant_table = translator->constant_table;
    translator->bytecode_file->type_table = translator->type_table;
    translator->bytecode_file->method_table = translator->method_table;
    
    // Set bytecode
    fflush(stdout);
    
    translator->bytecode_file->bytecode = translator->current_bytecode;
    translator->bytecode_file->bytecode_size = translator->current_bytecode_size;
    
    fflush(stdout);
    
    // Update method table entries with bytecode information
    if (translator->method_table && translator->method_table->count > 0) {
        translator->method_table->entries[0].bytecode_offset = 0; // First method starts at beginning
        translator->method_table->entries[0].bytecode_size = translator->current_bytecode_size;
    }
    
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