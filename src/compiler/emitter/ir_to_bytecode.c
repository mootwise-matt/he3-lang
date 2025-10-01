#include "ir_to_bytecode.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// IR to bytecode translator implementation
IRToBytecodeTranslator* ir_to_bytecode_translator_create(BytecodeWriter* writer) {
    if (!writer) return NULL;
    
    IRToBytecodeTranslator* translator = malloc(sizeof(IRToBytecodeTranslator));
    if (!translator) return NULL;
    
    translator->writer = writer;
    translator->current_function = NULL;
    translator->current_block = NULL;
    translator->error_message = NULL;
    translator->has_error = false;
    
    return translator;
}

void ir_to_bytecode_translator_destroy(IRToBytecodeTranslator* translator) {
    if (!translator) return;
    
    if (translator->error_message) {
        free(translator->error_message);
    }
    
    free(translator);
}

// Translation functions
bool ir_to_bytecode_translate_function(IRToBytecodeTranslator* translator, IRFunction* function) {
    if (!translator || !function) {
        ir_to_bytecode_translator_set_error(translator, "Invalid parameters");
        return false;
    }
    
    translator->current_function = function;
    
    // Translate all blocks in the function
    for (uint32_t i = 0; i < function->block_count; i++) {
        if (!ir_to_bytecode_translate_block(translator, function->blocks[i])) {
            return false;
        }
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
    for (uint32_t i = 0; i < block->instruction_count; i++) {
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
    
    OpCode opcode = ir_to_bytecode_get_opcode(instruction->op);
    if (opcode == 0) {
        ir_to_bytecode_translator_set_error(translator, "Unknown IR operation");
        return false;
    }
    
    // Prepare operands
    uint32_t* operands = NULL;
    uint16_t operand_count = 0;
    
    if (instruction->operand_count > 0) {
        operands = malloc(sizeof(uint32_t) * instruction->operand_count);
        if (!operands) {
            ir_to_bytecode_translator_set_error(translator, "Memory allocation failed");
            return false;
        }
        
        for (uint32_t i = 0; i < instruction->operand_count; i++) {
            if (!ir_to_bytecode_translate_value(translator, instruction->operands[i], &operands[i])) {
                free(operands);
                return false;
            }
        }
        
        operand_count = instruction->operand_count;
    }
    
    // Emit instruction
    bool success = bytecode_writer_emit_instruction(translator->writer, opcode, operands, operand_count);
    
    if (operands) {
        free(operands);
    }
    
    if (!success) {
        ir_to_bytecode_translator_set_error(translator, "Failed to emit instruction");
        return false;
    }
    
    return true;
}

// Value translation
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
            // For now, just use the lower 32 bits
            *operand = (uint32_t)value.data.f64;
            break;
        case IR_VALUE_BOOL:
            *operand = value.data.boolean ? 1 : 0;
            break;
        case IR_VALUE_OBJECT:
            *operand = (uint32_t)(uintptr_t)value.data.object;
            break;
        case IR_VALUE_STRING:
            *operand = value.data.string_id;
            break;
        case IR_VALUE_NULL:
            *operand = 0;
            break;
        case IR_VALUE_TEMP:
            *operand = value.data.temp_id;
            break;
        default:
            ir_to_bytecode_translator_set_error(translator, "Unknown value type");
            return false;
    }
    
    return true;
}

// Error handling
void ir_to_bytecode_translator_set_error(IRToBytecodeTranslator* translator, const char* message) {
    if (!translator) return;
    
    if (translator->error_message) {
        free(translator->error_message);
    }
    
    translator->error_message = message ? strdup(message) : NULL;
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
OpCode ir_to_bytecode_get_opcode(IROp ir_op) {
    switch (ir_op) {
        case IR_LOAD_CONST: return OP_PUSH_I64; // Will be specialized based on value type
        case IR_LOAD_LOCAL: return OP_LOAD_LOCAL;
        case IR_LOAD_ARG: return OP_LOAD_ARG;
        case IR_LOAD_FIELD: return OP_LOAD_FIELD;
        case IR_LOAD_STATIC: return OP_LOAD_STATIC;
        case IR_LOAD_ARRAY: return OP_LOAD_ARRAY;
        case IR_STORE_LOCAL: return OP_STORE_LOCAL;
        case IR_STORE_FIELD: return OP_STORE_FIELD;
        case IR_STORE_STATIC: return OP_STORE_STATIC;
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
        case IR_JMP: return OP_JMP;
        case IR_JMPF: return OP_JMPF;
        case IR_JMPT: return OP_JMPT;
        case IR_JMP_EQ: return OP_JMP_EQ;
        case IR_JMP_NE: return OP_JMP_NE;
        case IR_JMP_LT: return OP_JMP_LT;
        case IR_JMP_LE: return OP_JMP_LE;
        case IR_JMP_GT: return OP_JMP_GT;
        case IR_JMP_GE: return OP_JMP_GE;
        case IR_CALL: return OP_CALL;
        case IR_CALLV: return OP_CALLV;
        case IR_CALLI: return OP_CALLI;
        case IR_RETURN: return OP_RETURN;
        case IR_RETURN_VAL: return OP_RETURN_VAL;
        case IR_NEW: return OP_NEW;
        case IR_NEW_ARRAY: return OP_NEW_ARRAY;
        case IR_CAST: return OP_CAST;
        case IR_INSTANCEOF: return OP_INSTANCEOF;
        case IR_IS_NULL: return OP_IS_NULL;
        case IR_IS_NOT_NULL: return OP_IS_NOT_NULL;
        case IR_BOX: return OP_BOX;
        case IR_UNBOX: return OP_UNBOX;
        case IR_GET_TYPE: return OP_GET_TYPE;
        case IR_SPAWN: return OP_SPAWN;
        case IR_AWAIT: return OP_AWAIT;
        case IR_ASYNC_RETURN: return OP_ASYNC_RETURN;
        case IR_MATCH_SOME: return OP_MATCH_SOME;
        case IR_MATCH_NONE: return OP_MATCH_NONE;
        case IR_MATCH_OK: return OP_MATCH_OK;
        case IR_MATCH_ERR: return OP_MATCH_ERR;
        case IR_PHI: return 0; // Phi functions are handled during optimization
        case IR_COPY: return 0; // Copy operations are handled during optimization
        case IR_NOP: return 0; // NOP instructions are ignored
        default: return 0;
    }
}

bool ir_to_bytecode_is_optimizable_local(uint32_t local_index) {
    return local_index < 4;
}

OpCode ir_to_bytecode_get_optimized_load_local(uint32_t local_index) {
    switch (local_index) {
        case 0: return OP_LOAD_LOCAL_0;
        case 1: return OP_LOAD_LOCAL_1;
        case 2: return OP_LOAD_LOCAL_2;
        case 3: return OP_LOAD_LOCAL_3;
        default: return OP_LOAD_LOCAL;
    }
}

OpCode ir_to_bytecode_get_optimized_store_local(uint32_t local_index) {
    switch (local_index) {
        case 0: return OP_STORE_LOCAL_0;
        case 1: return OP_STORE_LOCAL_1;
        case 2: return OP_STORE_LOCAL_2;
        case 3: return OP_STORE_LOCAL_3;
        default: return OP_STORE_LOCAL;
    }
}
