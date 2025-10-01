#pragma once
#include <stdint.h>

// He³ Bytecode Opcodes
// This header defines all VM opcodes for the He³ virtual machine

// Stack operations
#define OP_PUSH_I64 0x01
#define OP_PUSH_F64 0x02
#define OP_PUSH_BOOL 0x03
#define OP_PUSH_STR 0x04
#define OP_PUSH_NULL 0x05
#define OP_POP 0x06
#define OP_DUP 0x07

// Local variables and arguments
#define OP_LOAD_LOCAL 0x10
#define OP_STORE_LOCAL 0x11
#define OP_LOAD_ARG 0x12
#define OP_STORE_ARG 0x13

// Arithmetic operations
#define OP_ADD 0x20
#define OP_SUB 0x21
#define OP_MUL 0x22
#define OP_DIV 0x23
#define OP_MOD 0x24
#define OP_NEG 0x25

// Comparison operations
#define OP_EQ 0x30
#define OP_NE 0x31
#define OP_LT 0x32
#define OP_LE 0x33
#define OP_GT 0x34
#define OP_GE 0x35

// Logical operations
#define OP_AND 0x40
#define OP_OR 0x41
#define OP_NOT 0x42

// Control flow
#define OP_JMP 0x50
#define OP_JMP_IF 0x51
#define OP_JMP_IF_FALSE 0x52
#define OP_CALL 0x53
#define OP_CALL_VIRTUAL 0x54
#define OP_RET 0x55
#define OP_RET_VAL 0x56

// Object operations
#define OP_NEW 0x60
#define OP_LOAD_FIELD 0x61
#define OP_STORE_FIELD 0x62
#define OP_BOX 0x63
#define OP_UNBOX 0x64

// Array operations
#define OP_LOAD_INDEX 0x70
#define OP_STORE_INDEX 0x71
#define OP_ARRAY_LEN 0x72
#define OP_NEW_ARRAY 0x73

// String operations
#define OP_STR_CONCAT 0x80
#define OP_STR_LEN 0x81

// Type operations
#define OP_TYPE_CHECK 0x90
#define OP_TYPE_CAST 0x91

// Async operations
#define OP_SPAWN 0xA0
#define OP_AWAIT 0xA1
#define OP_YIELD 0xA2

// Security operations
#define OP_SECURE_CALL 0xB0
#define OP_HANDSHAKE 0xB1
#define OP_VALIDATE_TOKEN 0xB2
#define OP_REVOKE_TOKEN 0xB3

// Event operations
#define OP_PUBLISH_EVENT 0xC0
#define OP_SUBSCRIBE_EVENT 0xC1
#define OP_UNSUBSCRIBE_EVENT 0xC2
#define OP_CREATE_EVENT 0xC3

// System operations
#define OP_HALT 0xFF

// Bytecode instruction structure
typedef struct {
    uint8_t opcode;
    uint8_t operand_count;
    union {
        int64_t i64_operand;
        double f64_operand;
        uint32_t u32_operand;
        uint16_t u16_operand;
        uint8_t u8_operand;
    } operands[4]; // Maximum 4 operands per instruction
} BytecodeInstruction;

// Function to get opcode name for debugging
const char* bytecode_get_opcode_name(uint8_t opcode);

// Function to get operand count for opcode
uint8_t bytecode_get_operand_count(uint8_t opcode);

// Function to format instruction for debugging
void bytecode_format_instruction(const BytecodeInstruction* instr, char* buffer, size_t buffer_size);
