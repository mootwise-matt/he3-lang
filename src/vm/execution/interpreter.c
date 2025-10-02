#include "interpreter.h"
#include "../vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Instruction interpretation
InterpretResult interpret_instruction(VM* vm, uint8_t opcode, uint8_t* operands) {
    if (!vm) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    switch (opcode) {
        // Stack operations
        case OP_PUSH_INT64:
            return op_push_i64(vm, *(int64_t*)operands);
        case OP_PUSH_FLOAT64:
            return op_push_f64(vm, *(double*)operands);
        case OP_PUSH_TRUE:
        case OP_PUSH_FALSE:
            return op_push_bool(vm, opcode == OP_PUSH_TRUE);
        case OP_PUSH_STRING:
            return op_push_str(vm, *(uint32_t*)operands);
        case OP_PUSH_NULL:
            return op_push_null(vm);
        case OP_PUSH_CONSTANT:
            return op_push_constant(vm, *(uint32_t*)operands);
        case OP_POP:
            return op_pop(vm);
        case OP_DUP:
            return op_dup(vm);
            
        // Local variable operations
        case OP_LOAD_LOCAL:
            return op_load_local(vm, *(uint32_t*)operands);
        case OP_STORE_LOCAL:
            return op_store_local(vm, *(uint32_t*)operands);
        case OP_LOAD_ARG:
            return op_load_arg(vm, *(uint32_t*)operands);
            
        // Arithmetic operations
        case OP_ADD:
            return op_add(vm);
        case OP_SUB:
            return op_sub(vm);
        case OP_MUL:
            return op_mul(vm);
        case OP_DIV:
            return op_div(vm);
        case OP_MOD:
            return op_mod(vm);
        case OP_NEG:
            return op_neg(vm);
            
        // Comparison operations
        case OP_EQ:
            return op_eq(vm);
        case OP_NE:
            return op_ne(vm);
        case OP_LT:
            return op_lt(vm);
        case OP_LE:
            return op_le(vm);
        case OP_GT:
            return op_gt(vm);
        case OP_GE:
            return op_ge(vm);
            
        // Logical operations
        case OP_AND:
            return op_and(vm);
        case OP_OR:
            return op_or(vm);
        case OP_NOT:
            return op_not(vm);
            
        // Control flow operations
        case OP_JUMP:
            return op_jmp(vm, *(int32_t*)operands);
        case OP_JUMP_IF_TRUE:
            return op_jmp_if(vm, *(int32_t*)operands);
        case OP_JUMP_IF_FALSE:
            return op_jmp_if_false(vm, *(int32_t*)operands);
        case OP_CALL:
            return op_call(vm, *(uint32_t*)operands);
        case OP_RETURN:
            return op_ret(vm);
        case OP_RETURN_VALUE:
            return op_ret_val(vm);
            
        // Object operations
        case OP_NEW_OBJECT:
            return op_new(vm, *(uint32_t*)operands);
        case OP_LOAD_FIELD:
            return op_load_field(vm, *(uint32_t*)operands);
        case OP_STORE_FIELD:
            return op_store_field(vm, *(uint32_t*)operands);
            
        // Array operations
        case OP_LOAD_ARRAY:
            return op_load_index(vm);
        case OP_STORE_ARRAY:
            return op_store_index(vm);
        case OP_ARRAY_LENGTH:
            return op_array_len(vm);
        case OP_NEW_ARRAY:
            return op_new_array(vm, *(uint32_t*)operands);
            
        // String operations
        case OP_STRING_CONCAT:
            return op_str_concat(vm);
        case OP_STRING_LENGTH:
            return op_str_len(vm);
            
        // Type operations
        case OP_IS_INSTANCE_OF:
            return op_type_check(vm, *(uint32_t*)operands);
        case OP_CAST:
            return op_type_cast(vm, *(uint32_t*)operands);
            
        // System operations
        // OP_HALT not defined in shared format
            
        default:
            return INTERPRET_INVALID_OPCODE;
    }
}

InterpretResult interpret_bytecode(VM* vm, uint8_t* bytecode, size_t size) {
    if (!vm || !bytecode) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("Bytecode interpreter: size=%zu bytes\n", size);
    size_t ip = 0; // Instruction pointer
    
    while (ip < size) {
        uint8_t opcode = bytecode[ip++];
        
        // Get operand count for this opcode
        uint8_t operand_count = opcode_get_operand_count(opcode);
        uint8_t operands[16] = {0}; // Max 16 bytes for operands
        
        // Read operands
        for (uint8_t i = 0; i < operand_count; i++) {
            if (ip + i < size) {
                operands[i] = bytecode[ip + i];
            }
        }
        ip += operand_count;
        
        // Debug output
        printf("Executing opcode %d (%s) with %d operands\n", opcode, opcode_get_name(opcode), operand_count);
        if (operand_count > 0) {
            printf("  Operands: ");
            for (uint8_t i = 0; i < operand_count; i++) {
                printf("%d ", operands[i]);
            }
            printf("\n");
        }
        printf("  Stack size before: %zu\n", stack_size(vm->stack));
        
        // Execute instruction
        InterpretResult result = interpret_instruction(vm, opcode, operands);
        if (result != INTERPRET_OK) {
            printf("  Instruction failed with result: %d\n", result);
            return result;
        }
        
        printf("  Stack size after: %zu\n", stack_size(vm->stack));
        printf("\n");
    }
    
    return INTERPRET_OK;
}

// Stack operations
InterpretResult op_push_i64(VM* vm, int64_t value) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_push_f64(VM* vm, double value) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value val = value_create_f64(value);
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_push_bool(VM* vm, bool value) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value val = value_create_bool(value);
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_push_str(VM* vm, uint32_t string_index) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // TODO: Get string from string table
    Value val = value_create_string("string");
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_push_constant(VM* vm, uint32_t constant_index) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // For now, just push the constant index as an integer
    // TODO: Get actual constant value from constant table
    Value val = value_create_i64(constant_index);
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_push_null(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value val = value_create_null();
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_pop(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val = stack_pop(vm->stack);
    value_destroy(&val);
    
    return INTERPRET_OK;
}

InterpretResult op_dup(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val = stack_peek(vm->stack, 0);
    Value copy = value_copy(val);
    if (!stack_push(vm->stack, copy)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

// Local variable operations
InterpretResult op_load_local(VM* vm, uint32_t local_index) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // For now, just push the local index as a value
    // TODO: Implement proper local variable storage
    Value val = value_create_i64(local_index);
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_store_local(VM* vm, uint32_t local_index) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // For now, just pop a value from the stack
    // TODO: Implement proper local variable storage
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val = stack_pop(vm->stack);
    value_destroy(&val);
    
    return INTERPRET_OK;
}

InterpretResult op_load_arg(VM* vm, uint32_t arg_index) {
    // TODO: Implement argument loading
    return INTERPRET_OK;
}

// Arithmetic operations
InterpretResult op_add(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        Value result = value_create_i64(a.data.i64_value + b.data.i64_value);
        stack_push(vm->stack, result);
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        Value result = value_create_f64(a.data.f64_value + b.data.f64_value);
        stack_push(vm->stack, result);
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_sub(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        Value result = value_create_i64(a.data.i64_value - b.data.i64_value);
        stack_push(vm->stack, result);
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        Value result = value_create_f64(a.data.f64_value - b.data.f64_value);
        stack_push(vm->stack, result);
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_mul(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        Value result = value_create_i64(a.data.i64_value * b.data.i64_value);
        stack_push(vm->stack, result);
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        Value result = value_create_f64(a.data.f64_value * b.data.f64_value);
        stack_push(vm->stack, result);
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_div(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        if (b.data.i64_value == 0) {
            value_destroy(&a);
            value_destroy(&b);
            return INTERPRET_RUNTIME_ERROR;
        }
        Value result = value_create_i64(a.data.i64_value / b.data.i64_value);
        stack_push(vm->stack, result);
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        Value result = value_create_f64(a.data.f64_value / b.data.f64_value);
        stack_push(vm->stack, result);
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_mod(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        if (b.data.i64_value == 0) {
            value_destroy(&a);
            value_destroy(&b);
            return INTERPRET_RUNTIME_ERROR;
        }
        Value result = value_create_i64(a.data.i64_value % b.data.i64_value);
        stack_push(vm->stack, result);
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_neg(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val = stack_pop(vm->stack);
    
    if (val.type == VALUE_I64) {
        Value result = value_create_i64(-val.data.i64_value);
        stack_push(vm->stack, result);
    } else if (val.type == VALUE_F64) {
        Value result = value_create_f64(-val.data.f64_value);
        stack_push(vm->stack, result);
    } else {
        value_destroy(&val);
        return INTERPRET_TYPE_ERROR;
    }
    
    value_destroy(&val);
    return INTERPRET_OK;
}

// Comparison operations
InterpretResult op_eq(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    bool result = value_equals(a, b);
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_ne(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    bool result = !value_equals(a, b);
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_lt(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    bool result = false;
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        result = a.data.i64_value < b.data.i64_value;
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        result = a.data.f64_value < b.data.f64_value;
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_le(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    bool result = false;
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        result = a.data.i64_value <= b.data.i64_value;
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        result = a.data.f64_value <= b.data.f64_value;
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_gt(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    bool result = false;
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        result = a.data.i64_value > b.data.i64_value;
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        result = a.data.f64_value > b.data.f64_value;
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_ge(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    bool result = false;
    if (a.type == VALUE_I64 && b.type == VALUE_I64) {
        result = a.data.i64_value >= b.data.i64_value;
    } else if (a.type == VALUE_F64 && b.type == VALUE_F64) {
        result = a.data.f64_value >= b.data.f64_value;
    } else {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

// Logical operations
InterpretResult op_and(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type != VALUE_BOOL || b.type != VALUE_BOOL) {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    bool result = a.data.bool_value && b.data.bool_value;
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_or(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value b = stack_pop(vm->stack);
    Value a = stack_pop(vm->stack);
    
    if (a.type != VALUE_BOOL || b.type != VALUE_BOOL) {
        value_destroy(&a);
        value_destroy(&b);
        return INTERPRET_TYPE_ERROR;
    }
    
    bool result = a.data.bool_value || b.data.bool_value;
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&a);
    value_destroy(&b);
    return INTERPRET_OK;
}

InterpretResult op_not(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val = stack_pop(vm->stack);
    
    if (val.type != VALUE_BOOL) {
        value_destroy(&val);
        return INTERPRET_TYPE_ERROR;
    }
    
    bool result = !val.data.bool_value;
    Value bool_result = value_create_bool(result);
    stack_push(vm->stack, bool_result);
    
    value_destroy(&val);
    return INTERPRET_OK;
}

// Control flow operations
InterpretResult op_jmp(VM* vm, int32_t offset) {
    // TODO: Implement jump
    (void)vm;
    (void)offset;
    return INTERPRET_OK;
}

InterpretResult op_jmp_if(VM* vm, int32_t offset) {
    // TODO: Implement conditional jump
    (void)vm;
    (void)offset;
    return INTERPRET_OK;
}

InterpretResult op_jmp_if_false(VM* vm, int32_t offset) {
    // TODO: Implement conditional jump if false
    (void)vm;
    (void)offset;
    return INTERPRET_OK;
}

InterpretResult op_call(VM* vm, uint32_t method_index) {
    // TODO: Implement method call
    (void)vm;
    (void)method_index;
    return INTERPRET_OK;
}

InterpretResult op_ret(VM* vm) {
    // TODO: Implement return
    (void)vm;
    return INTERPRET_OK;
}

InterpretResult op_ret_val(VM* vm) {
    // TODO: Implement return with value
    (void)vm;
    return INTERPRET_OK;
}

// Object operations
InterpretResult op_new(VM* vm, uint32_t type_index) {
    // TODO: Implement object creation
    (void)vm;
    (void)type_index;
    return INTERPRET_OK;
}

InterpretResult op_load_field(VM* vm, uint32_t field_index) {
    // TODO: Implement field loading
    (void)vm;
    (void)field_index;
    return INTERPRET_OK;
}

InterpretResult op_store_field(VM* vm, uint32_t field_index) {
    // TODO: Implement field storing
    (void)vm;
    (void)field_index;
    return INTERPRET_OK;
}

// Array operations
InterpretResult op_load_index(VM* vm) {
    // TODO: Implement array element loading
    (void)vm;
    return INTERPRET_OK;
}

InterpretResult op_store_index(VM* vm) {
    // TODO: Implement array element storing
    (void)vm;
    return INTERPRET_OK;
}

InterpretResult op_array_len(VM* vm) {
    // TODO: Implement array length
    (void)vm;
    return INTERPRET_OK;
}

InterpretResult op_new_array(VM* vm, uint32_t size) {
    // TODO: Implement array creation
    (void)vm;
    (void)size;
    return INTERPRET_OK;
}

// String operations
InterpretResult op_str_concat(VM* vm) {
    // TODO: Implement string concatenation
    (void)vm;
    return INTERPRET_OK;
}

InterpretResult op_str_len(VM* vm) {
    // TODO: Implement string length
    (void)vm;
    return INTERPRET_OK;
}

// Type operations
InterpretResult op_type_check(VM* vm, uint32_t type_index) {
    // TODO: Implement type checking
    (void)vm;
    (void)type_index;
    return INTERPRET_OK;
}

InterpretResult op_type_cast(VM* vm, uint32_t type_index) {
    // TODO: Implement type casting
    (void)vm;
    (void)type_index;
    return INTERPRET_OK;
}

// System operations
InterpretResult op_halt(VM* vm) {
    if (vm) {
        vm->running = false;
    }
    return INTERPRET_OK;
}

// Utility functions
const char* interpret_result_to_string(InterpretResult result) {
    switch (result) {
        case INTERPRET_OK: return "OK";
        case INTERPRET_RUNTIME_ERROR: return "Runtime Error";
        case INTERPRET_COMPILE_ERROR: return "Compile Error";
        case INTERPRET_STACK_OVERFLOW: return "Stack Overflow";
        case INTERPRET_STACK_UNDERFLOW: return "Stack Underflow";
        case INTERPRET_INVALID_OPCODE: return "Invalid Opcode";
        case INTERPRET_TYPE_ERROR: return "Type Error";
        case INTERPRET_MEMORY_ERROR: return "Memory Error";
        default: return "Unknown Error";
    }
}

bool is_arithmetic_opcode(uint8_t opcode) {
    return opcode >= OP_ADD && opcode <= OP_NEG;
}

bool is_comparison_opcode(uint8_t opcode) {
    return opcode >= OP_EQ && opcode <= OP_GE;
}

bool is_logical_opcode(uint8_t opcode) {
    return opcode >= OP_AND && opcode <= OP_NOT;
}

bool is_control_flow_opcode(uint8_t opcode) {
    return opcode >= OP_JUMP && opcode <= OP_RETURN_VALUE;
}
