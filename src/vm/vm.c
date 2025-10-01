#include "vm.h"
#include "loader/bytecode_loader.h"
#include "execution/stack.h"
#include "execution/interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VM Creation and Destruction
VM* vm_create(void) {
    VM* vm = malloc(sizeof(VM));
    if (!vm) {
        return NULL;
    }
    
    // Initialize VM
    vm->bytecode = NULL;
    vm->stack = stack_create(1024); // Initial stack capacity
    vm->context = NULL;
    vm->heap = heap_create(16 * 1024 * 1024); // 16MB heap
    vm->classes = NULL;
    vm->running = false;
    vm->exit_code = 0;
    
    if (!vm->stack) {
        free(vm);
        return NULL;
    }
    
    return vm;
}

void vm_destroy(VM* vm) {
    if (!vm) return;
    
    // Destroy components
    if (vm->bytecode) {
        bytecode_destroy_file(vm->bytecode);
    }
    
    if (vm->stack) {
        stack_destroy(vm->stack);
    }
    
    if (vm->context) {
        // TODO: Destroy execution context
        free(vm->context);
    }
    
    if (vm->heap) {
        heap_destroy(vm->heap);
    }
    
    // Clean up object system
    vm_cleanup_object_system(vm);
    
    free(vm);
}

// VM Execution
int vm_load_bytecode(VM* vm, const char* filename) {
    if (!vm || !filename) {
        return 0;
    }
    
    // Load bytecode file
    vm->bytecode = bytecode_load_file(filename);
    if (!vm->bytecode) {
        fprintf(stderr, "Failed to load bytecode file: %s\n", filename);
        return 0;
    }
    
    printf("Loaded bytecode file: %s\n", filename);
    printf("Domain: %s\n", bytecode_get_domain_name(vm->bytecode));
    printf("Methods: %u\n", vm->bytecode->method_table ? vm->bytecode->method_table->count : 0);
    printf("Types: %u\n", vm->bytecode->type_table ? vm->bytecode->type_table->count : 0);
    
    return 1;
}

int vm_execute(VM* vm) {
    if (!vm || !vm->bytecode) {
        fprintf(stderr, "No bytecode loaded\n");
        return 1;
    }
    
    vm->running = true;
    
    // Find main function
    MethodEntry* main_method = method_table_find_by_name(vm->bytecode->method_table, "main");
    if (!main_method) {
        fprintf(stderr, "Main function not found\n");
        return 1;
    }
    
    printf("Executing main function...\n");
    
    // Execute bytecode
    InterpretResult result = interpret_bytecode(vm, 
        vm->bytecode->bytecode + main_method->bytecode_offset,
        main_method->bytecode_size);
    
    if (result != INTERPRET_OK) {
        fprintf(stderr, "Runtime error: %s\n", interpret_result_to_string(result));
        return 1;
    }
    
    vm->running = false;
    printf("Execution completed successfully\n");
    
    return 0;
}

int vm_call_function(VM* vm, const char* function_name, Value* args, size_t arg_count) {
    if (!vm || !vm->bytecode || !function_name) {
        return 1;
    }
    
    // Find function
    MethodEntry* method = method_table_find_by_name(vm->bytecode->method_table, function_name);
    if (!method) {
        fprintf(stderr, "Function not found: %s\n", function_name);
        return 1;
    }
    
    // TODO: Implement function call with arguments
    // For now, just execute the method bytecode
    InterpretResult result = interpret_bytecode(vm,
        vm->bytecode->bytecode + method->bytecode_offset,
        method->bytecode_size);
    
    if (result != INTERPRET_OK) {
        fprintf(stderr, "Function call error: %s\n", interpret_result_to_string(result));
        return 1;
    }
    
    return 0;
}

// Value Operations
Value value_create_null(void) {
    Value value = { VALUE_NULL, { .i64_value = 0 } };
    return value;
}

Value value_create_bool(bool value) {
    Value val = { VALUE_BOOL, { .bool_value = value } };
    return val;
}

Value value_create_i64(int64_t value) {
    Value val = { VALUE_I64, { .i64_value = value } };
    return val;
}

Value value_create_f64(double value) {
    Value val = { VALUE_F64, { .f64_value = value } };
    return val;
}

Value value_create_string(const char* value) {
    Value val = { VALUE_STRING, { .string_value = NULL } };
    if (value) {
        val.data.string_value = malloc(strlen(value) + 1);
        if (val.data.string_value) {
            strcpy(val.data.string_value, value);
        }
    }
    return val;
}

Value value_create_object(struct Object* object) {
    Value val = { VALUE_OBJECT, { .object_value = object } };
    return val;
}

void value_destroy(Value* value) {
    if (!value) return;
    
    if (value->type == VALUE_STRING && value->data.string_value) {
        free(value->data.string_value);
        value->data.string_value = NULL;
    }
}

Value value_copy(Value value) {
    Value copy = value;
    if (value.type == VALUE_STRING && value.data.string_value) {
        copy.data.string_value = malloc(strlen(value.data.string_value) + 1);
        if (copy.data.string_value) {
            strcpy(copy.data.string_value, value.data.string_value);
        }
    }
    return copy;
}

bool value_equals(Value a, Value b) {
    if (a.type != b.type) {
        return false;
    }
    
    switch (a.type) {
        case VALUE_NULL:
            return true;
        case VALUE_BOOL:
            return a.data.bool_value == b.data.bool_value;
        case VALUE_I64:
            return a.data.i64_value == b.data.i64_value;
        case VALUE_F64:
            return a.data.f64_value == b.data.f64_value;
        case VALUE_STRING:
            if (!a.data.string_value || !b.data.string_value) {
                return a.data.string_value == b.data.string_value;
            }
            return strcmp(a.data.string_value, b.data.string_value) == 0;
        case VALUE_OBJECT:
            return a.data.object_value == b.data.object_value;
        case VALUE_ARRAY:
            return a.data.array_value == b.data.array_value;
        default:
            return false;
    }
}

const char* value_type_to_string(ValueType type) {
    switch (type) {
        case VALUE_NULL: return "null";
        case VALUE_BOOL: return "bool";
        case VALUE_I64: return "i64";
        case VALUE_F64: return "f64";
        case VALUE_STRING: return "string";
        case VALUE_OBJECT: return "object";
        case VALUE_ARRAY: return "array";
        default: return "unknown";
    }
}

void value_print(Value value) {
    switch (value.type) {
        case VALUE_NULL:
            printf("null");
            break;
        case VALUE_BOOL:
            printf("%s", value.data.bool_value ? "true" : "false");
            break;
        case VALUE_I64:
            printf("%lld", value.data.i64_value);
            break;
        case VALUE_F64:
            printf("%.6g", value.data.f64_value);
            break;
        case VALUE_STRING:
            if (value.data.string_value) {
                printf("\"%s\"", value.data.string_value);
            } else {
                printf("null");
            }
            break;
        case VALUE_OBJECT:
            printf("object@%p", value.data.object_value);
            break;
        case VALUE_ARRAY:
            printf("array@%p", value.data.array_value);
            break;
        default:
            printf("unknown");
            break;
    }
}

// VM Debugging
void vm_print_stack(VM* vm) {
    if (!vm || !vm->stack) {
        printf("No stack available\n");
        return;
    }
    
    printf("Stack (size: %zu):\n", stack_size(vm->stack));
    for (size_t i = 0; i < stack_size(vm->stack); i++) {
        printf("  [%zu]: ", i);
        value_print(stack_peek(vm->stack, stack_size(vm->stack) - 1 - i));
        printf("\n");
    }
}

void vm_print_frames(VM* vm) {
    if (!vm || !vm->context) {
        printf("No execution context available\n");
        return;
    }
    
    printf("Call frames: %zu\n", vm->context->frame_count);
    // TODO: Implement frame printing
}

void vm_disassemble(VM* vm) {
    if (!vm || !vm->bytecode) {
        printf("No bytecode loaded\n");
        return;
    }
    
    printf("Bytecode disassembly:\n");
    // TODO: Implement bytecode disassembly
}

// Heap debugging
void vm_print_heap_stats(VM* vm) {
    if (!vm || !vm->heap) {
        printf("No heap available\n");
        return;
    }
    
    heap_print_stats(vm->heap);
}

void vm_print_heap_regions(VM* vm) {
    if (!vm || !vm->heap) {
        printf("No heap available\n");
        return;
    }
    
    heap_print_regions(vm->heap);
}

void vm_validate_heap(VM* vm) {
    if (!vm || !vm->heap) {
        printf("No heap available\n");
        return;
    }
    
    heap_validate(vm->heap);
}

// Object System
void vm_initialize_object_system(VM* vm) {
    if (!vm) return;
    
    // Initialize object registry
    object_registry_init();
    
    // Create basic system classes
    struct Class* object_class = class_create("System.Object", 1, 0);
    if (object_class) {
        object_registry_register_class(object_class);
        vm->classes = object_class;
    }
    
    // Create basic types
    struct Class* string_class = class_create("System.String", 2, 64);
    if (string_class) {
        class_set_superclass(string_class, object_class);
        object_registry_register_class(string_class);
        string_class->next = vm->classes;
        vm->classes = string_class;
    }
    
    struct Class* integer_class = class_create("System.Int64", 3, 8);
    if (integer_class) {
        class_set_superclass(integer_class, object_class);
        object_registry_register_class(integer_class);
        integer_class->next = vm->classes;
        vm->classes = integer_class;
    }
    
    struct Class* float_class = class_create("System.Float64", 4, 8);
    if (float_class) {
        class_set_superclass(float_class, object_class);
        object_registry_register_class(float_class);
        float_class->next = vm->classes;
        vm->classes = float_class;
    }
    
    struct Class* boolean_class = class_create("System.Boolean", 5, 1);
    if (boolean_class) {
        class_set_superclass(boolean_class, object_class);
        object_registry_register_class(boolean_class);
        boolean_class->next = vm->classes;
        vm->classes = boolean_class;
    }
}

void vm_cleanup_object_system(VM* vm) {
    if (!vm) return;
    
    // Clean up VM class list
    struct Class* current = vm->classes;
    while (current) {
        struct Class* next = current->next;
        if (current) {
            class_destroy(current);
        }
        current = next;
    }
    vm->classes = NULL;
    
    // Clean up object registry (just clear the registry pointer, classes already destroyed)
    object_registry_clear();
}

struct Class* vm_find_class(VM* vm, const char* name) {
    if (!vm || !name) return NULL;
    
    return object_registry_find_class(name);
}

struct Class* vm_find_class_by_id(VM* vm, uint32_t type_id) {
    if (!vm) return NULL;
    
    return object_registry_find_class_by_id(type_id);
}

struct Object* vm_create_object(VM* vm, const char* class_name) {
    if (!vm || !class_name) return NULL;
    
    struct Class* class_info = vm_find_class(vm, class_name);
    if (!class_info) return NULL;
    
    return object_create(vm->heap, class_info);
}

struct Object* vm_create_object_by_class(VM* vm, struct Class* class_info) {
    if (!vm || !class_info) return NULL;
    
    return object_create(vm->heap, class_info);
}

void vm_print_object_system(VM* vm) {
    if (!vm) {
        printf("No VM available\n");
        return;
    }
    
    printf("=== Object System ===\n");
    printf("Loaded classes: %d\n", vm->classes ? 1 : 0);
    
    // Count classes
    int class_count = 0;
    struct Class* current = vm->classes;
    while (current) {
        class_count++;
        current = current->next;
    }
    printf("Total classes: %d\n", class_count);
    
    // Print heap stats
    vm_print_heap_stats(vm);
}

void vm_print_classes(VM* vm) {
    if (!vm) {
        printf("No VM available\n");
        return;
    }
    
    printf("=== Loaded Classes ===\n");
    struct Class* current = vm->classes;
    int index = 0;
    
    while (current) {
        printf("Class %d: %s (type_id=%u, size=%u)\n", 
               index++, current->name, current->type_id, current->size);
        
        if (current->superclass) {
            printf("  Superclass: %s\n", current->superclass->name);
        }
        
        printf("  Methods: %u\n", current->method_count);
        printf("  Fields: %u\n", current->field_count);
        printf("  Interfaces: %u\n", current->interface_count);
        printf("\n");
        
        current = current->next;
    }
}

void vm_print_objects(VM* vm) {
    if (!vm) {
        printf("No VM available\n");
        return;
    }
    
    printf("=== Object System Objects ===\n");
    printf("Note: Object tracking not yet implemented\n");
    printf("Heap contains allocated objects managed by GC\n");
}
