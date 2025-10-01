#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Stack creation and destruction
Stack* stack_create(size_t initial_capacity) {
    Stack* stack = malloc(sizeof(Stack));
    if (!stack) {
        return NULL;
    }
    
    stack->values = malloc(sizeof(Value) * initial_capacity);
    if (!stack->values) {
        free(stack);
        return NULL;
    }
    
    stack->capacity = initial_capacity;
    stack->top = 0;
    stack->max_size = 1024 * 1024; // 1MB max stack size
    
    return stack;
}

void stack_destroy(Stack* stack) {
    if (!stack) return;
    
    // Destroy all values on the stack
    for (size_t i = 0; i < stack->top; i++) {
        value_destroy(&stack->values[i]);
    }
    
    free(stack->values);
    free(stack);
}

// Stack operations
bool stack_push(Stack* stack, Value value) {
    if (!stack) return false;
    
    if (stack->top >= stack->capacity) {
        if (!stack_ensure_capacity(stack, stack->capacity * 2)) {
            return false;
        }
    }
    
    if (stack->top >= stack->max_size) {
        fprintf(stderr, "Stack overflow: maximum size exceeded\n");
        return false;
    }
    
    stack->values[stack->top] = value;
    stack->top++;
    return true;
}

Value stack_pop(Stack* stack) {
    if (!stack || stack->top == 0) {
        Value null_value = value_create_null();
        return null_value;
    }
    
    stack->top--;
    return stack->values[stack->top];
}

Value stack_peek(Stack* stack, size_t offset) {
    if (!stack || offset >= stack->top) {
        Value null_value = value_create_null();
        return null_value;
    }
    
    return stack->values[stack->top - 1 - offset];
}

bool stack_is_empty(Stack* stack) {
    return !stack || stack->top == 0;
}

size_t stack_size(Stack* stack) {
    return stack ? stack->top : 0;
}

bool stack_ensure_capacity(Stack* stack, size_t required_capacity) {
    if (!stack) return false;
    
    if (required_capacity <= stack->capacity) {
        return true;
    }
    
    if (required_capacity > stack->max_size) {
        return false;
    }
    
    Value* new_values = realloc(stack->values, sizeof(Value) * required_capacity);
    if (!new_values) {
        return false;
    }
    
    stack->values = new_values;
    stack->capacity = required_capacity;
    return true;
}

// Stack debugging
void stack_print(Stack* stack) {
    if (!stack) {
        printf("Stack: NULL\n");
        return;
    }
    
    printf("Stack (size: %zu, capacity: %zu):\n", stack->top, stack->capacity);
    for (size_t i = 0; i < stack->top; i++) {
        printf("  [%zu]: ", i);
        value_print(stack->values[i]);
        printf(" (%s)\n", value_type_to_string(stack->values[i].type));
    }
}

void stack_print_top_n(Stack* stack, size_t n) {
    if (!stack) {
        printf("Stack: NULL\n");
        return;
    }
    
    size_t print_count = n < stack->top ? n : stack->top;
    printf("Stack top %zu elements:\n", print_count);
    
    for (size_t i = 0; i < print_count; i++) {
        size_t index = stack->top - 1 - i;
        printf("  [%zu]: ", i);
        value_print(stack->values[index]);
        printf(" (%s)\n", value_type_to_string(stack->values[index].type));
    }
}

// Stack validation
bool stack_validate(Stack* stack) {
    if (!stack) return false;
    if (stack->top > stack->capacity) return false;
    if (stack->top > stack->max_size) return false;
    if (!stack->values) return false;
    
    return true;
}
