#include "interpreter.h"
#include "../../vm/vm.h"
#include "../../shared/bytecode/opcodes.h"
#include "stack.h"
#include "context.h"
#include "../modules/module_registry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// INTERPRETER IMPLEMENTATION
// ============================================================================

InterpretResult interpret_instruction(VM* vm, uint8_t opcode, uint8_t* operands) {
    if (!vm) return INTERPRET_RUNTIME_ERROR;
    
    switch (opcode) {
        case OP_PUSH_CONSTANT:
            return op_push_constant(vm, *(uint32_t*)operands);
        case OP_PUSH_INT8:
            return op_push_int8(vm, *(int8_t*)operands);
        case OP_PUSH_INT16:
            return op_push_int16(vm, *(int16_t*)operands);
        case OP_PUSH_INT32:
            return op_push_int32(vm, *(int32_t*)operands);
        case OP_PUSH_INT64:
            return op_push_int64(vm, *(int64_t*)operands);
        case OP_PUSH_UINT8:
            return op_push_uint8(vm, *(uint8_t*)operands);
        case OP_PUSH_UINT16:
            return op_push_uint16(vm, *(uint16_t*)operands);
        case OP_PUSH_UINT32:
            return op_push_uint32(vm, *(uint32_t*)operands);
        case OP_PUSH_UINT64:
            return op_push_uint64(vm, *(uint64_t*)operands);
        case OP_PUSH_FLOAT32:
            return op_push_float32(vm, *(float*)operands);
        case OP_PUSH_FLOAT64:
            return op_push_float64(vm, *(double*)operands);
        case OP_PUSH_TRUE:
            return op_push_true(vm);
        case OP_PUSH_FALSE:
            return op_push_false(vm);
        case OP_PUSH_NULL:
            return op_push_null(vm);
        case OP_POP:
            return op_pop(vm);
        case OP_DUP:
            return op_dup(vm);
        case OP_SWAP:
            return op_swap(vm);
        case OP_ADD:
            return op_add(vm);
        case OP_SUB:
            return op_sub(vm);
        case OP_MUL:
            return op_mul(vm);
        case OP_DIV:
            return op_div(vm);
        case OP_LOAD_LOCAL:
            return op_load_local(vm, *(uint32_t*)operands);
        case OP_STORE_LOCAL:
            return op_store_local(vm, *(uint32_t*)operands);
        case OP_INC:
            return op_inc(vm);
        case OP_DEC:
            return op_dec(vm);
        case OP_GE:
            return op_ge(vm);
        case OP_RETURN:
            return op_ret(vm);
        case OP_NEW_OBJECT:
            return op_new_object(vm, *(uint32_t*)operands);
        case OP_CALL:
            return op_call(vm, *(uint32_t*)operands);
        case OP_CALL_VIRTUAL:
            return op_call_virtual(vm, *(uint32_t*)operands);
        case OP_CALL_STATIC:
            return op_call_static(vm, *(uint32_t*)operands);
        case OP_LOAD_FIELD:
            return op_load_field(vm, *(uint32_t*)operands);
        case OP_STORE_FIELD:
            return op_store_field(vm, *(uint32_t*)operands);
        default:
            printf("Runtime error: Invalid Opcode 0x%02X\n", opcode);
            return INTERPRET_RUNTIME_ERROR;
    }
}

// ============================================================================
// STACK OPERATIONS
// ============================================================================

InterpretResult op_push_constant(VM* vm, uint32_t constant_index) {
    if (!vm || !vm->stack || !vm->bytecode || !vm->bytecode->constant_table) {
        return INTERPRET_RUNTIME_ERROR;
    }

    const ConstantEntry* entry = constant_table_get_constant(vm->bytecode->constant_table, constant_index);
    if (!entry) {
        fprintf(stderr, "Runtime error: Invalid constant index %u\n", constant_index);
        return INTERPRET_RUNTIME_ERROR;
    }

    Value val;
    switch (entry->type) {
        case CONSTANT_TYPE_INT64:
            val = value_create_i64(entry->value.int_value);
            break;
        case CONSTANT_TYPE_FLOAT64:
            val = value_create_f64(entry->value.float_value);
            break;
        case CONSTANT_TYPE_BOOLEAN:
            val = value_create_bool(entry->value.bool_value);
            break;
        case CONSTANT_TYPE_STRING:
            // For now, push string offset. Actual string object creation will be later.
            val = value_create_i64(entry->value.string_offset);
            break;
        case CONSTANT_TYPE_NULL:
            val = value_create_null();
            break;
        default:
            fprintf(stderr, "Runtime error: Unknown constant type %d\n", entry->type);
            return INTERPRET_RUNTIME_ERROR;
    }

    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }

    return INTERPRET_OK;
}

InterpretResult op_push_int8(VM* vm, int8_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_int16(VM* vm, int16_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_int32(VM* vm, int32_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_int64(VM* vm, int64_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_uint8(VM* vm, uint8_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_uint16(VM* vm, uint16_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_uint32(VM* vm, uint32_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_uint64(VM* vm, uint64_t value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_i64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_float32(VM* vm, float value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_f64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_float64(VM* vm, double value) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_f64(value);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_true(VM* vm) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_bool(true);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_false(VM* vm) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_bool(false);
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_push_null(VM* vm) {
    if (!vm || !vm->stack) return INTERPRET_RUNTIME_ERROR;
    Value val = value_create_null();
    if (!stack_push(vm->stack, val)) return INTERPRET_STACK_OVERFLOW;
    return INTERPRET_OK;
}

InterpretResult op_pop(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    stack_pop(vm->stack);
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
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_swap(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val1 = stack_pop(vm->stack);
    Value val2 = stack_pop(vm->stack);
    
    if (!stack_push(vm->stack, val1) || !stack_push(vm->stack, val2)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

// ============================================================================
// ARITHMETIC OPERATIONS
// ============================================================================

InterpretResult op_add(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val2 = stack_pop(vm->stack);
    Value val1 = stack_pop(vm->stack);
    
    printf("DEBUG: op_add - val1.type=%d, val2.type=%d\n", val1.type, val2.type);
    
    if (val1.type == VALUE_I64 && val2.type == VALUE_I64) {
        Value result = value_create_i64(val1.data.i64_value + val2.data.i64_value);
        printf("DEBUG: op_add - result=%lld\n", result.data.i64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else if (val1.type == VALUE_F64 && val2.type == VALUE_F64) {
        Value result = value_create_f64(val1.data.f64_value + val2.data.f64_value);
        printf("DEBUG: op_add - result=%f\n", result.data.f64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else if ((val1.type == VALUE_I64 && val2.type == VALUE_F64) || 
               (val1.type == VALUE_F64 && val2.type == VALUE_I64)) {
        // Mixed types: convert both to float
        double v1 = (val1.type == VALUE_I64) ? (double)val1.data.i64_value : val1.data.f64_value;
        double v2 = (val2.type == VALUE_I64) ? (double)val2.data.i64_value : val2.data.f64_value;
        Value result = value_create_f64(v1 + v2);
        printf("DEBUG: op_add - mixed types result=%f\n", result.data.f64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else {
        fprintf(stderr, "Runtime error: Invalid operands for addition (val1.type=%d, val2.type=%d)\n", val1.type, val2.type);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_sub(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val2 = stack_pop(vm->stack);
    Value val1 = stack_pop(vm->stack);
    
    if (val1.type == VALUE_I64 && val2.type == VALUE_I64) {
        Value result = value_create_i64(val1.data.i64_value - val2.data.i64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else if (val1.type == VALUE_F64 && val2.type == VALUE_F64) {
        Value result = value_create_f64(val1.data.f64_value - val2.data.f64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else {
        fprintf(stderr, "Runtime error: Invalid operands for subtraction\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_mul(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val2 = stack_pop(vm->stack);
    Value val1 = stack_pop(vm->stack);
    
    if (val1.type == VALUE_I64 && val2.type == VALUE_I64) {
        Value result = value_create_i64(val1.data.i64_value * val2.data.i64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else if (val1.type == VALUE_F64 && val2.type == VALUE_F64) {
        Value result = value_create_f64(val1.data.f64_value * val2.data.f64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else {
        fprintf(stderr, "Runtime error: Invalid operands for multiplication\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_div(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_size(vm->stack) < 2) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value val2 = stack_pop(vm->stack);
    Value val1 = stack_pop(vm->stack);
    
    if (val1.type == VALUE_I64 && val2.type == VALUE_I64) {
        if (val2.data.i64_value == 0) {
            fprintf(stderr, "Runtime error: Division by zero\n");
            return INTERPRET_RUNTIME_ERROR;
        }
        Value result = value_create_i64(val1.data.i64_value / val2.data.i64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else if (val1.type == VALUE_F64 && val2.type == VALUE_F64) {
        if (val2.data.f64_value == 0.0) {
            fprintf(stderr, "Runtime error: Division by zero\n");
            return INTERPRET_RUNTIME_ERROR;
        }
        Value result = value_create_f64(val1.data.f64_value / val2.data.f64_value);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    } else {
        fprintf(stderr, "Runtime error: Invalid operands for division\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

// ============================================================================
// LOCAL VARIABLE OPERATIONS
// ============================================================================

InterpretResult op_load_local(VM* vm, uint32_t local_index) {
    if (!vm || !vm->stack || !vm->context) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Get current call frame
    CallFrame* frame = execution_context_current_frame(vm->context);
    if (!frame) {
        fprintf(stderr, "Runtime error: No active call frame\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Get local variable value
    Value val = call_frame_get_local(frame, local_index);
    
    // Push onto stack
    if (!stack_push(vm->stack, val)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_store_local(VM* vm, uint32_t local_index) {
    if (!vm || !vm->stack || !vm->context) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (stack_is_empty(vm->stack)) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    // Get current call frame
    CallFrame* frame = execution_context_current_frame(vm->context);
    if (!frame) {
        fprintf(stderr, "Runtime error: No active call frame\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Pop value from stack
    Value val = stack_pop(vm->stack);
    
    // Store in local variable
    if (!call_frame_set_local(frame, local_index, val)) {
        fprintf(stderr, "Runtime error: Failed to store local variable %u\n", local_index);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

// ============================================================================
// CONTROL FLOW OPERATIONS
// ============================================================================

InterpretResult op_ret(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // For now, just mark as completed
    // TODO: Implement proper return handling
    vm->running = false;
    
    return INTERPRET_OK;
}

// ============================================================================
// BYTECODE INTERPRETATION
// ============================================================================

InterpretResult interpret_bytecode(VM* vm, uint8_t* bytecode, size_t size) {
    if (!vm || !bytecode) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    size_t ip = 0; // Instruction pointer
    
    while (ip < size) {
        uint8_t opcode = bytecode[ip++];
        
        // Get operands based on opcode
        uint8_t* operands = NULL;
        size_t operand_size = 0;
        
        // Determine operand size based on opcode
        switch (opcode) {
            case OP_PUSH_INT8:
                operand_size = 1;
                break;
            case OP_PUSH_INT16:
                operand_size = 2;
                break;
            case OP_PUSH_INT32:
            case OP_PUSH_UINT32:
            case OP_PUSH_FLOAT32:
                operand_size = 4;
                break;
            case OP_PUSH_INT64:
            case OP_PUSH_UINT64:
            case OP_PUSH_FLOAT64:
                operand_size = 8;
                break;
            case OP_PUSH_CONSTANT:
            case OP_LOAD_LOCAL:
            case OP_STORE_LOCAL:
                operand_size = 4;
                break;
            default:
                operand_size = 0;
                break;
        }
        
        // Check if we have enough bytes for operands
        if (ip + operand_size > size) {
            fprintf(stderr, "Runtime error: Incomplete instruction at offset %zu\n", ip - 1);
            return INTERPRET_RUNTIME_ERROR;
        }
        
        // Set operands pointer
        if (operand_size > 0) {
            operands = &bytecode[ip];
            ip += operand_size;
        }
        
        // Execute instruction
        InterpretResult result = interpret_instruction(vm, opcode, operands);
        if (result != INTERPRET_OK) {
            return result;
        }
        
        // Check if VM should stop
        if (!vm->running) {
            break;
        }
    }
    
    return INTERPRET_OK;
}

InterpretResult op_inc(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (vm->stack->top < 1) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value* top = &vm->stack->values[vm->stack->top - 1];
    if (top->type == VALUE_I64) {
        top->data.i64_value++;
    } else if (top->type == VALUE_F64) {
        top->data.f64_value++;
    } else {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_dec(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (vm->stack->top < 1) {
        return INTERPRET_STACK_UNDERFLOW;
    }
    
    Value* top = &vm->stack->values[vm->stack->top - 1];
    if (top->type == VALUE_I64) {
        top->data.i64_value--;
    } else if (top->type == VALUE_F64) {
        top->data.f64_value--;
    } else {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_ge(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    if (vm->stack->top < 2) {
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
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value result_value = value_create_bool(result);
    if (!stack_push(vm->stack, result_value)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

// ============================================================================
// OBJECT OPERATIONS
// ============================================================================

InterpretResult op_new_object(VM* vm, uint32_t type_id) {
    if (!vm || !vm->heap) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // For now, create a simple object with the given type ID
    // In a full implementation, we'd look up the class definition
    // and allocate the proper amount of memory
    
    // Create a basic object structure
    // This is a simplified implementation - in reality we'd need
    // to look up the class definition and allocate proper memory
    Value object_value = value_create_object(NULL); // NULL for now
    if (!stack_push(vm->stack, object_value)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    printf("DEBUG: Created object with type_id=%u\n", type_id);
    return INTERPRET_OK;
}

InterpretResult op_call_builtin(VM* vm) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // For now, we'll handle print function calls
    // In a full implementation, we'd look up the function name and dispatch accordingly
    
    // Pop the string argument from the stack
    if (stack_is_empty(vm->stack)) {
        printf("Runtime error: No arguments for built-in function\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value arg = stack_pop(vm->stack);
    if (arg.type != VALUE_STRING) {
        printf("Runtime error: print() expects a string argument\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Print the string
    printf("%s", arg.data.string_value);
    
    // Push a default return value (void)
    Value void_result = {0};
    void_result.type = VALUE_I64;
    void_result.data.i64_value = 0;
    stack_push(vm->stack, void_result);
    
    return INTERPRET_OK;
}

InterpretResult op_call(VM* vm, uint32_t method_id) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Check if this is a built-in function call (method_id 0 is reserved for built-ins)
    if (method_id == 0) {
        return op_call_builtin(vm);
    }
    
    // Look up the method in the module registry
    MethodRegistryEntry* method_entry = method_registry_find_method_by_id(method_id);
    if (!method_entry) {
        printf("Runtime error: Method with id=%u not found\n", method_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("DEBUG: Calling method '%s' (id=%u) from type %u\n", 
           method_entry->method_name, method_id, method_entry->type_id);
    
    // Check if this is a static method
    if (method_entry->method_info && method_entry->method_info->is_static) {
        return op_call_static(vm, method_id);
    } else {
        return op_call_virtual(vm, method_id);
    }
}

InterpretResult op_call_virtual(VM* vm, uint32_t method_id) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Look up the method in the module registry
    MethodRegistryEntry* method_entry = method_registry_find_method_by_id(method_id);
    if (!method_entry) {
        printf("Runtime error: Virtual method with id=%u not found\n", method_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("DEBUG: Virtual call to method '%s' (id=%u)\n", 
           method_entry->method_name, method_id);
    
    // For virtual calls, we need to pop the object from the stack
    if (vm->stack->top == 0) {
        printf("Runtime error: No object on stack for virtual call\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value object = stack_pop(vm->stack);
    if (object.type != VALUE_OBJECT) {
        printf("Runtime error: Expected object on stack for virtual call\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Get the method information
    Method* method_info = method_entry->method_info;
    if (!method_info) {
        printf("Runtime error: Method info not available for method %u\n", method_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Execute the method bytecode
    return execute_method_bytecode(vm, method_info, object);
}

InterpretResult op_call_static(VM* vm, uint32_t method_id) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Look up the method in the module registry
    MethodRegistryEntry* method_entry = method_registry_find_method_by_id(method_id);
    if (!method_entry) {
        printf("Runtime error: Static method with id=%u not found\n", method_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("DEBUG: Static call to method '%s' (id=%u)\n", 
           method_entry->method_name, method_id);
    
    // Get the method information
    Method* method_info = method_entry->method_info;
    if (!method_info) {
        printf("Runtime error: Method info not available for method %u\n", method_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Execute the method bytecode (no object for static methods)
    Value null_object = value_create_null();
    return execute_method_bytecode(vm, method_info, null_object);
}

InterpretResult op_load_field(VM* vm, uint32_t field_id) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Look up the field in the module registry
    FieldRegistryEntry* field_entry = field_registry_find_field_by_id(field_id);
    if (!field_entry) {
        printf("Runtime error: Field with id=%u not found\n", field_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("DEBUG: Loading field '%s' (id=%u) from type %u\n", 
           field_entry->field_name, field_id, field_entry->type_id);
    
    // Pop the object from the stack
    if (vm->stack->top == 0) {
        printf("Runtime error: No object on stack for field access\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value object = stack_pop(vm->stack);
    if (object.type != VALUE_OBJECT) {
        printf("Runtime error: Expected object on stack for field access\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Get the field information from the registry
    Field* field_info = field_entry->field_info;
    if (!field_info) {
        printf("Runtime error: Field info not available for field %u\n", field_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Access the field value from the object's data
    Object* obj = (Object*)object.data.object_value;
    if (!obj) {
        printf("Runtime error: Invalid object reference\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Check if the field offset is valid
    if (field_info->offset >= obj->header.size) {
        printf("Runtime error: Field offset %u exceeds object size %u\n", 
               field_info->offset, obj->header.size);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Read the field value based on its type
    Value field_value;
    uint8_t* field_data = obj->data + field_info->offset;
    
    switch (field_info->type_id) {
        case 1: // i64
            field_value = value_create_i64(*(int64_t*)field_data);
            break;
        case 2: // f64
            field_value = value_create_f64(*(double*)field_data);
            break;
        case 3: // bool
            field_value = value_create_bool(*(bool*)field_data);
            break;
        case 4: // string
            field_value = value_create_string((char*)field_data);
            break;
        default:
            printf("Runtime error: Unknown field type %u\n", field_info->type_id);
            return INTERPRET_RUNTIME_ERROR;
    }
    
    // Push the field value onto the stack
    if (!stack_push(vm->stack, field_value)) {
        return INTERPRET_STACK_OVERFLOW;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_store_field(VM* vm, uint32_t field_id) {
    if (!vm || !vm->stack) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Look up the field in the module registry
    FieldRegistryEntry* field_entry = field_registry_find_field_by_id(field_id);
    if (!field_entry) {
        printf("Runtime error: Field with id=%u not found\n", field_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("DEBUG: Storing field '%s' (id=%u) in type %u\n", 
           field_entry->field_name, field_id, field_entry->type_id);
    
    // Pop the value and object from the stack
    if (vm->stack->top < 2) {
        printf("Runtime error: Not enough values on stack for field store\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    Value value = stack_pop(vm->stack);
    Value object = stack_pop(vm->stack);
    
    if (object.type != VALUE_OBJECT) {
        printf("Runtime error: Expected object on stack for field store\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Get the field information from the registry
    Field* field_info = field_entry->field_info;
    if (!field_info) {
        printf("Runtime error: Field info not available for field %u\n", field_id);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Access the object
    Object* obj = (Object*)object.data.object_value;
    if (!obj) {
        printf("Runtime error: Invalid object reference\n");
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Check if the field offset is valid
    if (field_info->offset >= obj->header.size) {
        printf("Runtime error: Field offset %u exceeds object size %u\n", 
               field_info->offset, obj->header.size);
        return INTERPRET_RUNTIME_ERROR;
    }
    
    // Store the field value based on its type
    uint8_t* field_data = obj->data + field_info->offset;
    
    switch (field_info->type_id) {
        case 1: // i64
            if (value.type != VALUE_I64) {
                printf("Runtime error: Type mismatch for field %s (expected i64, got %d)\n", 
                       field_info->name, value.type);
                return INTERPRET_RUNTIME_ERROR;
            }
            *(int64_t*)field_data = value.data.i64_value;
            break;
        case 2: // f64
            if (value.type != VALUE_F64) {
                printf("Runtime error: Type mismatch for field %s (expected f64, got %d)\n", 
                       field_info->name, value.type);
                return INTERPRET_RUNTIME_ERROR;
            }
            *(double*)field_data = value.data.f64_value;
            break;
        case 3: // bool
            if (value.type != VALUE_BOOL) {
                printf("Runtime error: Type mismatch for field %s (expected bool, got %d)\n", 
                       field_info->name, value.type);
                return INTERPRET_RUNTIME_ERROR;
            }
            *(bool*)field_data = value.data.bool_value;
            break;
        case 4: // string
            if (value.type != VALUE_STRING) {
                printf("Runtime error: Type mismatch for field %s (expected string, got %d)\n", 
                       field_info->name, value.type);
                return INTERPRET_RUNTIME_ERROR;
            }
            // For strings, we need to copy the string data
            strcpy((char*)field_data, value.data.string_value);
            break;
        default:
            printf("Runtime error: Unknown field type %u\n", field_info->type_id);
            return INTERPRET_RUNTIME_ERROR;
    }
    
    return INTERPRET_OK;
}

InterpretResult op_nop(VM* vm) {
    // No operation - just return OK
    return INTERPRET_OK;
}

// Method execution function
InterpretResult execute_method_bytecode(VM* vm, Method* method, Value object) {
    if (!vm || !method) {
        return INTERPRET_RUNTIME_ERROR;
    }
    
    printf("DEBUG: Executing method '%s' with %u bytes of bytecode\n", 
           method->name, method->bytecode_size);
    
    // If no bytecode, return default value
    if (!method->bytecode || method->bytecode_size == 0) {
        printf("DEBUG: Method has no bytecode, returning default value\n");
        Value result = value_create_i64(0);
        if (!stack_push(vm->stack, result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
        return INTERPRET_OK;
    }
    
    // Save current stack state
    size_t original_stack_top = vm->stack->top;
    
    // Push the object onto the stack for method execution
    if (object.type != VALUE_NULL) {
        if (!stack_push(vm->stack, object)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    }
    
    // Execute the method bytecode using the existing interpreter
    InterpretResult result = interpret_bytecode(vm, method->bytecode, method->bytecode_size);
    
    // If the method didn't return a value, push a default one
    if (result == INTERPRET_OK && vm->stack->top == original_stack_top) {
        Value default_result = value_create_i64(0);
        if (!stack_push(vm->stack, default_result)) {
            return INTERPRET_STACK_OVERFLOW;
        }
    }
    
    return result;
}

const char* interpret_result_to_string(InterpretResult result) {
    switch (result) {
        case INTERPRET_OK:
            return "OK";
        case INTERPRET_COMPILE_ERROR:
            return "COMPILE_ERROR";
        case INTERPRET_RUNTIME_ERROR:
            return "RUNTIME_ERROR";
        case INTERPRET_STACK_OVERFLOW:
            return "STACK_OVERFLOW";
        case INTERPRET_STACK_UNDERFLOW:
            return "STACK_UNDERFLOW";
        default:
            return "UNKNOWN";
    }
}