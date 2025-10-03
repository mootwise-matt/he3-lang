#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../shared/bytecode/bytecode_format.h"

// Forward declarations
struct VM;

// Global string registry entry
typedef struct GlobalStringEntry {
    uint32_t global_string_id;        // Unique global string ID
    uint32_t module_id;               // Owning module ID
    uint32_t module_string_offset;    // Offset in module's string table
    char* string_data;                // Cached string data
    uint32_t hash;                    // String hash for fast lookup
    struct GlobalStringEntry* next;   // Next entry in hash chain
} GlobalStringEntry;

// Module string mapping entry
typedef struct ModuleStringMap {
    uint32_t module_id;               // Module ID
    uint32_t* offset_to_global_id;    // Array mapping module offsets to global IDs
    uint32_t max_offset;              // Maximum offset in this module
    struct ModuleStringMap* next;     // Next module mapping
} ModuleStringMap;

// Global string registry
typedef struct GlobalStringRegistry {
    GlobalStringEntry** hash_table;   // Hash table for string lookup
    uint32_t hash_table_size;         // Size of hash table
    uint32_t next_global_id;          // Next available global string ID
    uint32_t total_strings;           // Total number of strings
    ModuleStringMap* module_maps;     // Module string mappings
    struct VM* vm;                    // Reference to VM
} GlobalStringRegistry;

// String manager (main interface)
typedef struct StringManager {
    GlobalStringRegistry* global_registry;
    bool initialized;
} StringManager;

// ============================================================================
// GLOBAL STRING REGISTRY FUNCTIONS
// ============================================================================

// Create and destroy global string registry
GlobalStringRegistry* global_string_registry_create(struct VM* vm);
void global_string_registry_destroy(GlobalStringRegistry* registry);

// Register a string from a module
uint32_t global_string_registry_register_string(GlobalStringRegistry* registry, 
                                               uint32_t module_id, 
                                               uint32_t module_offset, 
                                               const char* string_data);

// Get global string ID for a module string
uint32_t global_string_registry_get_global_id(GlobalStringRegistry* registry, 
                                             uint32_t module_id, 
                                             uint32_t module_offset);

// Get string data by global ID
const char* global_string_registry_get_string(GlobalStringRegistry* registry, 
                                             uint32_t global_string_id);

// Find string by content (for deduplication)
uint32_t global_string_registry_find_string(GlobalStringRegistry* registry, 
                                           const char* string_data);

// Get module string mapping
ModuleStringMap* global_string_registry_get_module_map(GlobalStringRegistry* registry, 
                                                      uint32_t module_id);

// Print registry statistics
void global_string_registry_print_stats(GlobalStringRegistry* registry);

// Internal function for updating module mappings
void global_string_registry_update_module_mapping(GlobalStringRegistry* registry, 
                                                 uint32_t module_id, 
                                                 uint32_t module_offset, 
                                                 uint32_t global_id);

// ============================================================================
// STRING MANAGER FUNCTIONS
// ============================================================================

// Initialize and cleanup string manager
bool string_manager_init(StringManager* manager, struct VM* vm);
void string_manager_cleanup(StringManager* manager);

// Register a module's strings
bool string_manager_register_module_strings(StringManager* manager, 
                                           uint32_t module_id, 
                                           StringTable* module_string_table);

// Get string data (resolves module offset to global string)
const char* string_manager_get_string(StringManager* manager, 
                                     uint32_t module_id, 
                                     uint32_t module_offset);

// Get global string ID
uint32_t string_manager_get_global_id(StringManager* manager, 
                                     uint32_t module_id, 
                                     uint32_t module_offset);

// Add a new string to the global registry
uint32_t string_manager_add_string(StringManager* manager, 
                                  uint32_t module_id, 
                                  const char* string_data);

// Print string manager statistics
void string_manager_print_stats(StringManager* manager);
