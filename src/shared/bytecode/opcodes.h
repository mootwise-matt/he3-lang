#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ============================================================================
// HE³ BYTECODE INSTRUCTION SET
// ============================================================================
// This header defines the complete instruction set for the He³ virtual machine.
// Instructions are designed for a stack-based execution model.

// ============================================================================
// INSTRUCTION FORMAT
// ============================================================================

// All instructions are 1 byte opcode followed by 0-4 bytes of operands
// Operands are stored in little-endian format

// ============================================================================
// STACK OPERATIONS
// ============================================================================

#define OP_PUSH_NULL          0x00  // Push null onto stack
#define OP_PUSH_TRUE          0x01  // Push boolean true onto stack
#define OP_PUSH_FALSE         0x02  // Push boolean false onto stack
#define OP_PUSH_INT8          0x03  // Push 8-bit integer (1 byte operand)
#define OP_PUSH_INT16         0x04  // Push 16-bit integer (2 byte operand)
#define OP_PUSH_INT32         0x05  // Push 32-bit integer (4 byte operand)
#define OP_PUSH_INT64         0x06  // Push 64-bit integer (8 byte operand)
#define OP_PUSH_UINT8         0x07  // Push 8-bit unsigned integer (1 byte operand)
#define OP_PUSH_UINT16        0x08  // Push 16-bit unsigned integer (2 byte operand)
#define OP_PUSH_UINT32        0x09  // Push 32-bit unsigned integer (4 byte operand)
#define OP_PUSH_UINT64        0x0A  // Push 64-bit unsigned integer (8 byte operand)
#define OP_PUSH_FLOAT32       0x0B  // Push 32-bit float (4 byte operand)
#define OP_PUSH_FLOAT64       0x0C  // Push 64-bit float (8 byte operand)
#define OP_PUSH_STRING        0x0D  // Push string (4 byte string index)
#define OP_PUSH_CONSTANT      0x0E  // Push constant (4 byte constant index)

#define OP_POP                0x0F  // Pop value from stack
#define OP_DUP                0x10  // Duplicate top of stack
#define OP_SWAP               0x11  // Swap top two stack values

// ============================================================================
// ARITHMETIC OPERATIONS
// ============================================================================

#define OP_ADD                0x12  // Add top two values (a + b)
#define OP_SUB                0x13  // Subtract top two values (a - b)
#define OP_MUL                0x14  // Multiply top two values (a * b)
#define OP_DIV                0x15  // Divide top two values (a / b)
#define OP_MOD                0x16  // Modulo top two values (a % b)
#define OP_NEG                0x17  // Negate top value (-a)
#define OP_INC                0x18  // Increment top value (a++)
#define OP_DEC                0x19  // Decrement top value (a--)

// ============================================================================
// COMPARISON OPERATIONS
// ============================================================================

#define OP_EQ                 0x20  // Equal (a == b)
#define OP_NE                 0x21  // Not equal (a != b)
#define OP_LT                 0x22  // Less than (a < b)
#define OP_LE                 0x23  // Less than or equal (a <= b)
#define OP_GT                 0x24  // Greater than (a > b)
#define OP_GE                 0x25  // Greater than or equal (a >= b)

// ============================================================================
// LOGICAL OPERATIONS
// ============================================================================

#define OP_AND                0x30  // Logical AND (a && b)
#define OP_OR                 0x31  // Logical OR (a || b)
#define OP_NOT                0x32  // Logical NOT (!a)

// ============================================================================
// BITWISE OPERATIONS
// ============================================================================

#define OP_BIT_AND            0x40  // Bitwise AND (a & b)
#define OP_BIT_OR             0x41  // Bitwise OR (a | b)
#define OP_BIT_XOR            0x42  // Bitwise XOR (a ^ b)
#define OP_BIT_NOT            0x43  // Bitwise NOT (~a)
#define OP_BIT_SHL            0x44  // Left shift (a << b)
#define OP_BIT_SHR            0x45  // Right shift (a >> b)

// ============================================================================
// CONTROL FLOW
// ============================================================================

#define OP_JUMP               0x50  // Unconditional jump (4 byte offset)
#define OP_JUMP_IF_TRUE       0x51  // Jump if true (4 byte offset)
#define OP_JUMP_IF_FALSE      0x52  // Jump if false (4 byte offset)
#define OP_JUMP_IF_NULL       0x53  // Jump if null (4 byte offset)
#define OP_JUMP_IF_NOT_NULL   0x54  // Jump if not null (4 byte offset)

// ============================================================================
// FUNCTION CALLS
// ============================================================================

#define OP_CALL               0x60  // Call function (4 byte method ID)
#define OP_CALL_VIRTUAL       0x61  // Call virtual method (4 byte method ID)
#define OP_CALL_STATIC        0x62  // Call static method (4 byte method ID)
#define OP_CALL_INTERFACE     0x63  // Call interface method (4 byte method ID)
#define OP_RETURN             0x64  // Return from function
#define OP_RETURN_VALUE       0x65  // Return value from function

// ============================================================================
// LOCAL VARIABLES
// ============================================================================

#define OP_LOAD_LOCAL         0x70  // Load local variable (4 byte local index)
#define OP_STORE_LOCAL        0x71  // Store local variable (4 byte local index)
#define OP_LOAD_ARG           0x72  // Load argument (4 byte arg index)
#define OP_STORE_ARG          0x73  // Store argument (4 byte arg index)

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

#define OP_LOAD_GLOBAL        0x80  // Load global variable (4 byte global index)
#define OP_STORE_GLOBAL       0x81  // Store global variable (4 byte global index)

// ============================================================================
// OBJECT OPERATIONS
// ============================================================================

#define OP_NEW_OBJECT         0x90  // Create new object (4 byte type ID)
#define OP_NEW_ARRAY          0x91  // Create new array (4 byte type ID, 4 byte length)
#define OP_LOAD_FIELD         0x92  // Load object field (4 byte field ID)
#define OP_STORE_FIELD        0x93  // Store object field (4 byte field ID)
#define OP_LOAD_STATIC_FIELD  0x94  // Load static field (4 byte field ID)
#define OP_STORE_STATIC_FIELD 0x95  // Store static field (4 byte field ID)

// ============================================================================
// ARRAY OPERATIONS
// ============================================================================

#define OP_LOAD_ARRAY         0xA0  // Load array element (array, index)
#define OP_STORE_ARRAY        0xA1  // Store array element (array, index, value)
#define OP_ARRAY_LENGTH       0xA2  // Get array length
#define OP_ARRAY_RESIZE       0xA3  // Resize array (array, new_length)

// ============================================================================
// TYPE OPERATIONS
// ============================================================================

#define OP_TYPE_OF            0xB0  // Get type of value
#define OP_IS_INSTANCE_OF     0xB1  // Check if value is instance of type (4 byte type ID)
#define OP_CAST               0xB2  // Cast value to type (4 byte type ID)

// ============================================================================
// STRING OPERATIONS
// ============================================================================

#define OP_STRING_LENGTH      0xC0  // Get string length
#define OP_STRING_CONCAT      0xC1  // Concatenate strings
#define OP_STRING_SUBSTRING   0xC2  // Get substring (string, start, length)
#define OP_STRING_INDEX_OF    0xC3  // Find substring index (string, substring)

// ============================================================================
// BUILT-IN FUNCTIONS
// ============================================================================

#define OP_PRINT              0xD0  // Print value to console
#define OP_PRINTLN            0xD1  // Print value and newline to console
#define OP_READ_LINE          0xD2  // Read line from console
#define OP_READ_INT           0xD3  // Read integer from console
#define OP_READ_FLOAT         0xD4  // Read float from console

// ============================================================================
// ASYNC OPERATIONS
// ============================================================================

#define OP_AWAIT              0xE0  // Await async operation
#define OP_YIELD              0xE1  // Yield control to scheduler
#define OP_ASYNC_CALL         0xE2  // Call async function (4 byte method ID)

// ============================================================================
// OPTION/RESULT OPERATIONS
// ============================================================================

#define OP_OPTION_SOME        0xF0  // Create Some(value) option
#define OP_OPTION_NONE        0xF1  // Create None option
#define OP_OPTION_IS_SOME     0xF2  // Check if option is Some
#define OP_OPTION_UNWRAP      0xF3  // Unwrap Some value (panics if None)
#define OP_OPTION_UNWRAP_OR   0xF4  // Unwrap Some value or return default
#define OP_RESULT_OK          0xE5  // Create Ok(value) result
#define OP_RESULT_ERR         0xE6  // Create Err(error) result
#define OP_RESULT_IS_OK       0xE7  // Check if result is Ok
#define OP_RESULT_UNWRAP      0xEC  // Unwrap Ok value (panics if Err)
#define OP_RESULT_UNWRAP_OR   0xED  // Unwrap Ok value or return default

// ============================================================================
// PATTERN MATCHING
// ============================================================================

#define OP_MATCH              0xE8  // Pattern matching (4 byte pattern count)
#define OP_MATCH_CASE         0xE9  // Match case (4 byte case index)
#define OP_MATCH_WHEN         0xEA  // Match when condition (4 byte condition index)

// ============================================================================
// SPECIAL OPERATIONS
// ============================================================================

#define OP_NOP                0xEB  // No operation

// ============================================================================
// SECURITY OPERATIONS
// ============================================================================

#define OP_SECURE_CALL        0xF8  // Secure function call (4 byte method ID)
#define OP_CHECK_PERMISSION   0xF9  // Check permission (4 byte permission ID)
#define OP_DOMAIN_CALL        0xFA  // Cross-domain call (4 byte method ID)

// ============================================================================
// EVENT OPERATIONS
// ============================================================================

#define OP_PUBLISH_EVENT      0xFB  // Publish event (4 byte event ID)
#define OP_SUBSCRIBE_EVENT    0xFC  // Subscribe to event (4 byte event ID)
#define OP_UNSUBSCRIBE_EVENT  0xFD  // Unsubscribe from event (4 byte event ID)

// ============================================================================
// DEBUG OPERATIONS
// ============================================================================

#define OP_BREAKPOINT         0xFE  // Debug breakpoint
#define OP_TRACE              0xFF  // Debug trace

// ============================================================================
// INSTRUCTION UTILITIES
// ============================================================================

// Get instruction name
const char* opcode_get_name(uint8_t opcode);

// Get instruction operand count
uint8_t opcode_get_operand_count(uint8_t opcode);

// Get instruction operand size
uint8_t opcode_get_operand_size(uint8_t opcode);

// Check if instruction is valid
bool opcode_is_valid(uint8_t opcode);

// Check if instruction has operands
bool opcode_has_operands(uint8_t opcode);

// Get instruction category
const char* opcode_get_category(uint8_t opcode);

// ============================================================================
// INSTRUCTION CATEGORIES
// ============================================================================

#define OPCODE_CATEGORY_STACK      "Stack"
#define OPCODE_CATEGORY_ARITHMETIC "Arithmetic"
#define OPCODE_CATEGORY_COMPARISON "Comparison"
#define OPCODE_CATEGORY_LOGICAL    "Logical"
#define OPCODE_CATEGORY_BITWISE    "Bitwise"
#define OPCODE_CATEGORY_CONTROL    "Control Flow"
#define OPCODE_CATEGORY_CALL       "Function Call"
#define OPCODE_CATEGORY_LOCAL      "Local Variables"
#define OPCODE_CATEGORY_GLOBAL     "Global Variables"
#define OPCODE_CATEGORY_OBJECT     "Object Operations"
#define OPCODE_CATEGORY_ARRAY      "Array Operations"
#define OPCODE_CATEGORY_TYPE       "Type Operations"
#define OPCODE_CATEGORY_STRING     "String Operations"
#define OPCODE_CATEGORY_BUILTIN    "Built-in Functions"
#define OPCODE_CATEGORY_ASYNC      "Async Operations"
#define OPCODE_CATEGORY_PATTERN    "Pattern Matching"
#define OPCODE_CATEGORY_SECURITY   "Security"
#define OPCODE_CATEGORY_EVENT      "Event System"
#define OPCODE_CATEGORY_DEBUG      "Debug"

// ============================================================================
// INSTRUCTION DISASSEMBLY
// ============================================================================

// Disassemble single instruction
void opcode_disassemble(uint8_t* bytecode, size_t offset, char* buffer, size_t buffer_size);

// Disassemble instruction with operands
void opcode_disassemble_with_operands(uint8_t* bytecode, size_t offset, char* buffer, size_t buffer_size);

// Get instruction size including operands
size_t opcode_get_size(uint8_t* bytecode, size_t offset);

// ============================================================================
// CONSTANTS
// ============================================================================

// Maximum operand count
#define MAX_OPERAND_COUNT 4

// Maximum operand size
#define MAX_OPERAND_SIZE 8

// Instruction size limits
#define MIN_INSTRUCTION_SIZE 1
#define MAX_INSTRUCTION_SIZE 9  // 1 byte opcode + 8 bytes operands

// ============================================================================
// INSTRUCTION VALIDATION
// ============================================================================

// Validate instruction at offset
bool opcode_validate(uint8_t* bytecode, size_t offset, size_t bytecode_size);

// Validate all instructions in bytecode
bool opcode_validate_all(uint8_t* bytecode, size_t bytecode_size);

// ============================================================================
// INSTRUCTION EXECUTION HELPERS
// ============================================================================

// Read operand from bytecode
uint64_t opcode_read_operand(uint8_t* bytecode, size_t offset, uint8_t operand_size);

// Write operand to bytecode
void opcode_write_operand(uint8_t* bytecode, size_t offset, uint8_t operand_size, uint64_t value);

// Get next instruction offset
size_t opcode_get_next_offset(uint8_t* bytecode, size_t offset);

// ============================================================================
// INSTRUCTION STATISTICS
// ============================================================================

// Count instructions by category
void opcode_count_by_category(uint8_t* bytecode, size_t bytecode_size, uint32_t* counts);

// Get most common instructions
void opcode_get_most_common(uint8_t* bytecode, size_t bytecode_size, uint8_t* opcodes, uint32_t* counts, size_t max_count);

// Calculate instruction density
float opcode_calculate_density(uint8_t* bytecode, size_t bytecode_size);
