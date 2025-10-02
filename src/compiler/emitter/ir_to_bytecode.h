#pragma once

#include "../ir/ir.h"
#include "../../shared/bytecode/bytecode_format.h"
#include "../../shared/bytecode/opcodes.h"

// Forward declarations
struct IRToBytecodeTranslator;

// IR to bytecode translator
typedef struct IRToBytecodeTranslator {
    BytecodeFile* bytecode_file;    // Shared bytecode file structure
    IRFunction* current_function;   // Current function being translated
    IRBlock* current_block;         // Current block being translated
    
    // String table for method/type names
    StringTable* string_table;
    
    // Constant table for literal values
    ConstantTable* constant_table;
    
    // Method table for generated methods
    MethodTable* method_table;
    
    // Type table for generated types
    TypeTable* type_table;
    
    // Current method being built
    uint8_t* current_bytecode;
    size_t current_bytecode_size;
    size_t current_bytecode_capacity;
    
    // Error handling
    char* error_message;
    bool has_error;
} IRToBytecodeTranslator;

// Creation and destruction
IRToBytecodeTranslator* ir_to_bytecode_translator_create(void);
void ir_to_bytecode_translator_destroy(IRToBytecodeTranslator* translator);

// Translation functions
bool ir_to_bytecode_translate_function(IRToBytecodeTranslator* translator, IRFunction* function);
bool ir_to_bytecode_translate_block(IRToBytecodeTranslator* translator, IRBlock* block);
bool ir_to_bytecode_translate_instruction(IRToBytecodeTranslator* translator, IRInstruction* instruction);

// Bytecode generation
bool ir_to_bytecode_emit_instruction(IRToBytecodeTranslator* translator, uint8_t opcode, const uint8_t* operands, size_t operand_size);
bool ir_to_bytecode_emit_push_constant(IRToBytecodeTranslator* translator, int64_t value);
bool ir_to_bytecode_emit_push_float(IRToBytecodeTranslator* translator, double value);
bool ir_to_bytecode_emit_push_boolean(IRToBytecodeTranslator* translator, bool value);
bool ir_to_bytecode_emit_push_string(IRToBytecodeTranslator* translator, const char* value);
bool ir_to_bytecode_emit_push_null(IRToBytecodeTranslator* translator);

// Table management
uint32_t ir_to_bytecode_add_string(IRToBytecodeTranslator* translator, const char* str);
uint32_t ir_to_bytecode_add_constant(IRToBytecodeTranslator* translator, const ConstantEntry* entry);
uint32_t ir_to_bytecode_add_int64_constant(IRToBytecodeTranslator* translator, int64_t value);
uint32_t ir_to_bytecode_add_float64_constant(IRToBytecodeTranslator* translator, double value);
uint32_t ir_to_bytecode_add_boolean_constant(IRToBytecodeTranslator* translator, bool value);
uint32_t ir_to_bytecode_add_string_constant(IRToBytecodeTranslator* translator, const char* value);
uint32_t ir_to_bytecode_add_null_constant(IRToBytecodeTranslator* translator);
uint32_t ir_to_bytecode_add_method(IRToBytecodeTranslator* translator, const char* name, const char* signature, uint32_t type_id);
uint32_t ir_to_bytecode_add_type(IRToBytecodeTranslator* translator, const char* name, uint32_t super_type_id, uint32_t size);

// Final bytecode generation
BytecodeFile* ir_to_bytecode_generate_file(IRToBytecodeTranslator* translator);

// Error handling
void ir_to_bytecode_translator_set_error(IRToBytecodeTranslator* translator, const char* message);
const char* ir_to_bytecode_translator_get_error(IRToBytecodeTranslator* translator);
bool ir_to_bytecode_translator_has_error(IRToBytecodeTranslator* translator);