#include "vm.h"
#include "loader/bytecode_loader.h"
#include "execution/stack.h"
#include "execution/interpreter.h"
#include "execution/context.h"
#include "modules/module_registry.h"
#include "../shared/bytecode/helium_format.h"
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
    vm->current_module = NULL;
    
    vm->stack = stack_create(1024); // Initial stack capacity
    if (!vm->stack) {
        free(vm);
        return NULL;
    }
    
    vm->context = execution_context_create();
    if (!vm->context) {
        stack_destroy(vm->stack);
        free(vm);
        return NULL;
    }
    
    vm->heap = heap_create(16 * 1024 * 1024); // 16MB heap
    if (!vm->heap) {
        execution_context_destroy(vm->context);
        stack_destroy(vm->stack);
        free(vm);
        return NULL;
    }
    
    vm->classes = NULL;
    
    vm->module_registry = module_registry_create();
    if (!vm->module_registry) {
        fprintf(stderr, "Failed to create module registry\n");
        if (vm->stack) stack_destroy(vm->stack);
        if (vm->context) execution_context_destroy(vm->context);
        if (vm->heap) heap_destroy(vm->heap);
        free(vm);
        return NULL;
    }
    
    // Initialize string manager
    if (!string_manager_init(&vm->string_manager, vm)) {
        fprintf(stderr, "Failed to initialize string manager\n");
        if (vm->stack) stack_destroy(vm->stack);
        if (vm->context) execution_context_destroy(vm->context);
        if (vm->heap) heap_destroy(vm->heap);
        if (vm->module_registry) module_registry_destroy(vm->module_registry);
        free(vm);
        return NULL;
    }
    vm->running = false;
    vm->exit_code = 0;
    
    if (!vm->stack || !vm->context) {
        if (vm->stack) stack_destroy(vm->stack);
        if (vm->context) execution_context_destroy(vm->context);
        free(vm);
        return NULL;
    }
    
    return vm;
}

void vm_destroy(VM* vm) {
    if (!vm) return;
    
    // Destroy components
    if (vm->current_module) {
        helium_module_destroy(vm->current_module);
    }
    
    if (vm->stack) {
        stack_destroy(vm->stack);
    }
    
    if (vm->context) {
        execution_context_destroy(vm->context);
    }
    
    if (vm->heap) {
        heap_destroy(vm->heap);
    }
    
    // Clean up object system
    vm_cleanup_object_system(vm);
    
    // Clean up module registry
    if (vm->module_registry) {
        module_registry_destroy(vm->module_registry);
    }
    
    // Clean up string manager
    string_manager_cleanup(&vm->string_manager);
    
    // Clean up global registries
    module_registry_cleanup();
    
    free(vm);
}

// VM Execution
int vm_load_helium3_module(VM* vm, const char* filename) {
    
    if (!vm || !filename) {
        return 0;
    }
    
    // Check file extension
    const char* extension = strrchr(filename, '.');
    if (!extension || strcmp(extension, ".helium3") != 0) {
        fprintf(stderr, "Invalid file format: %s (expected .helium3)\n", filename);
        return 0;
    }
    
    // Load the .helium3 module directly
    HeliumModule* module = helium_module_load(filename);
    if (!module) {
        fprintf(stderr, "Failed to load .helium3 module: %s\n", filename);
        return 0;
    }
    
    
    // Set as current module
    vm->current_module = module;
    
    // Register module in registry for class discovery
    const char* basename = strrchr(filename, '/');
    if (basename) {
        basename++;
    } else {
        basename = filename;
    }
    
    // Remove extension
    char module_name[256];
    strncpy(module_name, basename, sizeof(module_name) - 1);
    module_name[sizeof(module_name) - 1] = '\0';
    char* dot = strrchr(module_name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    // Register module in registry
    if (!module_registry_register_module(vm->module_registry, module_name, module)) {
        fprintf(stderr, "Failed to register module: %s\n", module_name);
        helium_module_destroy(module);
        vm->current_module = NULL;
        return 0;
    }
    
    printf("Loaded .helium3 module: %s\n", filename);
    printf("Module Name: %s\n", module_name);
    printf("Entry Point Method ID: %u\n", module->header.entry_point_method_id);
    printf("Methods: %u\n", module->method_table ? module->method_table->count : 0);
    printf("Types: %u\n", module->type_table ? module->type_table->count : 0);
    
    return 1;
}

int vm_execute(VM* vm) {
    
    if (!vm || !vm->current_module) {
        fprintf(stderr, "No .helium3 module loaded\n");
        return 1;
    }
    
    vm->running = true;
    
    // Check if module has an entry point
    if (vm->current_module->header.entry_point_method_id == 0) {
        printf("Module is a library (no entry point) - classes registered for use by other modules\n");
        vm->running = false;
        return 0;
    }
    
    // Find the entry point method
    uint32_t entry_point_id = vm->current_module->header.entry_point_method_id;
    
    if (!vm->current_module->method_table) {
        fprintf(stderr, "Method table is null\n");
        return 1;
    }
    
    // Find method by ID
    MethodEntry* entry_method = NULL;
    for (uint32_t i = 0; i < vm->current_module->method_table->count; i++) {
        if (vm->current_module->method_table->entries[i].method_id == entry_point_id) {
            entry_method = &vm->current_module->method_table->entries[i];
            break;
        }
    }
    
    if (!entry_method) {
        fprintf(stderr, "Entry point method ID %u not found\n", entry_point_id);
        return 1;
    }
    
    // Get method name for debugging
    const char* method_name = helium_module_get_string(vm->current_module, entry_method->name_offset);
    if (method_name) {
        printf("Executing method: %s\n", method_name);
    }
    
    // Execute the entry point method
    return vm_execute_method(vm, vm->current_module, entry_point_id);
}

int vm_execute_method(VM* vm, HeliumModule* module, uint32_t method_id) {
    if (!vm || !module) {
        fprintf(stderr, "Invalid VM or module\n");
        return 1;
    }
    
    // Find the method by ID
    MethodEntry* method = NULL;
    for (uint32_t i = 0; i < module->method_table->count; i++) {
        if (module->method_table->entries[i].method_id == method_id) {
            method = &module->method_table->entries[i];
            break;
        }
    }
    
    if (!method) {
        fprintf(stderr, "Method ID %u not found\n", method_id);
        return 1;
    }
    
    
    // Create call frame for the method
    CallFrame* method_frame = call_frame_create(
        module->bytecode + method->bytecode_offset,
        method->local_count
    );
    
    if (!method_frame) {
        fprintf(stderr, "Failed to create call frame for method\n");
        return 1;
    }
    
    // Push frame onto execution context
    if (!execution_context_push_frame(vm->context, method_frame)) {
        fprintf(stderr, "Failed to push method frame onto execution context\n");
        call_frame_destroy(method_frame);
        return 1;
    }
    
    // Execute bytecode
    InterpretResult result = interpret_bytecode(vm,
        module->bytecode + method->bytecode_offset,
        method->bytecode_size);
    
    if (result != INTERPRET_OK) {
        fprintf(stderr, "Runtime error: %s\n", interpret_result_to_string(result));
        // Clean up call frame
        CallFrame* frame = execution_context_pop_frame(vm->context);
        if (frame) call_frame_destroy(frame);
        return 1;
    }

    // Clean up call frame
    CallFrame* frame = execution_context_pop_frame(vm->context);
    if (frame) {
        call_frame_destroy(frame);
    }
    
    vm->running = false;
    printf("Method execution completed successfully\n");
    
    // Get return value from stack
    int return_value = 0;

    if (!stack_is_empty(vm->stack)) {
        Value result_value = stack_pop(vm->stack);
        if (result_value.type == VALUE_I64) {
            return_value = (int)result_value.data.i64_value;
        } else if (result_value.type == VALUE_F64) {
            return_value = (int)result_value.data.f64_value;
        }
        value_destroy(&result_value);
    }
    
    printf("Method execution completed with result: %d\n", return_value);
    return return_value;
}

int vm_call_function(VM* vm, const char* function_name, Value* args, size_t arg_count) {
    if (!vm || !vm->current_module || !function_name) {
        return 1;
    }
    
    // Find function in current module
    MethodEntry* method = NULL;
    for (uint32_t i = 0; i < vm->current_module->method_table->count; i++) {
        const char* method_name = helium_module_get_string(vm->current_module, 
            vm->current_module->method_table->entries[i].name_offset);
        if (method_name && strcmp(method_name, function_name) == 0) {
            method = &vm->current_module->method_table->entries[i];
            break;
        }
    }
    
    if (!method) {
        fprintf(stderr, "Function not found: %s\n", function_name);
        return 1;
    }
    
    // TODO: Implement function call with arguments
    // For now, just execute the method bytecode
    InterpretResult result = interpret_bytecode(vm,
        vm->current_module->bytecode + method->bytecode_offset,
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
    if (!vm || !vm->current_module) {
        printf("No .helium3 module loaded\n");
        return;
    }
    
    printf("Module disassembly:\n");
    
    // Show types
    if (vm->current_module->type_table && vm->current_module->type_table->count > 0) {
        printf("  Types (%u):\n", vm->current_module->type_table->count);
        for (uint32_t i = 0; i < vm->current_module->type_table->count; i++) {
            TypeEntry* type = &vm->current_module->type_table->entries[i];
            const char* name = helium_module_get_string(vm->current_module, type->name_offset);
            printf("    %u: %s (ID: %u)\n", i, name ? name : "NULL", type->type_id);
        }
    }
    
    // Show methods
    if (vm->current_module->method_table && vm->current_module->method_table->count > 0) {
        printf("  Methods (%u):\n", vm->current_module->method_table->count);
        for (uint32_t i = 0; i < vm->current_module->method_table->count; i++) {
            MethodEntry* method = &vm->current_module->method_table->entries[i];
            const char* name = helium_module_get_string(vm->current_module, method->name_offset);
            const char* signature = helium_module_get_string(vm->current_module, method->signature_offset);
            printf("    %u: %s %s (ID: %u, Type: %u)\n", 
                   i, name ? name : "NULL", signature ? signature : "NULL", 
                   method->method_id, method->type_id);
        }
    }
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

// Module registry functions
void vm_initialize_module_registry(VM* vm) {
    if (!vm) return;
    
    if (!vm->module_registry) {
        vm->module_registry = module_registry_create();
    }
}

void vm_cleanup_module_registry(VM* vm) {
    if (!vm || !vm->module_registry) return;
    
    module_registry_destroy(vm->module_registry);
    vm->module_registry = NULL;
}

bool vm_load_module(VM* vm, const char* filename) {
    if (!vm || !filename) {
        return false;
    }
    
    if (!vm->module_registry) {
        vm_initialize_module_registry(vm);
    }
    
    const char* extension = strrchr(filename, '.');
    if (!extension) {
        return false;
    }
    
    if (strcmp(extension, ".helium3") == 0) {
        return module_registry_load_helium3_module(vm->module_registry, filename);
    } else if (strcmp(extension, ".bx") == 0) {
        return module_registry_load_bytecode_file(vm->module_registry, filename);
    }
    
    return false;
}

ModuleEntry* vm_find_module(VM* vm, const char* module_name) {
    if (!vm || !vm->module_registry || !module_name) {
        return NULL;
    }
    
    return module_registry_find_module(vm->module_registry, module_name);
}

ClassRegistryEntry* vm_find_class_in_registry(VM* vm, const char* class_name) {
    if (!vm || !class_name) {
        return NULL;
    }
    
    return class_registry_find_class(class_name);
}

MethodRegistryEntry* vm_find_method_in_registry(VM* vm, const char* method_name) {
    if (!vm || !method_name) {
        return NULL;
    }
    
    return method_registry_find_method(method_name);
}

FieldRegistryEntry* vm_find_field_in_registry(VM* vm, const char* field_name) {
    if (!vm || !field_name) {
        return NULL;
    }
    
    return field_registry_find_field(field_name);
}

void vm_print_module_registry(VM* vm) {
    if (!vm || !vm->module_registry) {
        printf("No module registry available\n");
        return;
    }
    
    module_registry_print_info(vm->module_registry);
    class_registry_print_info();
    method_registry_print_info();
    field_registry_print_info();
}

// Multi-module and library support functions

int vm_load_library(VM* vm, const char* library_path) {
    if (!vm || !library_path) {
        return 0;
    }
    
    printf("Loading library: %s\n", library_path);
    
    // Check if library is already loaded
    const char* basename = strrchr(library_path, '/');
    if (basename) {
        basename++;
    } else {
        basename = library_path;
    }
    
    // Remove extension
    char module_name[256];
    strncpy(module_name, basename, sizeof(module_name) - 1);
    module_name[sizeof(module_name) - 1] = '\0';
    char* dot = strrchr(module_name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    // Check if already loaded
    if (vm_find_module(vm, module_name)) {
        printf("Library %s already loaded\n", module_name);
        return 1;
    }
    
    // Load the library module
    if (!vm_load_module(vm, library_path)) {
        fprintf(stderr, "Failed to load library: %s\n", library_path);
        return 0;
    }
    
    printf("Library %s loaded successfully\n", module_name);
    return 1;
}

int vm_load_project_modules(VM* vm, const char* project_file) {
    if (!vm || !project_file) {
        return 0;
    }
    
    printf("Loading project modules from: %s\n", project_file);
    
    // TODO: Parse project file and load all modules
    // For now, this is a placeholder that would:
    // 1. Parse the he3project.json file
    // 2. Load all source files as modules
    // 3. Load all dependencies as libraries
    // 4. Resolve cross-module references
    
    printf("Project module loading not yet implemented\n");
    return 0;
}

bool vm_resolve_dependencies(VM* vm, const char* project_file) {
    if (!vm || !project_file) {
        return false;
    }
    
    printf("Resolving dependencies for project: %s\n", project_file);
    
    // TODO: Implement dependency resolution
    // This would:
    // 1. Parse project dependencies
    // 2. Load required libraries
    // 3. Resolve cross-module class/method references
    // 4. Validate all dependencies are satisfied
    
    printf("Dependency resolution not yet implemented\n");
    return false;
}

ClassRegistryEntry* vm_find_class_anywhere(VM* vm, const char* class_name) {
    if (!vm || !class_name) {
        return NULL;
    }
    
    // Search through all loaded modules for the class
    ModuleEntry* current = vm->module_registry->modules;
    while (current) {
        ClassRegistryEntry* class_entry = class_registry_find_class_by_name_and_module(
            class_name, current->module_id
        );
        if (class_entry) {
            return class_entry;
        }
        current = current->next;
    }
    
    return NULL;
}

MethodRegistryEntry* vm_find_method_anywhere(VM* vm, const char* method_name, const char* class_name) {
    if (!vm || !method_name || !class_name) {
        return NULL;
    }
    
    // First find the class
    ClassRegistryEntry* class_entry = vm_find_class_anywhere(vm, class_name);
    if (!class_entry) {
        return NULL;
    }
    
    // Then find the method in that class
    return method_registry_find_method_by_name_and_type(method_name, class_entry->type_id);
}

FieldRegistryEntry* vm_find_field_anywhere(VM* vm, const char* field_name, const char* class_name) {
    if (!vm || !field_name || !class_name) {
        return NULL;
    }
    
    // First find the class
    ClassRegistryEntry* class_entry = vm_find_class_anywhere(vm, class_name);
    if (!class_entry) {
        return NULL;
    }
    
    // Then find the field in that class
    return field_registry_find_field_by_name_and_type(field_name, class_entry->type_id);
}
