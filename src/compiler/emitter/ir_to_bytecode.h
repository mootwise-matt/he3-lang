#pragma once

#include "../ir/ir.h"
#include "../../vm/bytecode/bytecode.h"

// Forward declarations
struct IRToBytecodeTranslator;

// IR to bytecode translator
typedef struct IRToBytecodeTranslator {
    BytecodeWriter* writer;         // Bytecode writer
    IRFunction* current_function;   // Current function being translated
    IRBlock* current_block;         // Current block being translated
    
    // Error handling
    char* error_message;
    bool has_error;
} IRToBytecodeTranslator;

// Translator functions
IRToBytecodeTranslator* ir_to_bytecode_translator_create(BytecodeWriter* writer);
void ir_to_bytecode_translator_destroy(IRToBytecodeTranslator* translator);

// Translation functions
bool ir_to_bytecode_translate_function(IRToBytecodeTranslator* translator, IRFunction* function);
bool ir_to_bytecode_translate_block(IRToBytecodeTranslator* translator, IRBlock* block);
bool ir_to_bytecode_translate_instruction(IRToBytecodeTranslator* translator, IRInstruction* instruction);

// Value translation
bool ir_to_bytecode_translate_value(IRToBytecodeTranslator* translator, IRValue value, uint32_t* operand);

// Error handling
void ir_to_bytecode_translator_set_error(IRToBytecodeTranslator* translator, const char* message);
const char* ir_to_bytecode_translator_get_error(IRToBytecodeTranslator* translator);
bool ir_to_bytecode_translator_has_error(IRToBytecodeTranslator* translator);

// Utility functions
OpCode ir_to_bytecode_get_opcode(IROp ir_op);
bool ir_to_bytecode_is_optimizable_local(uint32_t local_index);
OpCode ir_to_bytecode_get_optimized_load_local(uint32_t local_index);
OpCode ir_to_bytecode_get_optimized_store_local(uint32_t local_index);
