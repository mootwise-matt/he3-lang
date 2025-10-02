#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../shared/bytecode/bytecode_format.h"
#include "../shared/bytecode/opcodes.h"

// Forward declarations
struct VM;
struct Value;
struct Stack;
struct Heap;
struct Object;
struct Class;

// Include heap and object headers
#include "memory/heap.h"
#include "objects/object.h"

// VM Value types
typedef enum {
    VALUE_NULL = 0,
    VALUE_BOOL,
    VALUE_I64,
    VALUE_F64,
    VALUE_STRING,
    VALUE_OBJECT,
    VALUE_ARRAY
} ValueType;

// VM Value structure
typedef struct Value {
    ValueType type;
    union {
        bool bool_value;
        int64_t i64_value;
        double f64_value;
        char* string_value;
        struct Object* object_value;
        struct Array* array_value;
    } data;
} Value;

// Forward declaration for Stack (defined in stack.h)
struct Stack;

// VM Call Frame
typedef struct CallFrame {
    uint8_t* ip;                    // Instruction pointer
    Value* locals;                  // Local variables
    size_t local_count;             // Number of local variables
    struct CallFrame* caller;       // Previous frame
    struct Object* this_object;     // Current object (for methods)
} CallFrame;

// VM Execution Context
typedef struct ExecutionContext {
    struct CallFrame** frames;      // Call stack (array of pointers)
    size_t frame_count;             // Number of frames
    size_t frame_capacity;          // Frame capacity
    struct CallFrame* current_frame; // Current frame
} ExecutionContext;

// VM Main Structure
typedef struct VM {
    BytecodeFile* bytecode;  // Loaded bytecode
    struct Stack* stack;            // Operand stack
    struct ExecutionContext* context; // Execution context
    struct Heap* heap;              // Memory heap
    struct Class* classes;          // Loaded classes
    bool running;                   // VM running state
    int exit_code;                  // VM exit code
} VM;

// VM Creation and Destruction
VM* vm_create(void);
void vm_destroy(VM* vm);

// VM Execution
int vm_load_bytecode(VM* vm, const char* filename);
int vm_execute(VM* vm);
int vm_call_function(VM* vm, const char* function_name, Value* args, size_t arg_count);

// Stack Operations
struct Stack* stack_create(size_t initial_capacity);
void stack_destroy(struct Stack* stack);
bool stack_push(struct Stack* stack, Value value);
Value stack_pop(struct Stack* stack);
Value stack_peek(struct Stack* stack, size_t offset);
bool stack_is_empty(struct Stack* stack);
size_t stack_size(struct Stack* stack);

// Value Operations
Value value_create_null(void);
Value value_create_bool(bool value);
Value value_create_i64(int64_t value);
Value value_create_f64(double value);
Value value_create_string(const char* value);
Value value_create_object(struct Object* object);
void value_destroy(Value* value);
Value value_copy(Value value);
bool value_equals(Value a, Value b);
const char* value_type_to_string(ValueType type);
void value_print(Value value);

// VM Debugging
void vm_print_stack(VM* vm);
void vm_print_frames(VM* vm);
void vm_disassemble(VM* vm);

// Heap Debugging
void vm_print_heap_stats(VM* vm);
void vm_print_heap_regions(VM* vm);
void vm_validate_heap(VM* vm);

// Object System
void vm_initialize_object_system(VM* vm);
void vm_cleanup_object_system(VM* vm);
struct Class* vm_find_class(VM* vm, const char* name);
struct Class* vm_find_class_by_id(VM* vm, uint32_t type_id);
struct Object* vm_create_object(VM* vm, const char* class_name);
struct Object* vm_create_object_by_class(VM* vm, struct Class* class_info);
void vm_print_object_system(VM* vm);
void vm_print_classes(VM* vm);
void vm_print_objects(VM* vm);
