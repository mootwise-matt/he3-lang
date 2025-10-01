#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Forward declarations
struct BytecodeWriter;
struct StringTable;
struct RelocationTable;

// Opcode definitions (matching the instruction set)
typedef enum {
    // Stack operations
    OP_PUSH_I64 = 0x01,      // Push 64-bit integer
    OP_PUSH_F64 = 0x02,      // Push 64-bit float
    OP_PUSH_BOOL = 0x03,     // Push boolean
    OP_PUSH_STR = 0x04,      // Push string reference
    OP_PUSH_NULL = 0x05,     // Push null reference
    OP_POP = 0x06,           // Pop top of stack
    OP_DUP = 0x07,           // Duplicate top of stack
    OP_SWAP = 0x08,          // Swap top two stack elements
    
    // Local variable operations
    OP_LOAD_LOCAL = 0x10,    // Load local variable
    OP_STORE_LOCAL = 0x11,   // Store local variable
    OP_LOAD_LOCAL_0 = 0x12,  // Load local 0 (optimization)
    OP_LOAD_LOCAL_1 = 0x13,  // Load local 1 (optimization)
    OP_LOAD_LOCAL_2 = 0x14,  // Load local 2 (optimization)
    OP_LOAD_LOCAL_3 = 0x15,  // Load local 3 (optimization)
    OP_STORE_LOCAL_0 = 0x16, // Store local 0 (optimization)
    OP_STORE_LOCAL_1 = 0x17, // Store local 1 (optimization)
    OP_STORE_LOCAL_2 = 0x18, // Store local 2 (optimization)
    OP_STORE_LOCAL_3 = 0x19, // Store local 3 (optimization)
    
    // Argument operations
    OP_LOAD_ARG = 0x20,      // Load argument
    OP_LOAD_ARG_0 = 0x21,    // Load argument 0 (optimization)
    OP_LOAD_ARG_1 = 0x22,    // Load argument 1 (optimization)
    OP_LOAD_ARG_2 = 0x23,    // Load argument 2 (optimization)
    OP_LOAD_ARG_3 = 0x24,    // Load argument 3 (optimization)
    
    // Field operations
    OP_LOAD_FIELD = 0x30,    // Load object field
    OP_STORE_FIELD = 0x31,   // Store object field
    OP_LOAD_STATIC = 0x32,   // Load static field
    OP_STORE_STATIC = 0x33,  // Store static field
    
    // Array operations
    OP_LOAD_ARRAY = 0x40,    // Load array element
    OP_STORE_ARRAY = 0x41,   // Store array element
    OP_ARRAY_LENGTH = 0x42,  // Get array length
    OP_NEW_ARRAY = 0x43,     // Create new array
    
    // Arithmetic operations
    OP_ADD = 0x50,           // Addition
    OP_SUB = 0x51,           // Subtraction
    OP_MUL = 0x52,           // Multiplication
    OP_DIV = 0x53,           // Division
    OP_MOD = 0x54,           // Modulo
    OP_NEG = 0x55,           // Negation
    OP_INC = 0x56,           // Increment
    OP_DEC = 0x57,           // Decrement
    
    // Comparison operations
    OP_EQ = 0x60,            // Equal
    OP_NE = 0x61,            // Not equal
    OP_LT = 0x62,            // Less than
    OP_LE = 0x63,            // Less than or equal
    OP_GT = 0x64,            // Greater than
    OP_GE = 0x65,            // Greater than or equal
    
    // Logical operations
    OP_AND = 0x70,           // Logical AND
    OP_OR = 0x71,            // Logical OR
    OP_NOT = 0x72,           // Logical NOT
    OP_XOR = 0x73,           // Logical XOR
    
    // Bitwise operations
    OP_BIT_AND = 0x80,       // Bitwise AND
    OP_BIT_OR = 0x81,        // Bitwise OR
    OP_BIT_XOR = 0x82,       // Bitwise XOR
    OP_BIT_NOT = 0x83,       // Bitwise NOT
    OP_SHL = 0x84,           // Left shift
    OP_SHR = 0x85,           // Right shift
    OP_USHR = 0x86,          // Unsigned right shift
    
    // Control flow
    OP_JMP = 0x90,           // Unconditional jump
    OP_JMPF = 0x91,          // Jump if false
    OP_JMPT = 0x92,          // Jump if true
    OP_JMP_EQ = 0x93,        // Jump if equal
    OP_JMP_NE = 0x94,        // Jump if not equal
    OP_JMP_LT = 0x95,        // Jump if less than
    OP_JMP_LE = 0x96,        // Jump if less than or equal
    OP_JMP_GT = 0x97,        // Jump if greater than
    OP_JMP_GE = 0x98,        // Jump if greater than or equal
    
    // Method calls
    OP_CALL = 0xA0,          // Call method
    OP_CALLV = 0xA1,         // Virtual method call
    OP_CALLI = 0xA2,         // Interface method call
    OP_RETURN = 0xA3,        // Return from method
    OP_RETURN_VAL = 0xA4,    // Return with value
    
    // Object operations
    OP_NEW = 0xB0,           // Create new object
    OP_CAST = 0xB2,          // Type cast
    OP_INSTANCEOF = 0xB3,    // Instance of check
    OP_IS_NULL = 0xB4,       // Null check
    OP_IS_NOT_NULL = 0xB5,   // Not null check
    
    // Type operations
    OP_BOX = 0xC0,           // Box value type
    OP_UNBOX = 0xC1,         // Unbox value type
    OP_GET_TYPE = 0xC2,      // Get object type
    OP_GET_TYPE_NAME = 0xC3, // Get type name
    
    // Async operations
    OP_SPAWN = 0xD0,         // Spawn async task
    OP_AWAIT = 0xD1,         // Await async result
    OP_ASYNC_RETURN = 0xD2,  // Async return
    OP_YIELD = 0xD3,         // Yield execution
    
    // Pattern matching
    OP_MATCH = 0xE0,         // Pattern match
    OP_MATCH_SOME = 0xE1,    // Match Some variant
    OP_MATCH_NONE = 0xE2,    // Match None variant
    OP_MATCH_OK = 0xE3,      // Match Ok variant
    OP_MATCH_ERR = 0xE4,     // Match Err variant
    
    // Security operations
    OP_SECURITY_CHECK = 0xF0, // Security permission check
    OP_DOMAIN_ENTER = 0xF1,   // Enter domain
    OP_DOMAIN_EXIT = 0xF2,    // Exit domain
    OP_CROWD_VERIFY = 0xF3,   // Verify crowd membership
    
    // System operations
    OP_HALT = 0xFF,          // Halt execution
} OpCode;

// Instruction structure
typedef struct Instruction {
    uint8_t opcode;          // Operation code
    uint8_t flags;           // Instruction flags
    uint16_t operand_count;  // Number of operands
    uint32_t operands[];     // Variable-length operand array
} Instruction;

// Instruction flags
#define INST_FLAG_WIDE       0x01  // Wide instruction (64-bit operands)
#define INST_FLAG_CONDITIONAL 0x02 // Conditional execution
#define INST_FLAG_ATOMIC     0x04  // Atomic operation
#define INST_FLAG_ASYNC      0x08  // Async operation

// String table entry
typedef struct StringEntry {
    uint32_t offset;         // Offset in string data
    uint32_t length;         // String length in bytes
    uint32_t hash;           // String hash for fast lookup
} StringEntry;

// String table
typedef struct StringTable {
    uint32_t count;          // Number of strings
    uint32_t total_size;     // Total size of all strings
    StringEntry* entries;    // String entries
    char* data;              // String data
} StringTable;

// Relocation entry
typedef struct RelocationEntry {
    uint32_t offset;         // Offset in bytecode
    uint32_t type;           // Relocation type
    uint32_t symbol_id;      // Symbol ID
    int32_t addend;          // Addend value
} RelocationEntry;

// Relocation table
typedef struct RelocationTable {
    uint32_t count;          // Number of relocations
    RelocationEntry* entries; // Relocation entries
} RelocationTable;

// Bytecode writer
typedef struct BytecodeWriter {
    uint8_t* buffer;         // Output buffer
    size_t capacity;         // Buffer capacity
    size_t position;         // Current position
    size_t size;             // Current size
    
    // String table
    struct StringTable* strings;
    
    // Relocation table
    struct RelocationTable* relocations;
    
    // Error handling
    char* error_message;
    bool has_error;
} BytecodeWriter;

// Bytecode writer functions
BytecodeWriter* bytecode_writer_create(size_t initial_capacity);
void bytecode_writer_destroy(BytecodeWriter* writer);

// Buffer management
bool bytecode_writer_ensure_capacity(BytecodeWriter* writer, size_t additional_bytes);
uint8_t* bytecode_writer_get_current_position(BytecodeWriter* writer);
size_t bytecode_writer_get_size(BytecodeWriter* writer);
const uint8_t* bytecode_writer_get_buffer(BytecodeWriter* writer);

// Basic emission functions
bool bytecode_writer_emit_u8(BytecodeWriter* writer, uint8_t value);
bool bytecode_writer_emit_u16(BytecodeWriter* writer, uint16_t value);
bool bytecode_writer_emit_u32(BytecodeWriter* writer, uint32_t value);
bool bytecode_writer_emit_u64(BytecodeWriter* writer, uint64_t value);
bool bytecode_writer_emit_i8(BytecodeWriter* writer, int8_t value);
bool bytecode_writer_emit_i16(BytecodeWriter* writer, int16_t value);
bool bytecode_writer_emit_i32(BytecodeWriter* writer, int32_t value);
bool bytecode_writer_emit_i64(BytecodeWriter* writer, int64_t value);
bool bytecode_writer_emit_f64(BytecodeWriter* writer, double value);
bool bytecode_writer_emit_bool(BytecodeWriter* writer, bool value);

// String emission
uint32_t bytecode_writer_emit_string(BytecodeWriter* writer, const char* str);
uint32_t bytecode_writer_emit_string_with_length(BytecodeWriter* writer, const char* str, size_t length);

// Instruction emission
bool bytecode_writer_emit_opcode(BytecodeWriter* writer, OpCode opcode);
bool bytecode_writer_emit_operand(BytecodeWriter* writer, uint32_t operand);
bool bytecode_writer_emit_instruction(BytecodeWriter* writer, OpCode opcode, uint32_t* operands, uint16_t operand_count);
bool bytecode_writer_emit_instruction_with_flags(BytecodeWriter* writer, OpCode opcode, uint8_t flags, uint32_t* operands, uint16_t operand_count);

// Relocation support
bool bytecode_writer_add_relocation(BytecodeWriter* writer, uint32_t offset, uint32_t type, uint32_t symbol_id, int32_t addend);
bool bytecode_writer_resolve_relocations(BytecodeWriter* writer);

// String table functions
StringTable* string_table_create(void);
void string_table_destroy(StringTable* table);
uint32_t string_table_add_string(StringTable* table, const char* str);
uint32_t string_table_add_string_with_length(StringTable* table, const char* str, size_t length);
const char* string_table_get_string(StringTable* table, uint32_t id);
uint32_t string_table_get_count(StringTable* table);
uint32_t string_table_get_total_size(StringTable* table);

// Relocation table functions
RelocationTable* relocation_table_create(void);
void relocation_table_destroy(RelocationTable* table);
bool relocation_table_add_entry(RelocationTable* table, uint32_t offset, uint32_t type, uint32_t symbol_id, int32_t addend);
uint32_t relocation_table_get_count(RelocationTable* table);

// Error handling
void bytecode_writer_set_error(BytecodeWriter* writer, const char* message);
const char* bytecode_writer_get_error(BytecodeWriter* writer);
bool bytecode_writer_has_error(BytecodeWriter* writer);

// Utility functions
void bytecode_writer_align(BytecodeWriter* writer, size_t alignment);
void bytecode_writer_pad(BytecodeWriter* writer, size_t bytes);
size_t bytecode_writer_get_aligned_size(BytecodeWriter* writer, size_t alignment);

// Debug functions
void bytecode_writer_print_hex(BytecodeWriter* writer);
void bytecode_writer_print_disassembly(BytecodeWriter* writer);
const char* opcode_to_string(OpCode opcode);
