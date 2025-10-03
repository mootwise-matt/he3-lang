#include "global_string_registry.h"
#include "../../vm/vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// GLOBAL STRING REGISTRY IMPLEMENTATION
// ============================================================================

// Hash function for strings
static uint32_t global_string_hash(const char* str) {
    if (!str) return 0;
    
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

// Create global string registry
GlobalStringRegistry* global_string_registry_create(struct VM* vm) {
    GlobalStringRegistry* registry = malloc(sizeof(GlobalStringRegistry));
    if (!registry) return NULL;
    
    registry->hash_table_size = 1024; // Start with 1024 buckets
    registry->hash_table = calloc(registry->hash_table_size, sizeof(GlobalStringEntry*));
    if (!registry->hash_table) {
        free(registry);
        return NULL;
    }
    
    registry->next_global_id = 1; // Start from 1 (0 is reserved for errors)
    registry->total_strings = 0;
    registry->module_maps = NULL;
    registry->vm = vm;
    
    return registry;
}

// Destroy global string registry
void global_string_registry_destroy(GlobalStringRegistry* registry) {
    if (!registry) return;
    
    // Free hash table entries
    for (uint32_t i = 0; i < registry->hash_table_size; i++) {
        GlobalStringEntry* entry = registry->hash_table[i];
        while (entry) {
            GlobalStringEntry* next = entry->next;
            if (entry->string_data) {
                free(entry->string_data);
            }
            free(entry);
            entry = next;
        }
    }
    
    free(registry->hash_table);
    
    // Free module maps
    ModuleStringMap* map = registry->module_maps;
    while (map) {
        ModuleStringMap* next = map->next;
        if (map->offset_to_global_id) {
            free(map->offset_to_global_id);
        }
        free(map);
        map = next;
    }
    
    free(registry);
}

// Register a string from a module
uint32_t global_string_registry_register_string(GlobalStringRegistry* registry, 
                                               uint32_t module_id, 
                                               uint32_t module_offset, 
                                               const char* string_data) {
    if (!registry || !string_data) return 0;
    
    // Check if string already exists
    uint32_t existing_id = global_string_registry_find_string(registry, string_data);
    if (existing_id != 0) {
        // String already exists, just update module mapping
        global_string_registry_update_module_mapping(registry, module_id, module_offset, existing_id);
        return existing_id;
    }
    
    // Create new global string entry
    GlobalStringEntry* entry = malloc(sizeof(GlobalStringEntry));
    if (!entry) return 0;
    
    entry->global_string_id = registry->next_global_id++;
    entry->module_id = module_id;
    entry->module_string_offset = module_offset;
    entry->string_data = strdup(string_data);
    entry->hash = global_string_hash(string_data);
    entry->next = NULL;
    
    if (!entry->string_data) {
        free(entry);
        return 0;
    }
    
    // Add to hash table
    uint32_t bucket = entry->hash % registry->hash_table_size;
    entry->next = registry->hash_table[bucket];
    registry->hash_table[bucket] = entry;
    
    registry->total_strings++;
    
    // Update module mapping
    global_string_registry_update_module_mapping(registry, module_id, module_offset, entry->global_string_id);
    
    return entry->global_string_id;
}

// Update module mapping
void global_string_registry_update_module_mapping(GlobalStringRegistry* registry, 
                                                 uint32_t module_id, 
                                                 uint32_t module_offset, 
                                                 uint32_t global_id) {
    // Find or create module map
    ModuleStringMap* map = global_string_registry_get_module_map(registry, module_id);
    if (!map) {
        map = malloc(sizeof(ModuleStringMap));
        if (!map) return;
        
        map->module_id = module_id;
        map->offset_to_global_id = NULL;
        map->max_offset = 0;
        map->next = registry->module_maps;
        registry->module_maps = map;
    }
    
    // Resize offset array if needed
    if (module_offset >= map->max_offset) {
        uint32_t new_size = module_offset + 1;
        uint32_t* new_array = realloc(map->offset_to_global_id, new_size * sizeof(uint32_t));
        if (!new_array) return;
        
        // Initialize new entries to 0
        for (uint32_t i = map->max_offset; i < new_size; i++) {
            new_array[i] = 0;
        }
        
        map->offset_to_global_id = new_array;
        map->max_offset = new_size;
    }
    
    map->offset_to_global_id[module_offset] = global_id;
}

// Get global string ID for a module string
uint32_t global_string_registry_get_global_id(GlobalStringRegistry* registry, 
                                             uint32_t module_id, 
                                             uint32_t module_offset) {
    if (!registry) return 0;
    
    ModuleStringMap* map = global_string_registry_get_module_map(registry, module_id);
    if (!map || module_offset >= map->max_offset) return 0;
    
    return map->offset_to_global_id[module_offset];
}

// Get string data by global ID
const char* global_string_registry_get_string(GlobalStringRegistry* registry, 
                                             uint32_t global_string_id) {
    if (!registry || global_string_id == 0) return NULL;
    
    // Search through all hash table buckets
    for (uint32_t i = 0; i < registry->hash_table_size; i++) {
        GlobalStringEntry* entry = registry->hash_table[i];
        while (entry) {
            if (entry->global_string_id == global_string_id) {
                return entry->string_data;
            }
            entry = entry->next;
        }
    }
    
    return NULL;
}

// Find string by content (for deduplication)
uint32_t global_string_registry_find_string(GlobalStringRegistry* registry, 
                                           const char* string_data) {
    if (!registry || !string_data) return 0;
    
    uint32_t hash = global_string_hash(string_data);
    uint32_t bucket = hash % registry->hash_table_size;
    
    GlobalStringEntry* entry = registry->hash_table[bucket];
    while (entry) {
        if (entry->hash == hash && strcmp(entry->string_data, string_data) == 0) {
            return entry->global_string_id;
        }
        entry = entry->next;
    }
    
    return 0;
}

// Get module string mapping
ModuleStringMap* global_string_registry_get_module_map(GlobalStringRegistry* registry, 
                                                      uint32_t module_id) {
    if (!registry) return NULL;
    
    ModuleStringMap* map = registry->module_maps;
    while (map) {
        if (map->module_id == module_id) {
            return map;
        }
        map = map->next;
    }
    
    return NULL;
}

// Print registry statistics
void global_string_registry_print_stats(GlobalStringRegistry* registry) {
    if (!registry) {
        printf("Global String Registry: NULL\n");
        return;
    }
    
    printf("=== Global String Registry Statistics ===\n");
    printf("Total strings: %u\n", registry->total_strings);
    printf("Hash table size: %u\n", registry->hash_table_size);
    printf("Next global ID: %u\n", registry->next_global_id);
    
    // Count module mappings
    uint32_t module_count = 0;
    ModuleStringMap* map = registry->module_maps;
    while (map) {
        module_count++;
        map = map->next;
    }
    printf("Module mappings: %u\n", module_count);
    
    // Print hash table distribution
    uint32_t empty_buckets = 0;
    uint32_t max_chain_length = 0;
    for (uint32_t i = 0; i < registry->hash_table_size; i++) {
        uint32_t chain_length = 0;
        GlobalStringEntry* entry = registry->hash_table[i];
        while (entry) {
            chain_length++;
            entry = entry->next;
        }
        
        if (chain_length == 0) {
            empty_buckets++;
        }
        if (chain_length > max_chain_length) {
            max_chain_length = chain_length;
        }
    }
    
    printf("Empty buckets: %u (%.1f%%)\n", empty_buckets, 
           (float)empty_buckets / registry->hash_table_size * 100);
    printf("Max chain length: %u\n", max_chain_length);
}

// ============================================================================
// STRING MANAGER IMPLEMENTATION
// ============================================================================

// Initialize string manager
bool string_manager_init(StringManager* manager, struct VM* vm) {
    if (!manager || !vm) return false;
    
    manager->global_registry = global_string_registry_create(vm);
    if (!manager->global_registry) return false;
    
    manager->initialized = true;
    return true;
}

// Cleanup string manager
void string_manager_cleanup(StringManager* manager) {
    if (!manager) return;
    
    if (manager->global_registry) {
        global_string_registry_destroy(manager->global_registry);
        manager->global_registry = NULL;
    }
    
    manager->initialized = false;
}

// Register a module's strings
bool string_manager_register_module_strings(StringManager* manager, 
                                           uint32_t module_id, 
                                           StringTable* module_string_table) {
    if (!manager || !manager->initialized || !module_string_table) return false;
    
    // Register all strings from the module
    for (uint32_t i = 0; i < module_string_table->count; i++) {
        const char* string_data = module_string_table->data + module_string_table->entries[i].offset;
        uint32_t module_offset = module_string_table->entries[i].offset;
        
        global_string_registry_register_string(manager->global_registry, 
                                             module_id, 
                                             module_offset, 
                                             string_data);
    }
    
    return true;
}

// Get string data (resolves module offset to global string)
const char* string_manager_get_string(StringManager* manager, 
                                     uint32_t module_id, 
                                     uint32_t module_offset) {
    if (!manager || !manager->initialized) return NULL;
    
    uint32_t global_id = global_string_registry_get_global_id(manager->global_registry, 
                                                            module_id, 
                                                            module_offset);
    if (global_id == 0) return NULL;
    
    return global_string_registry_get_string(manager->global_registry, global_id);
}

// Get global string ID
uint32_t string_manager_get_global_id(StringManager* manager, 
                                     uint32_t module_id, 
                                     uint32_t module_offset) {
    if (!manager || !manager->initialized) return 0;
    
    return global_string_registry_get_global_id(manager->global_registry, 
                                              module_id, 
                                              module_offset);
}

// Add a new string to the global registry
uint32_t string_manager_add_string(StringManager* manager, 
                                  uint32_t module_id, 
                                  const char* string_data) {
    if (!manager || !manager->initialized || !string_data) return 0;
    
    // Find a free offset in the module (this is a simplified approach)
    // In a real implementation, we'd need to track the module's string table
    uint32_t module_offset = 0; // Placeholder
    
    return global_string_registry_register_string(manager->global_registry, 
                                                module_id, 
                                                module_offset, 
                                                string_data);
}

// Print string manager statistics
void string_manager_print_stats(StringManager* manager) {
    if (!manager || !manager->initialized) {
        printf("String Manager: Not initialized\n");
        return;
    }
    
    global_string_registry_print_stats(manager->global_registry);
}
