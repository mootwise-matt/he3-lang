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
    translator->method_table = method_table_create();
    translator->type_table = type_table_create();
    
    if (!translator->string_table || !translator->method_table || !translator->type_table) {
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

// Bytecode emission functions
bool ir_to_bytecode_emit_opcode(IRToBytecodeTranslator* translator, uint8_t opcode) {
    if (!translator) return false;
    
    // Ensure capacity
    if (translator->current_bytecode_size + 1 > translator->current_bytecode_capacity) {
        size_t new_capacity = translator->current_bytecode_capacity * 2;
        if (new_capacity < 1024) new_capacity = 1024;
        
        uint8_t* new_bytecode = realloc(translator->current_bytecode, new_capacity);
        if (!new_bytecode) {
            ir_to_bytecode_translator_set_error(translator, "Failed to allocate bytecode buffer");
            return false;
        }
        
        translator->current_bytecode = new_bytecode;
        translator->current_bytecode_capacity = new_capacity;
    }
    
    translator->current_bytecode[translator->current_bytecode_size++] = opcode;
    return true;
}

bool ir_to_bytecode_emit_operand(IRToBytecodeTranslator* translator, uint32_t operand) {
    if (!translator) return false;
    
    // Ensure capacity for 4-byte operand
    if (translator->current_bytecode_size + 4 > translator->current_bytecode_capacity) {
        size_t new_capacity = translator->current_bytecode_capacity * 2;
        if (new_capacity < 1024) new_capacity = 1024;
        
        uint8_t* new_bytecode = realloc(translator->current_bytecode, new_capacity);
        if (!new_bytecode) {
            ir_to_bytecode_translator_set_error(translator, "Failed to allocate bytecode buffer");
            return false;
        }
        
        translator->current_bytecode = new_bytecode;
        translator->current_bytecode_capacity = new_capacity;
    }
    
    // Emit operand as little-endian
    translator->current_bytecode[translator->current_bytecode_size++] = (operand >> 0) & 0xFF;
    translator->current_bytecode[translator->current_bytecode_size++] = (operand >> 8) & 0xFF;
    translator->current_bytecode[translator->current_bytecode_size++] = (operand >> 16) & 0xFF;
    translator->current_bytecode[translator->current_bytecode_size++] = (operand >> 24) & 0xFF;
    
    return true;
}

bool ir_to_bytecode_emit_instruction(IRToBytecodeTranslator* translator, uint8_t opcode, uint32_t* operands, uint8_t operand_count) {
    if (!translator) return false;
    
    // Emit opcode
    if (!ir_to_bytecode_emit_opcode(translator, opcode)) {
        return false;
    }
    
    // Emit operands
    for (uint8_t i = 0; i < operand_count; i++) {
        if (!ir_to_bytecode_emit_operand(translator, operands[i])) {
            return false;
        }
    }
    
    return true;
}

// String and method management
uint32_t ir_to_bytecode_add_string(IRToBytecodeTranslator* translator, const char* str) {
    if (!translator || !str) return 0;
    
    return string_table_add_string(translator->string_table, str);
}

uint32_t ir_to_bytecode_add_method(IRToBytecodeTranslator* translator, const char* name, const char* signature, uint32_t type_id) {
    if (!translator || !name || !signature) return 0;
    
    // Add strings to string table
    uint32_t name_offset = string_table_add_string(translator->string_table, name);
    uint32_t signature_offset = string_table_add_string(translator->string_table, signature);
    
    if (name_offset == 0 || signature_offset == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add method strings");
        return 0;
    }
    
    // Create method entry
    MethodEntry* entry = malloc(sizeof(MethodEntry));
    if (!entry) {
        ir_to_bytecode_translator_set_error(translator, "Failed to allocate method entry");
        return 0;
    }
    
    entry->method_id = translator->method_table->count + 1;
    entry->name_offset = name_offset;
    entry->signature_offset = signature_offset;
    entry->type_id = type_id;
    entry->bytecode_offset = 0; // Will be set when method is finalized
    entry->bytecode_size = 0;   // Will be set when method is finalized
    entry->local_count = 0;     // Will be set from IR function
    entry->param_count = 0;     // Will be set from IR function
    entry->flags = 0;           // Will be set based on method properties
    
    // Add to method table
    translator->method_table->entries = realloc(translator->method_table->entries, 
                                               sizeof(MethodEntry) * (translator->method_table->count + 1));
    if (!translator->method_table->entries) {
        free(entry);
        ir_to_bytecode_translator_set_error(translator, "Failed to allocate method table");
        return 0;
    }
    
    translator->method_table->entries[translator->method_table->count] = *entry;
    translator->method_table->count++;
    
    free(entry);
    return entry->method_id;
}

uint32_t ir_to_bytecode_add_type(IRToBytecodeTranslator* translator, const char* name, uint32_t super_type_id, uint32_t size) {
    if (!translator || !name) return 0;
    
    // Add name to string table
    uint32_t name_offset = string_table_add_string(translator->string_table, name);
    if (name_offset == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add type name");
        return 0;
    }
    
    // Create type entry
    TypeEntry* entry = malloc(sizeof(TypeEntry));
    if (!entry) {
        ir_to_bytecode_translator_set_error(translator, "Failed to allocate type entry");
        return 0;
    }
    
    entry->type_id = translator->type_table->count + 1;
    entry->name_offset = name_offset;
    entry->parent_type_id = super_type_id;
    entry->size = size;
    entry->field_count = 0;     // Will be set when fields are added
    entry->method_count = 0;    // Will be set when methods are added
    entry->interface_count = 0; // Will be set when interfaces are added
    entry->vtable_offset = 0;   // Will be set when vtable is created
    entry->flags = 0;           // Will be set based on type properties
    
    // Add to type table
    translator->type_table->entries = realloc(translator->type_table->entries, 
                                             sizeof(TypeEntry) * (translator->type_table->count + 1));
    if (!translator->type_table->entries) {
        free(entry);
        ir_to_bytecode_translator_set_error(translator, "Failed to allocate type table");
        return 0;
    }
    
    translator->type_table->entries[translator->type_table->count] = *entry;
    translator->type_table->count++;
    
    free(entry);
    return entry->type_id;
}

// Translation functions
bool ir_to_bytecode_translate_function(IRToBytecodeTranslator* translator, IRFunction* function) {
    if (!translator || !function) {
        ir_to_bytecode_translator_set_error(translator, "Invalid parameters");
        return false;
    }
    
    printf("Translating function: %s (blocks: %u)\n", function->name, function->block_count);
    translator->current_function = function;
    
    // Reset current method bytecode
    translator->current_bytecode_size = 0;
    
    // Add the function as a method
    uint32_t method_id = ir_to_bytecode_add_method(translator, function->name, "()I", 1); // Assuming type_id 1 for main class
    if (method_id == 0) {
        ir_to_bytecode_translator_set_error(translator, "Failed to add method to bytecode");
        return false;
    }
    printf("Added method %s with ID %u\n", function->name, method_id);
    
    // Translate all blocks in the function
    for (uint32_t i = 0; i < function->block_count; i++) {
        printf("Translating block %u (instructions: %u)\n", i, function->blocks[i]->instruction_count);
        if (!ir_to_bytecode_translate_block(translator, function->blocks[i])) {
            return false;
        }
    }
    
    // Update the method's bytecode information
    if (translator->method_table->count > 0) {
        MethodEntry* method = &translator->method_table->entries[translator->method_table->count - 1];
        method->bytecode_offset = 0; // Bytecode starts at offset 0 in the file
        method->bytecode_size = translator->current_bytecode_size;
        printf("Updated method %s: bytecode_offset=%u, bytecode_size=%zu\n", 
               function->name, method->bytecode_offset, method->bytecode_size);
    }
    
    return true;
}

bool ir_to_bytecode_translate_block(IRToBytecodeTranslator* translator, IRBlock* block) {
    if (!translator || !block) {
        ir_to_bytecode_translator_set_error(translator, "Invalid parameters");
        return false;
    }
    
    translator->current_block = block;
    
    // Translate all instructions in the block
    printf("  Block has %u instructions\n", block->instruction_count);
    for (uint32_t i = 0; i < block->instruction_count; i++) {
        printf("  Instruction %u: op=%d\n", i, block->instructions[i]->op);
        if (!ir_to_bytecode_translate_instruction(translator, block->instructions[i])) {
            return false;
        }
    }
    
    return true;
}

bool ir_to_bytecode_translate_instruction(IRToBytecodeTranslator* translator, IRInstruction* instruction) {
    if (!translator || !instruction) {
        ir_to_bytecode_translator_set_error(translator, "Invalid parameters");
        return false;
    }

    // Debug output
    printf("  Translating IR instruction: op=%d, operand_count=%u\n", instruction->op, instruction->operand_count);

    // Get the corresponding opcode
    uint8_t opcode = ir_to_bytecode_get_opcode(instruction->op);
    if (opcode == 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Unknown IR operation: %d", instruction->op);
        ir_to_bytecode_translator_set_error(translator, error_msg);
        return false;
    }
    
    printf("  Mapped to bytecode opcode: %d\n", opcode);
    
    // Get the expected operand byte count for this opcode
    uint8_t expected_operand_bytes = opcode_get_operand_count(opcode);
    printf("  Expected operand bytes: %d\n", expected_operand_bytes);
    
    // Translate operands - only emit the operands we actually have
    uint32_t operands[4] = {0};
    uint8_t operand_count = 0;
    
    // Translate available operands
    for (uint32_t i = 0; i < instruction->operand_count && i < 4; i++) {
        if (!ir_to_bytecode_translate_value(translator, instruction->operands[i], &operands[i])) {
            return false;
        }
        operand_count++;
    }
    
    // For opcodes that need more operands than we have, pad with zeros
    // expected_operand_bytes is the total number of bytes, so divide by 4 to get operand count
    uint8_t expected_operand_count = expected_operand_bytes / 4;
    
    // Only emit operands if the opcode expects them
    if (expected_operand_count > 0) {
        while (operand_count < expected_operand_count) {
            operands[operand_count] = 0;
            operand_count++;
        }
    } else {
        // Opcode expects 0 operands, so don't emit any
        operand_count = 0;
    }
    
    // Emit instruction
    return ir_to_bytecode_emit_instruction(translator, opcode, operands, operand_count);
}

bool ir_to_bytecode_translate_value(IRToBytecodeTranslator* translator, IRValue value, uint32_t* operand) {
    if (!translator || !operand) {
        ir_to_bytecode_translator_set_error(translator, "Invalid parameters");
        return false;
    }
    
    switch (value.type) {
        case IR_VALUE_I64:
            *operand = (uint32_t)value.data.i64;
            break;
        case IR_VALUE_F64:
            *operand = (uint32_t)value.data.f64;
            break;
        case IR_VALUE_BOOL:
            *operand = value.data.boolean ? 1 : 0;
            break;
        case IR_VALUE_STRING:
            *operand = value.data.string_id;
            break;
        case IR_VALUE_OBJECT:
            *operand = (uint32_t)(uintptr_t)value.data.object;
            break;
        case IR_VALUE_NULL:
            *operand = 0;
            break;
        case IR_VALUE_TEMP:
            *operand = value.data.temp_id;
            break;
        default:
            ir_to_bytecode_translator_set_error(translator, "Unsupported value type");
            return false;
    }
    
    return true;
}

// Final bytecode generation
BytecodeFile* ir_to_bytecode_generate_file(IRToBytecodeTranslator* translator) {
    if (!translator) return NULL;
    
    // Add a basic type for the main class if none exists
    if (translator->type_table->count == 0) {
        uint32_t type_id = ir_to_bytecode_add_type(translator, "Program", 0, 0); // System.Object is parent (0)
        if (type_id == 0) {
            ir_to_bytecode_translator_set_error(translator, "Failed to add main type");
            return NULL;
        }
        printf("Added type Program with ID %u\n", type_id);
    }
    
    printf("Final bytecode file: %u types, %u methods\n", translator->type_table->count, translator->method_table->count);
    
    // Set up bytecode file with our tables
    translator->bytecode_file->string_table = translator->string_table;
    translator->bytecode_file->type_table = translator->type_table;
    translator->bytecode_file->method_table = translator->method_table;
    
    // Set up header
    translator->bytecode_file->header.flags = BYTECODE_FLAG_EXECUTABLE;
    translator->bytecode_file->header.entry_point_method_id = 1; // First method is entry point
    
    // Set table sizes in header
    translator->bytecode_file->header.string_table_size = translator->string_table ? 
        (sizeof(uint32_t) * 2 + sizeof(StringEntry) * translator->string_table->count + translator->string_table->total_size) : 0;
    translator->bytecode_file->header.type_table_size = translator->type_table ? 
        (sizeof(uint32_t) + sizeof(TypeEntry) * translator->type_table->count) : 0;
    translator->bytecode_file->header.method_table_size = translator->method_table ? 
        (sizeof(uint32_t) + sizeof(MethodEntry) * translator->method_table->count) : 0;
    translator->bytecode_file->header.field_table_size = 0; // Not implemented yet
    translator->bytecode_file->header.bytecode_size = translator->current_bytecode_size;
    
    // Set up bytecode
    if (translator->current_bytecode_size > 0) {
        translator->bytecode_file->bytecode = malloc(translator->current_bytecode_size);
        if (!translator->bytecode_file->bytecode) {
            ir_to_bytecode_translator_set_error(translator, "Failed to allocate bytecode buffer");
            return NULL;
        }
        
        memcpy(translator->bytecode_file->bytecode, translator->current_bytecode, translator->current_bytecode_size);
        translator->bytecode_file->bytecode_size = translator->current_bytecode_size;
    }
    
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

// Utility functions
uint8_t ir_to_bytecode_get_opcode(IROp ir_op) {
    switch (ir_op) {
        case IR_LOAD_CONST: return OP_PUSH_CONSTANT;
        case IR_LOAD_LOCAL: return OP_LOAD_LOCAL;
        case IR_LOAD_ARG: return OP_LOAD_ARG;
        case IR_LOAD_FIELD: return OP_LOAD_FIELD;
        case IR_LOAD_STATIC: return OP_LOAD_STATIC_FIELD;
        case IR_LOAD_ARRAY: return OP_LOAD_ARRAY;
        
        case IR_STORE_LOCAL: return OP_STORE_LOCAL;
        case IR_STORE_FIELD: return OP_STORE_FIELD;
        case IR_STORE_STATIC: return OP_STORE_STATIC_FIELD;
        case IR_STORE_ARRAY: return OP_STORE_ARRAY;
        
        case IR_ADD: return OP_ADD;
        case IR_SUB: return OP_SUB;
        case IR_MUL: return OP_MUL;
        case IR_DIV: return OP_DIV;
        case IR_MOD: return OP_MOD;
        case IR_NEG: return OP_NEG;
        case IR_INC: return OP_INC;
        case IR_DEC: return OP_DEC;
        
        case IR_EQ: return OP_EQ;
        case IR_NE: return OP_NE;
        case IR_LT: return OP_LT;
        case IR_LE: return OP_LE;
        case IR_GT: return OP_GT;
        case IR_GE: return OP_GE;
        
        case IR_AND: return OP_AND;
        case IR_OR: return OP_OR;
        case IR_NOT: return OP_NOT;
        
        case IR_JMP: return OP_JUMP;
        case IR_JMPT: return OP_JUMP_IF_TRUE;
        case IR_JMPF: return OP_JUMP_IF_FALSE;
        case IR_CALL: return OP_CALL;
        case IR_RETURN: return OP_RETURN;
        case IR_RETURN_VAL: return OP_RETURN_VALUE;
        
        case IR_NEW: return OP_NEW_OBJECT;
        case IR_NEW_ARRAY: return OP_NEW_ARRAY;
        
        case IR_INSTANCEOF: return OP_IS_INSTANCE_OF;
        case IR_CAST: return OP_CAST;
        
        default: return 0; // Unknown opcode
    }
}

bool ir_to_bytecode_is_optimizable_local(uint32_t local_index) {
    return local_index < 4;
}

uint8_t ir_to_bytecode_get_optimized_load_local(uint32_t local_index) {
    switch (local_index) {
        case 0: return OP_LOAD_LOCAL; // Could be optimized to OP_LOAD_LOCAL_0 if we had it
        case 1: return OP_LOAD_LOCAL;
        case 2: return OP_LOAD_LOCAL;
        case 3: return OP_LOAD_LOCAL;
        default: return OP_LOAD_LOCAL;
    }
}

uint8_t ir_to_bytecode_get_optimized_store_local(uint32_t local_index) {
    switch (local_index) {
        case 0: return OP_STORE_LOCAL; // Could be optimized to OP_STORE_LOCAL_0 if we had it
        case 1: return OP_STORE_LOCAL;
        case 2: return OP_STORE_LOCAL;
        case 3: return OP_STORE_LOCAL;
        default: return OP_STORE_LOCAL;
    }
}