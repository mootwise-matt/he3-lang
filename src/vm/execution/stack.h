#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../vm/vm.h"

// Stack implementation for VM operand stack
typedef struct Stack {
    Value* values;               // Stack values
    size_t capacity;             // Current capacity
    size_t top;                  // Top of stack index
    size_t max_size;             // Maximum stack size
} Stack;

// Stack creation and destruction
Stack* stack_create(size_t initial_capacity);
void stack_destroy(Stack* stack);

// Stack operations
bool stack_push(Stack* stack, Value value);
Value stack_pop(Stack* stack);
Value stack_peek(Stack* stack, size_t offset);
bool stack_is_empty(Stack* stack);
size_t stack_size(Stack* stack);
bool stack_ensure_capacity(Stack* stack, size_t required_capacity);

// Stack debugging
void stack_print(Stack* stack);
void stack_print_top_n(Stack* stack, size_t n);

// Stack validation
bool stack_validate(Stack* stack);
