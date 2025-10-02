---
layout: page
title: instruction-set
---
# He³ Bytecode Instruction Set

## Overview

The He³ Virtual Machine executes a stack-based bytecode instruction set designed for efficiency, safety, and cross-platform compatibility. This document describes the complete instruction set and execution model.

## Instruction Format

### Basic Instruction Structure

```c
// src/vm/bytecode/instructions.h
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
```

### Opcode Categories

```c
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
    OP_NEW_ARRAY = 0xB1,     // Create new array
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
```

## Stack Operations

### Push Operations

```c
// Push 64-bit integer
void execute_push_i64(VM* vm, Instruction* inst) {
    int64_t value = *(int64_t*)inst->operands;
    stack_push_i64(vm->stack, value);
}

// Push 64-bit float
void execute_push_f64(VM* vm, Instruction* inst) {
    double value = *(double*)inst->operands;
    stack_push_f64(vm->stack, value);
}

// Push boolean
void execute_push_bool(VM* vm, Instruction* inst) {
    bool value = (bool)inst->operands[0];
    stack_push_bool(vm->stack, value);
}

// Push string reference
void execute_push_str(VM* vm, Instruction* inst) {
    uint32_t string_id = inst->operands[0];
    String* str = get_string_by_id(vm, string_id);
    stack_push_object(vm->stack, (Object*)str);
}

// Push null reference
void execute_push_null(VM* vm, Instruction* inst) {
    stack_push_object(vm->stack, NULL);
}
```

### Stack Manipulation

```c
// Pop top of stack
void execute_pop(VM* vm, Instruction* inst) {
    stack_pop(vm->stack);
}

// Duplicate top of stack
void execute_dup(VM* vm, Instruction* inst) {
    Value top = stack_peek(vm->stack);
    stack_push(vm->stack, top);
}

// Swap top two stack elements
void execute_swap(VM* vm, Instruction* inst) {
    Value top = stack_pop(vm->stack);
    Value second = stack_pop(vm->stack);
    stack_push(vm->stack, top);
    stack_push(vm->stack, second);
}
```

## Local Variable Operations

### Load Operations

```c
// Load local variable
void execute_load_local(VM* vm, Instruction* inst) {
    uint32_t local_index = inst->operands[0];
    Value value = get_local(vm, local_index);
    stack_push(vm->stack, value);
}

// Optimized local loads
void execute_load_local_0(VM* vm, Instruction* inst) {
    Value value = get_local(vm, 0);
    stack_push(vm->stack, value);
}

void execute_load_local_1(VM* vm, Instruction* inst) {
    Value value = get_local(vm, 1);
    stack_push(vm->stack, value);
}

void execute_load_local_2(VM* vm, Instruction* inst) {
    Value value = get_local(vm, 2);
    stack_push(vm->stack, value);
}

void execute_load_local_3(VM* vm, Instruction* inst) {
    Value value = get_local(vm, 3);
    stack_push(vm->stack, value);
}
```

### Store Operations

```c
// Store local variable
void execute_store_local(VM* vm, Instruction* inst) {
    uint32_t local_index = inst->operands[0];
    Value value = stack_pop(vm->stack);
    set_local(vm, local_index, value);
}

// Optimized local stores
void execute_store_local_0(VM* vm, Instruction* inst) {
    Value value = stack_pop(vm->stack);
    set_local(vm, 0, value);
}

void execute_store_local_1(VM* vm, Instruction* inst) {
    Value value = stack_pop(vm->stack);
    set_local(vm, 1, value);
}

void execute_store_local_2(VM* vm, Instruction* inst) {
    Value value = stack_pop(vm->stack);
    set_local(vm, 2, value);
}

void execute_store_local_3(VM* vm, Instruction* inst) {
    Value value = stack_pop(vm->stack);
    set_local(vm, 3, value);
}
```

## Field Operations

### Instance Field Access

```c
// Load object field
void execute_load_field(VM* vm, Instruction* inst) {
    uint32_t field_id = inst->operands[0];
    Object* obj = stack_pop_object(vm->stack);
    
    if (!obj) {
        throw_null_pointer_exception("Cannot access field of null object");
        return;
    }
    
    FieldInfo* field = get_field_info(field_id);
    void* field_ptr = get_field_ptr(obj, field->offset);
    Value value = load_field_value(field_ptr, field->type_id);
    stack_push(vm->stack, value);
}

// Store object field
void execute_store_field(VM* vm, Instruction* inst) {
    uint32_t field_id = inst->operands[0];
    Value value = stack_pop(vm->stack);
    Object* obj = stack_pop_object(vm->stack);
    
    if (!obj) {
        throw_null_pointer_exception("Cannot access field of null object");
        return;
    }
    
    FieldInfo* field = get_field_info(field_id);
    void* field_ptr = get_field_ptr(obj, field->offset);
    store_field_value(field_ptr, field->type_id, value);
}
```

### Static Field Access

```c
// Load static field
void execute_load_static(VM* vm, Instruction* inst) {
    uint32_t field_id = inst->operands[0];
    FieldInfo* field = get_field_info(field_id);
    
    if (!(field->flags & FIELD_FLAG_STATIC)) {
        throw_invalid_operation_exception("Field is not static");
        return;
    }
    
    Value value = load_static_field_value(field);
    stack_push(vm->stack, value);
}

// Store static field
void execute_store_static(VM* vm, Instruction* inst) {
    uint32_t field_id = inst->operands[0];
    Value value = stack_pop(vm->stack);
    FieldInfo* field = get_field_info(field_id);
    
    if (!(field->flags & FIELD_FLAG_STATIC)) {
        throw_invalid_operation_exception("Field is not static");
        return;
    }
    
    store_static_field_value(field, value);
}
```

## Arithmetic Operations

### Basic Arithmetic

```c
// Addition
void execute_add(VM* vm, Instruction* inst) {
    Value right = stack_pop(vm->stack);
    Value left = stack_pop(vm->stack);
    Value result = add_values(left, right);
    stack_push(vm->stack, result);
}

// Subtraction
void execute_sub(VM* vm, Instruction* inst) {
    Value right = stack_pop(vm->stack);
    Value left = stack_pop(vm->stack);
    Value result = sub_values(left, right);
    stack_push(vm->stack, result);
}

// Multiplication
void execute_mul(VM* vm, Instruction* inst) {
    Value right = stack_pop(vm->stack);
    Value left = stack_pop(vm->stack);
    Value result = mul_values(left, right);
    stack_push(vm->stack, result);
}

// Division
void execute_div(VM* vm, Instruction* inst) {
    Value right = stack_pop(vm->stack);
    Value left = stack_pop(vm->stack);
    Value result = div_values(left, right);
    stack_push(vm->stack, result);
}

// Modulo
void execute_mod(VM* vm, Instruction* inst) {
    Value right = stack_pop(vm->stack);
    Value left = stack_pop(vm->stack);
    Value result = mod_values(left, right);
    stack_push(vm->stack, result);
}
```

### Value Arithmetic Implementation

```c
// Add two values with type promotion
Value add_values(Value left, Value right) {
    Value result;
    
    if (left.type == VALUE_I64 && right.type == VALUE_I64) {
        result.type = VALUE_I64;
        result.data.i64 = left.data.i64 + right.data.i64;
    } else if (left.type == VALUE_F64 && right.type == VALUE_F64) {
        result.type = VALUE_F64;
        result.data.f64 = left.data.f64 + right.data.f64;
    } else if (left.type == VALUE_I64 && right.type == VALUE_F64) {
        result.type = VALUE_F64;
        result.data.f64 = (double)left.data.i64 + right.data.f64;
    } else if (left.type == VALUE_F64 && right.type == VALUE_I64) {
        result.type = VALUE_F64;
        result.data.f64 = left.data.f64 + (double)right.data.i64;
    } else {
        throw_type_exception("Cannot add incompatible types");
        result.type = VALUE_NULL;
    }
    
    return result;
}
```

## Control Flow Operations

### Jump Operations

```c
// Unconditional jump
void execute_jmp(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    vm->pc += offset;
}

// Jump if false
void execute_jmpf(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    Value condition = stack_pop(vm->stack);
    
    if (!is_truthy(condition)) {
        vm->pc += offset;
    }
}

// Jump if true
void execute_jmpt(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    Value condition = stack_pop(vm->stack);
    
    if (is_truthy(condition)) {
        vm->pc += offset;
    }
}
```

### Truthiness Evaluation

```c
// Check if value is truthy
bool is_truthy(Value value) {
    switch (value.type) {
        case VALUE_BOOL:
            return value.data.boolean;
        case VALUE_I64:
            return value.data.i64 != 0;
        case VALUE_F64:
            return value.data.f64 != 0.0;
        case VALUE_OBJECT:
            return value.data.object != NULL;
        case VALUE_NULL:
            return false;
        default:
            return false;
    }
}
```

## Method Call Operations

### Static Method Calls

```c
// Call method
void execute_call(VM* vm, Instruction* inst) {
    uint32_t method_id = inst->operands[0];
    uint32_t arg_count = inst->operands[1];
    
    MethodInfo* method = get_method_info(method_id);
    if (!method) {
        throw_method_not_found_exception("Method not found");
        return;
    }
    
    // Create new frame
    CallFrame* frame = create_call_frame(vm, method, arg_count);
    if (!frame) {
        throw_stack_overflow_exception("Call stack overflow");
        return;
    }
    
    // Push frame onto call stack
    push_call_frame(vm, frame);
    
    // Set program counter to method start
    vm->pc = method->bytecode_offset;
}
```

### Virtual Method Calls

```c
// Virtual method call
void execute_callv(VM* vm, Instruction* inst) {
    uint32_t method_id = inst->operands[0];
    uint32_t arg_count = inst->operands[1];
    
    // Get object from stack (first argument is 'this')
    Object* obj = stack_peek_object(vm->stack, arg_count - 1);
    if (!obj) {
        throw_null_pointer_exception("Cannot call method on null object");
        return;
    }
    
    // Find virtual method
    MethodInfo* method = find_virtual_method(obj, method_id);
    if (!method) {
        throw_method_not_found_exception("Virtual method not found");
        return;
    }
    
    // Create new frame
    CallFrame* frame = create_call_frame(vm, method, arg_count);
    if (!frame) {
        throw_stack_overflow_exception("Call stack overflow");
        return;
    }
    
    // Push frame onto call stack
    push_call_frame(vm, frame);
    
    // Set program counter to method start
    vm->pc = method->bytecode_offset;
}
```

### Return Operations

```c
// Return from method
void execute_return(VM* vm, Instruction* inst) {
    CallFrame* frame = pop_call_frame(vm);
    if (!frame) {
        // No more frames, execution complete
        vm->halted = true;
        return;
    }
    
    // Restore program counter
    vm->pc = frame->return_pc;
    
    // Free frame
    free_call_frame(frame);
}

// Return with value
void execute_return_val(VM* vm, Instruction* inst) {
    Value return_value = stack_pop(vm->stack);
    
    CallFrame* frame = pop_call_frame(vm);
    if (!frame) {
        // No more frames, execution complete
        vm->halted = true;
        return;
    }
    
    // Restore program counter
    vm->pc = frame->return_pc;
    
    // Push return value onto stack
    stack_push(vm->stack, return_value);
    
    // Free frame
    free_call_frame(frame);
}
```

## Object Operations

### Object Creation

```c
// Create new object
void execute_new(VM* vm, Instruction* inst) {
    uint32_t type_id = inst->operands[0];
    
    TypeInfo* type = get_type_info(type_id);
    if (!type) {
        throw_type_not_found_exception("Type not found");
        return;
    }
    
    Object* obj = allocate_object(type_id, type->size);
    if (!obj) {
        throw_out_of_memory_exception("Failed to allocate object");
        return;
    }
    
    // Initialize object
    initialize_object(obj, type);
    
    // Push object onto stack
    stack_push_object(vm->stack, obj);
}
```

### Type Checking

```c
// Instance of check
void execute_instanceof(VM* vm, Instruction* inst) {
    uint32_t type_id = inst->operands[0];
    Object* obj = stack_pop_object(vm->stack);
    
    bool is_instance = false;
    if (obj) {
        is_instance = is_instance_of(obj, type_id);
    }
    
    stack_push_bool(vm->stack, is_instance);
}

// Type cast
void execute_cast(VM* vm, Instruction* inst) {
    uint32_t type_id = inst->operands[0];
    Object* obj = stack_pop_object(vm->stack);
    
    if (!obj) {
        stack_push_object(vm->stack, NULL);
        return;
    }
    
    if (!is_instance_of(obj, type_id)) {
        throw_class_cast_exception("Cannot cast to incompatible type");
        return;
    }
    
    stack_push_object(vm->stack, obj);
}
```

## Async Operations

### Async Task Creation

```c
// Spawn async task
void execute_spawn(VM* vm, Instruction* inst) {
    uint32_t method_id = inst->operands[0];
    uint32_t arg_count = inst->operands[1];
    
    MethodInfo* method = get_method_info(method_id);
    if (!method) {
        throw_method_not_found_exception("Method not found");
        return;
    }
    
    // Create async task
    AsyncTask* task = create_async_task(vm, method, arg_count);
    if (!task) {
        throw_out_of_memory_exception("Failed to create async task");
        return;
    }
    
    // Start task execution
    start_async_task(task);
    
    // Push task handle onto stack
    stack_push_object(vm->stack, (Object*)task);
}
```

### Async Result Handling

```c
// Await async result
void execute_await(VM* vm, Instruction* inst) {
    Object* task_obj = stack_pop_object(vm->stack);
    if (!task_obj) {
        throw_null_pointer_exception("Cannot await null task");
        return;
    }
    
    AsyncTask* task = (AsyncTask*)task_obj;
    
    if (task->status == ASYNC_COMPLETED) {
        // Task is already complete, push result
        stack_push(vm->stack, task->result);
    } else if (task->status == ASYNC_FAILED) {
        // Task failed, throw exception
        throw_async_exception("Async task failed");
    } else {
        // Task is still running, suspend current execution
        suspend_for_async_task(vm, task);
    }
}
```

## Pattern Matching Operations

### Option Type Matching

```c
// Match Some variant
void execute_match_some(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    Object* obj = stack_peek_object(vm->stack);
    
    if (obj && is_some_variant(obj)) {
        // Extract value from Some
        Value value = extract_some_value(obj);
        stack_push(vm->stack, value);
        vm->pc += offset;
    }
}

// Match None variant
void execute_match_none(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    Object* obj = stack_peek_object(vm->stack);
    
    if (!obj || is_none_variant(obj)) {
        vm->pc += offset;
    }
}
```

### Result Type Matching

```c
// Match Ok variant
void execute_match_ok(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    Object* obj = stack_peek_object(vm->stack);
    
    if (obj && is_ok_variant(obj)) {
        // Extract value from Ok
        Value value = extract_ok_value(obj);
        stack_push(vm->stack, value);
        vm->pc += offset;
    }
}

// Match Err variant
void execute_match_err(VM* vm, Instruction* inst) {
    int32_t offset = (int32_t)inst->operands[0];
    Object* obj = stack_peek_object(vm->stack);
    
    if (obj && is_err_variant(obj)) {
        // Extract error from Err
        Value error = extract_err_value(obj);
        stack_push(vm->stack, error);
        vm->pc += offset;
    }
}
```

## Security Operations

### Domain Security

```c
// Security permission check
void execute_security_check(VM* vm, Instruction* inst) {
    uint32_t permission = inst->operands[0];
    
    if (!has_security_permission(vm, permission)) {
        throw_security_exception("Insufficient security permissions");
        return;
    }
}

// Enter domain
void execute_domain_enter(VM* vm, Instruction* inst) {
    uint32_t domain_id = inst->operands[0];
    
    if (!enter_domain(vm, domain_id)) {
        throw_security_exception("Cannot enter domain");
        return;
    }
}

// Exit domain
void execute_domain_exit(VM* vm, Instruction* inst) {
    if (!exit_domain(vm)) {
        throw_security_exception("Cannot exit domain");
        return;
    }
}
```

## Instruction Execution Engine

### Main Execution Loop

```c
// Main VM execution loop
void execute_bytecode(VM* vm) {
    while (!vm->halted && vm->pc < vm->bytecode_size) {
        Instruction* inst = get_instruction(vm, vm->pc);
        if (!inst) {
            throw_invalid_instruction_exception("Invalid instruction");
            break;
        }
        
        // Execute instruction
        execute_instruction(vm, inst);
        
        // Advance program counter
        vm->pc += get_instruction_size(inst);
    }
}

// Instruction dispatcher
void execute_instruction(VM* vm, Instruction* inst) {
    switch (inst->opcode) {
        case OP_PUSH_I64:
            execute_push_i64(vm, inst);
            break;
        case OP_PUSH_F64:
            execute_push_f64(vm, inst);
            break;
        case OP_PUSH_BOOL:
            execute_push_bool(vm, inst);
            break;
        case OP_PUSH_STR:
            execute_push_str(vm, inst);
            break;
        case OP_PUSH_NULL:
            execute_push_null(vm, inst);
            break;
        case OP_POP:
            execute_pop(vm, inst);
            break;
        case OP_DUP:
            execute_dup(vm, inst);
            break;
        case OP_SWAP:
            execute_swap(vm, inst);
            break;
        // ... other cases
        default:
            throw_invalid_instruction_exception("Unknown opcode");
            break;
    }
}
```

This comprehensive instruction set provides the foundation for executing He³ programs efficiently and safely in the Virtual Machine.
