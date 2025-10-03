#include "module_registry.h"
#include "../objects/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global registries
ModuleRegistry* g_module_registry = NULL;
ClassRegistryEntry* g_class_registry = NULL;
MethodRegistryEntry* g_method_registry = NULL;
FieldRegistryEntry* g_field_registry = NULL;

// Module registry functions
ModuleRegistry* module_registry_create(void) {
    ModuleRegistry* registry = malloc(sizeof(ModuleRegistry));
    if (!registry) {
        return NULL;
    }
    
    registry->modules = NULL;
    registry->next_module_id = 1;
    registry->module_count = 0;
    
    return registry;
}

void module_registry_destroy(ModuleRegistry* registry) {
    if (!registry) return;
    
    ModuleEntry* current = registry->modules;
    while (current) {
        ModuleEntry* next = current->next;
        
        if (current->module_name) free(current->module_name);
        if (current->module_version) free(current->module_version);
        // Don't destroy helium_module here as it might be in use by the VM
        // The VM will handle its own module cleanup
        // if (current->helium_module) helium_module_destroy(current->helium_module);
        // Don't destroy bytecode_file here as it might be in use by the VM
        // The VM will handle its own bytecode file cleanup
        
        free(current);
        current = next;
    }
    
    free(registry);
}

bool module_registry_register_module(ModuleRegistry* registry, const char* filename, HeliumModule* module) {
    if (!registry || !filename || !module) {
        return false;
    }
    
    ModuleEntry* entry = malloc(sizeof(ModuleEntry));
    if (!entry) {
        return false;
    }
    
    // Extract module name from filename
    const char* basename = strrchr(filename, '/');
    if (basename) {
        basename++;
    } else {
        basename = filename;
    }
    
    // Remove extension
    char* module_name = malloc(strlen(basename) + 1);
    if (!module_name) {
        free(entry);
        return false;
    }
    
    strcpy(module_name, basename);
    char* dot = strrchr(module_name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    entry->module_name = module_name;
    
    // Handle null module version gracefully
    const char* version_str = helium_module_get_string(module, module->header.module_version_offset);
    if (version_str) {
        entry->module_version = strdup(version_str);
    } else {
        entry->module_version = strdup("1.0.0"); // Default version
    }
    
    entry->module_id = registry->next_module_id++;
    entry->helium_module = module;
    entry->bytecode_file = NULL;
    entry->next = registry->modules;
    
    registry->modules = entry;
    registry->module_count++;
    
    printf("Registered module: %s (ID: %u)\n", module_name, entry->module_id);
    
    // Discover classes, methods, and fields from this module
    module_registry_discover_classes_from_module(registry, entry->module_id);
    module_registry_discover_methods_from_module(registry, entry->module_id);
    module_registry_discover_fields_from_module(registry, entry->module_id);
    
    return true;
}

bool module_registry_register_bytecode(ModuleRegistry* registry, const char* filename, BytecodeFile* file) {
    if (!registry || !filename || !file) {
        return false;
    }
    
    ModuleEntry* entry = malloc(sizeof(ModuleEntry));
    if (!entry) {
        return false;
    }
    
    // Extract module name from filename
    const char* basename = strrchr(filename, '/');
    if (basename) {
        basename++;
    } else {
        basename = filename;
    }
    
    // Remove extension
    char* module_name = malloc(strlen(basename) + 1);
    if (!module_name) {
        free(entry);
        return false;
    }
    
    strcpy(module_name, basename);
    char* dot = strrchr(module_name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    entry->module_name = module_name;
    entry->module_version = strdup("1.0.0"); // Default version for bytecode files
    entry->module_id = registry->next_module_id++;
    entry->helium_module = NULL;
    entry->bytecode_file = file;
    entry->next = registry->modules;
    
    registry->modules = entry;
    registry->module_count++;
    
    printf("Registered bytecode module: %s (ID: %u)\n", module_name, entry->module_id);
    
    // Discover classes, methods, and fields from this module
    module_registry_discover_classes_from_module(registry, entry->module_id);
    module_registry_discover_methods_from_module(registry, entry->module_id);
    module_registry_discover_fields_from_module(registry, entry->module_id);
    
    return true;
}

ModuleEntry* module_registry_find_module(ModuleRegistry* registry, const char* module_name) {
    if (!registry || !module_name) {
        return NULL;
    }
    
    ModuleEntry* current = registry->modules;
    while (current) {
        if (strcmp(current->module_name, module_name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

ModuleEntry* module_registry_find_module_by_id(ModuleRegistry* registry, uint32_t module_id) {
    if (!registry) {
        return NULL;
    }
    
    ModuleEntry* current = registry->modules;
    while (current) {
        if (current->module_id == module_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void module_registry_print_info(ModuleRegistry* registry) {
    if (!registry) {
        printf("No module registry available\n");
        return;
    }
    
    printf("=== Module Registry ===\n");
    printf("Loaded modules: %u\n", registry->module_count);
    
    ModuleEntry* current = registry->modules;
    while (current) {
        printf("Module %u: %s v%s\n", 
               current->module_id, current->module_name, current->module_version);
        current = current->next;
    }
}

// Class registry functions
bool class_registry_register_class(ClassRegistryEntry* entry) {
    if (!entry) {
        return false;
    }
    
    entry->next = g_class_registry;
    g_class_registry = entry;
    
    printf("Registered class: %s (type_id=%u, module_id=%u)\n", 
           entry->class_name, entry->type_id, entry->module_id);
    
    return true;
}

ClassRegistryEntry* class_registry_find_class(const char* class_name) {
    if (!class_name) {
        return NULL;
    }
    
    ClassRegistryEntry* current = g_class_registry;
    while (current) {
        if (strcmp(current->class_name, class_name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

ClassRegistryEntry* class_registry_find_class_by_id(uint32_t type_id) {
    ClassRegistryEntry* current = g_class_registry;
    while (current) {
        if (current->type_id == type_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

ClassRegistryEntry* class_registry_find_class_by_name_and_module(const char* class_name, uint32_t module_id) {
    if (!class_name) {
        return NULL;
    }
    
    ClassRegistryEntry* current = g_class_registry;
    while (current) {
        if (strcmp(current->class_name, class_name) == 0 && current->module_id == module_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void class_registry_print_info(void) {
    printf("=== Class Registry ===\n");
    
    int count = 0;
    ClassRegistryEntry* current = g_class_registry;
    while (current) {
        printf("Class %d: %s (type_id=%u, module_id=%u)\n", 
               count++, current->class_name, current->type_id, current->module_id);
        current = current->next;
    }
    
    printf("Total classes: %d\n", count);
}

// Method registry functions
bool method_registry_register_method(MethodRegistryEntry* entry) {
    if (!entry) {
        return false;
    }
    
    entry->next = g_method_registry;
    g_method_registry = entry;
    
    printf("Registered method: %s (method_id=%u, type_id=%u, module_id=%u)\n", 
           entry->method_name, entry->method_id, entry->type_id, entry->module_id);
    
    return true;
}

MethodRegistryEntry* method_registry_find_method(const char* method_name) {
    if (!method_name) {
        return NULL;
    }
    
    MethodRegistryEntry* current = g_method_registry;
    while (current) {
        if (strcmp(current->method_name, method_name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

MethodRegistryEntry* method_registry_find_method_by_id(uint32_t method_id) {
    MethodRegistryEntry* current = g_method_registry;
    while (current) {
        if (current->method_id == method_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

MethodRegistryEntry* method_registry_find_method_by_name_and_type(const char* method_name, uint32_t type_id) {
    if (!method_name) {
        return NULL;
    }
    
    MethodRegistryEntry* current = g_method_registry;
    while (current) {
        if (strcmp(current->method_name, method_name) == 0 && current->type_id == type_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void method_registry_print_info(void) {
    printf("=== Method Registry ===\n");
    
    int count = 0;
    MethodRegistryEntry* current = g_method_registry;
    while (current) {
        printf("Method %d: %s (method_id=%u, type_id=%u, module_id=%u)\n", 
               count++, current->method_name, current->method_id, current->type_id, current->module_id);
        current = current->next;
    }
    
    printf("Total methods: %d\n", count);
}

// Field registry functions
bool field_registry_register_field(FieldRegistryEntry* entry) {
    if (!entry) {
        return false;
    }
    
    entry->next = g_field_registry;
    g_field_registry = entry;
    
    printf("Registered field: %s (field_id=%u, type_id=%u, module_id=%u)\n", 
           entry->field_name, entry->field_id, entry->type_id, entry->module_id);
    
    return true;
}

FieldRegistryEntry* field_registry_find_field(const char* field_name) {
    if (!field_name) {
        return NULL;
    }
    
    FieldRegistryEntry* current = g_field_registry;
    while (current) {
        if (strcmp(current->field_name, field_name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

FieldRegistryEntry* field_registry_find_field_by_id(uint32_t field_id) {
    FieldRegistryEntry* current = g_field_registry;
    while (current) {
        if (current->field_id == field_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

FieldRegistryEntry* field_registry_find_field_by_name_and_type(const char* field_name, uint32_t type_id) {
    if (!field_name) {
        return NULL;
    }
    
    FieldRegistryEntry* current = g_field_registry;
    while (current) {
        if (strcmp(current->field_name, field_name) == 0 && current->type_id == type_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void field_registry_print_info(void) {
    printf("=== Field Registry ===\n");
    
    int count = 0;
    FieldRegistryEntry* current = g_field_registry;
    while (current) {
        printf("Field %d: %s (field_id=%u, type_id=%u, module_id=%u)\n", 
               count++, current->field_name, current->field_id, current->type_id, current->module_id);
        current = current->next;
    }
    
    printf("Total fields: %d\n", count);
}

// Module loading and discovery
bool module_registry_load_helium3_module(ModuleRegistry* registry, const char* filename) {
    if (!registry || !filename) {
        return false;
    }
    
    HeliumModule* module = helium_module_load(filename);
    if (!module) {
        return false;
    }
    
    bool result = module_registry_register_module(registry, filename, module);
    return result;
}

bool module_registry_load_bytecode_file(ModuleRegistry* registry, const char* filename) {
    if (!registry || !filename) {
        return false;
    }
    
    BytecodeFile* file = bytecode_file_load(filename);
    if (!file) {
        return false;
    }
    
    return module_registry_register_bytecode(registry, filename, file);
}

bool module_registry_discover_classes_from_module(ModuleRegistry* registry, uint32_t module_id) {
    if (!registry) {
        return false;
    }
    
    ModuleEntry* module_entry = module_registry_find_module_by_id(registry, module_id);
    if (!module_entry) {
        return false;
    }
    
    TypeTable* type_table = NULL;
    StringTable* string_table = NULL;
    
    if (module_entry->helium_module) {
        type_table = module_entry->helium_module->type_table;
        string_table = module_entry->helium_module->string_table_obj;
    } else if (module_entry->bytecode_file) {
        type_table = module_entry->bytecode_file->type_table;
        string_table = module_entry->bytecode_file->string_table;
    }
    
    if (!type_table || !string_table) {
        return false;
    }
    
    // Register each class from the type table
    for (uint32_t i = 0; i < type_table->count; i++) {
        TypeEntry* type_entry = &type_table->entries[i];
        
        // Only register classes (not interfaces, records, etc.)
        if (!(type_entry->flags & TYPE_FLAG_CLASS)) {
            continue;
        }
        
        // Use offset-based access for helium modules
        const char* class_name = NULL;
        if (module_entry->helium_module) {
            class_name = helium_module_get_string(module_entry->helium_module, type_entry->name_offset);
        } else {
            class_name = string_table_get_string(string_table, type_entry->name_offset);
        }
        if (!class_name) {
            continue;
        }
        
        ClassRegistryEntry* class_entry = malloc(sizeof(ClassRegistryEntry));
        if (!class_entry) {
            continue;
        }
        
        class_entry->type_id = type_entry->type_id;
        class_entry->module_id = module_id;
        class_entry->class_name = strdup(class_name);
        class_entry->class_info = NULL; // Will be created when needed
        class_entry->next = NULL;
        
        class_registry_register_class(class_entry);
    }
    
    return true;
}

bool module_registry_discover_methods_from_module(ModuleRegistry* registry, uint32_t module_id) {
    if (!registry) {
        return false;
    }
    
    ModuleEntry* module_entry = module_registry_find_module_by_id(registry, module_id);
    if (!module_entry) {
        return false;
    }
    
    MethodTable* method_table = NULL;
    StringTable* string_table = NULL;
    
    if (module_entry->helium_module) {
        method_table = module_entry->helium_module->method_table;
        string_table = module_entry->helium_module->string_table_obj;
    } else if (module_entry->bytecode_file) {
        method_table = module_entry->bytecode_file->method_table;
        string_table = module_entry->bytecode_file->string_table;
    }
    
    if (!method_table || !string_table) {
        return false;
    }
    
    // Register each method from the method table
    for (uint32_t i = 0; i < method_table->count; i++) {
        MethodEntry* method_entry = &method_table->entries[i];
        
        // Use offset-based access for helium modules
        const char* method_name = NULL;
        const char* signature = NULL;
        if (module_entry->helium_module) {
            method_name = helium_module_get_string(module_entry->helium_module, method_entry->name_offset);
            signature = helium_module_get_string(module_entry->helium_module, method_entry->signature_offset);
        } else {
            method_name = string_table_get_string(string_table, method_entry->name_offset);
            signature = string_table_get_string(string_table, method_entry->signature_offset);
        }
        
        if (!method_name) {
            continue;
        }
        
        MethodRegistryEntry* registry_entry = malloc(sizeof(MethodRegistryEntry));
        if (!registry_entry) {
            continue;
        }
        
        registry_entry->method_id = method_entry->method_id;
        registry_entry->module_id = module_id;
        registry_entry->type_id = method_entry->type_id;
        registry_entry->method_name = strdup(method_name);
        registry_entry->signature = signature ? strdup(signature) : NULL;
        
        // Create the method info structure
        Method* method_info = malloc(sizeof(Method));
        if (method_info) {
            method_info->name = strdup(method_name);
            method_info->signature = signature ? strdup(signature) : NULL;
            method_info->bytecode = NULL; // TODO: Extract bytecode from method entry
            method_info->bytecode_size = 0; // TODO: Extract bytecode size from method entry
            method_info->local_count = 0; // TODO: Extract from method entry
            method_info->param_count = 0; // TODO: Extract from method entry
            method_info->is_static = false; // TODO: Determine from method flags
            method_info->is_virtual = false; // TODO: Determine from method flags
            method_info->is_abstract = false; // TODO: Determine from method flags
            method_info->is_private = false; // TODO: Determine from method flags
            method_info->is_protected = false; // TODO: Determine from method flags
            method_info->is_public = true; // Default to public
            method_info->next = NULL;
        }
        registry_entry->method_info = method_info;
        registry_entry->next = NULL;
        
        method_registry_register_method(registry_entry);
    }
    
    return true;
}

bool module_registry_discover_fields_from_module(ModuleRegistry* registry, uint32_t module_id) {
    if (!registry) {
        return false;
    }
    
    ModuleEntry* module_entry = module_registry_find_module_by_id(registry, module_id);
    if (!module_entry) {
        return false;
    }
    
    FieldTable* field_table = NULL;
    StringTable* string_table = NULL;
    
    if (module_entry->helium_module) {
        field_table = module_entry->helium_module->field_table;
        string_table = module_entry->helium_module->string_table_obj;
    } else if (module_entry->bytecode_file) {
        field_table = module_entry->bytecode_file->field_table;
        string_table = module_entry->bytecode_file->string_table;
    }
    
    if (!field_table || !string_table) {
        return false;
    }
    
    // Register each field from the field table
    for (uint32_t i = 0; i < field_table->count; i++) {
        FieldEntry* field_entry = &field_table->entries[i];
        
        // Use offset-based access for helium modules
        const char* field_name = NULL;
        if (module_entry->helium_module) {
            field_name = helium_module_get_string(module_entry->helium_module, field_entry->name_offset);
        } else {
            field_name = string_table_get_string(string_table, field_entry->name_offset);
        }
        if (!field_name) {
            continue;
        }
        
        FieldRegistryEntry* registry_entry = malloc(sizeof(FieldRegistryEntry));
        if (!registry_entry) {
            continue;
        }
        
        registry_entry->field_id = field_entry->field_id;
        registry_entry->module_id = module_id;
        registry_entry->type_id = field_entry->type_id;
        registry_entry->field_name = strdup(field_name);
        registry_entry->field_type_id = field_entry->field_type_id;
        
        // Create the field info structure
        Field* field_info = malloc(sizeof(Field));
        if (field_info) {
            field_info->name = strdup(field_name);
            field_info->type_id = field_entry->field_type_id;
            field_info->offset = field_entry->offset; // Use the offset from the field entry
            field_info->size = 8; // Default size, TODO: Calculate based on type
            field_info->is_static = false; // TODO: Determine from field flags
            field_info->is_private = false; // TODO: Determine from field flags
            field_info->is_protected = false; // TODO: Determine from field flags
            field_info->is_public = true; // Default to public
            field_info->next = NULL;
        }
        registry_entry->field_info = field_info;
        registry_entry->next = NULL;
        
        field_registry_register_field(registry_entry);
    }
    
    return true;
}

// Utility functions
const char* module_registry_get_string_from_module(ModuleRegistry* registry, uint32_t module_id, uint32_t string_offset) {
    if (!registry) {
        return NULL;
    }
    
    ModuleEntry* module_entry = module_registry_find_module_by_id(registry, module_id);
    if (!module_entry) {
        return NULL;
    }
    
    StringTable* string_table = NULL;
    if (module_entry->helium_module) {
        string_table = module_entry->helium_module->string_table_obj;
    } else if (module_entry->bytecode_file) {
        string_table = module_entry->bytecode_file->string_table;
    }
    
    if (!string_table) {
        return NULL;
    }
    
    return string_table_get_string(string_table, string_offset);
}

uint32_t module_registry_get_next_module_id(ModuleRegistry* registry) {
    if (!registry) {
        return 0;
    }
    
    return registry->next_module_id;
}

void module_registry_cleanup(void) {
    // Clean up class registry
    ClassRegistryEntry* class_current = g_class_registry;
    while (class_current) {
        ClassRegistryEntry* next = class_current->next;
        if (class_current->class_name) free(class_current->class_name);
        free(class_current);
        class_current = next;
    }
    g_class_registry = NULL;
    
    // Clean up method registry
    MethodRegistryEntry* method_current = g_method_registry;
    while (method_current) {
        MethodRegistryEntry* next = method_current->next;
        if (method_current->method_name) free(method_current->method_name);
        if (method_current->signature) free(method_current->signature);
        free(method_current);
        method_current = next;
    }
    g_method_registry = NULL;
    
    // Clean up field registry
    FieldRegistryEntry* field_current = g_field_registry;
    while (field_current) {
        FieldRegistryEntry* next = field_current->next;
        if (field_current->field_name) free(field_current->field_name);
        free(field_current);
        field_current = next;
    }
    g_field_registry = NULL;
}
