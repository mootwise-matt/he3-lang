#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../shared/bytecode/bytecode_format.h"
#include "../../shared/bytecode/helium_format.h"

// Forward declarations
struct VM;
struct Class;
struct Method;
struct Field;

// Module registry entry
typedef struct ModuleEntry {
    char* module_name;                    // Module name
    char* module_version;                 // Module version
    uint32_t module_id;                   // Unique module identifier
    HeliumModule* helium_module;          // Helium3 module data
    BytecodeFile* bytecode_file;          // Bytecode file data
    struct ModuleEntry* next;             // Next module in list
} ModuleEntry;

// Module registry structure
typedef struct ModuleRegistry {
    ModuleEntry* modules;                 // Linked list of loaded modules
    uint32_t next_module_id;              // Next available module ID
    uint32_t module_count;                // Number of loaded modules
} ModuleRegistry;

// Class registry entry (for runtime class discovery)
typedef struct ClassRegistryEntry {
    uint32_t type_id;                     // Type ID from module
    uint32_t module_id;                   // Owning module ID
    char* class_name;                     // Class name
    struct Class* class_info;             // Runtime class information
    struct ClassRegistryEntry* next;      // Next class in list
} ClassRegistryEntry;

// Method registry entry (for runtime method discovery)
typedef struct MethodRegistryEntry {
    uint32_t method_id;                   // Method ID from module
    uint32_t module_id;                   // Owning module ID
    uint32_t type_id;                     // Owning type ID
    char* method_name;                    // Method name
    char* signature;                      // Method signature
    struct Method* method_info;           // Runtime method information
    struct MethodRegistryEntry* next;     // Next method in list
} MethodRegistryEntry;

// Field registry entry (for runtime field discovery)
typedef struct FieldRegistryEntry {
    uint32_t field_id;                    // Field ID from module
    uint32_t module_id;                   // Owning module ID
    uint32_t type_id;                     // Owning type ID
    char* field_name;                     // Field name
    uint32_t field_type_id;               // Field type ID
    struct Field* field_info;             // Runtime field information
    struct FieldRegistryEntry* next;      // Next field in list
} FieldRegistryEntry;

// Global module registry
extern ModuleRegistry* g_module_registry;
extern ClassRegistryEntry* g_class_registry;
extern MethodRegistryEntry* g_method_registry;
extern FieldRegistryEntry* g_field_registry;

// Module registry functions
ModuleRegistry* module_registry_create(void);
void module_registry_destroy(ModuleRegistry* registry);
bool module_registry_register_module(ModuleRegistry* registry, const char* filename, HeliumModule* module);
bool module_registry_register_bytecode(ModuleRegistry* registry, const char* filename, BytecodeFile* file);
ModuleEntry* module_registry_find_module(ModuleRegistry* registry, const char* module_name);
ModuleEntry* module_registry_find_module_by_id(ModuleRegistry* registry, uint32_t module_id);
void module_registry_print_info(ModuleRegistry* registry);

// Class registry functions
bool class_registry_register_class(ClassRegistryEntry* entry);
ClassRegistryEntry* class_registry_find_class(const char* class_name);
ClassRegistryEntry* class_registry_find_class_by_id(uint32_t type_id);
ClassRegistryEntry* class_registry_find_class_by_name_and_module(const char* class_name, uint32_t module_id);
void class_registry_print_info(void);

// Method registry functions
bool method_registry_register_method(MethodRegistryEntry* entry);
MethodRegistryEntry* method_registry_find_method(const char* method_name);
MethodRegistryEntry* method_registry_find_method_by_id(uint32_t method_id);
MethodRegistryEntry* method_registry_find_method_by_name_and_type(const char* method_name, uint32_t type_id);
void method_registry_print_info(void);

// Field registry functions
bool field_registry_register_field(FieldRegistryEntry* entry);
FieldRegistryEntry* field_registry_find_field(const char* field_name);
FieldRegistryEntry* field_registry_find_field_by_id(uint32_t field_id);
FieldRegistryEntry* field_registry_find_field_by_name_and_type(const char* field_name, uint32_t type_id);
void field_registry_print_info(void);

// Module loading and discovery
bool module_registry_load_helium3_module(ModuleRegistry* registry, const char* filename);
bool module_registry_load_bytecode_file(ModuleRegistry* registry, const char* filename);
bool module_registry_discover_classes_from_module(ModuleRegistry* registry, uint32_t module_id);
bool module_registry_discover_methods_from_module(ModuleRegistry* registry, uint32_t module_id);
bool module_registry_discover_fields_from_module(ModuleRegistry* registry, uint32_t module_id);

// Utility functions
const char* module_registry_get_string_from_module(ModuleRegistry* registry, uint32_t module_id, uint32_t string_offset);
uint32_t module_registry_get_next_module_id(ModuleRegistry* registry);
void module_registry_cleanup(void);
