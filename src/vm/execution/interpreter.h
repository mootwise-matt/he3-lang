#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../vm/vm.h"
#include "../loader/bytecode_loader.h"

// Forward declarations
struct VM;
struct CallFrame;

// Instruction interpretation result
typedef enum {
    INTERPRET_OK = 0,
    INTERPRET_RUNTIME_ERROR,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_STACK_OVERFLOW,
    INTERPRET_STACK_UNDERFLOW,
    INTERPRET_INVALID_OPCODE,
    INTERPRET_TYPE_ERROR,
    INTERPRET_MEMORY_ERROR
} InterpretResult;

// Instruction interpreter
InterpretResult interpret_instruction(VM* vm, uint8_t opcode, uint8_t* operands);
InterpretResult interpret_bytecode(VM* vm, uint8_t* bytecode, size_t size);

// Stack operations
InterpretResult op_push_i64(VM* vm, int64_t value);
InterpretResult op_push_f64(VM* vm, double value);
InterpretResult op_push_bool(VM* vm, bool value);
InterpretResult op_push_str(VM* vm, uint32_t string_index);
InterpretResult op_push_constant(VM* vm, uint32_t constant_index);
InterpretResult op_push_null(VM* vm);
InterpretResult op_push_true(VM* vm);
InterpretResult op_push_false(VM* vm);
InterpretResult op_push_int8(VM* vm, int8_t value);
InterpretResult op_push_int16(VM* vm, int16_t value);
InterpretResult op_push_int32(VM* vm, int32_t value);
InterpretResult op_push_int64(VM* vm, int64_t value);
InterpretResult op_push_uint8(VM* vm, uint8_t value);
InterpretResult op_push_uint16(VM* vm, uint16_t value);
InterpretResult op_push_uint32(VM* vm, uint32_t value);
InterpretResult op_push_uint64(VM* vm, uint64_t value);
InterpretResult op_push_float32(VM* vm, float value);
InterpretResult op_push_float64(VM* vm, double value);
InterpretResult op_pop(VM* vm);
InterpretResult op_dup(VM* vm);
InterpretResult op_swap(VM* vm);
InterpretResult op_inc(VM* vm);
InterpretResult op_dec(VM* vm);
InterpretResult op_ge(VM* vm);
InterpretResult op_nop(VM* vm);

// Local variable operations
InterpretResult op_load_local(VM* vm, uint32_t local_index);
InterpretResult op_store_local(VM* vm, uint32_t local_index);
InterpretResult op_load_arg(VM* vm, uint32_t arg_index);

// Arithmetic operations
InterpretResult op_add(VM* vm);
InterpretResult op_sub(VM* vm);
InterpretResult op_mul(VM* vm);
InterpretResult op_div(VM* vm);
InterpretResult op_mod(VM* vm);
InterpretResult op_neg(VM* vm);

// Comparison operations
InterpretResult op_eq(VM* vm);
InterpretResult op_ne(VM* vm);
InterpretResult op_lt(VM* vm);
InterpretResult op_le(VM* vm);
InterpretResult op_gt(VM* vm);
InterpretResult op_ge(VM* vm);

// Logical operations
InterpretResult op_and(VM* vm);
InterpretResult op_or(VM* vm);
InterpretResult op_not(VM* vm);

// Control flow operations
InterpretResult op_jmp(VM* vm, int32_t offset);
InterpretResult op_jmp_if(VM* vm, int32_t offset);
InterpretResult op_jmp_if_false(VM* vm, int32_t offset);
InterpretResult op_call(VM* vm, uint32_t method_index);
InterpretResult op_ret(VM* vm);
InterpretResult op_ret_val(VM* vm);

// Object operations
InterpretResult op_new(VM* vm, uint32_t type_index);
InterpretResult op_new_object(VM* vm, uint32_t type_id);
InterpretResult op_load_field(VM* vm, uint32_t field_index);
InterpretResult op_store_field(VM* vm, uint32_t field_index);
InterpretResult op_call_virtual(VM* vm, uint32_t method_id);
InterpretResult op_call_static(VM* vm, uint32_t method_id);

// Array operations
InterpretResult op_load_index(VM* vm);
InterpretResult op_store_index(VM* vm);
InterpretResult op_array_len(VM* vm);
InterpretResult op_new_array(VM* vm, uint32_t size);

// String operations
InterpretResult op_str_concat(VM* vm);
InterpretResult op_str_len(VM* vm);

// Type operations
InterpretResult op_type_check(VM* vm, uint32_t type_index);
InterpretResult op_type_cast(VM* vm, uint32_t type_index);

// System operations
InterpretResult op_halt(VM* vm);

// Utility functions
const char* interpret_result_to_string(InterpretResult result);
bool is_arithmetic_opcode(uint8_t opcode);
bool is_comparison_opcode(uint8_t opcode);
bool is_logical_opcode(uint8_t opcode);
bool is_control_flow_opcode(uint8_t opcode);
