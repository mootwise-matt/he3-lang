#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "bytecode_format.h"  // Reuse existing structures

// Helium3 module format definitions

// Magic number for .helium3 files
#define HELIUM_MAGIC "HELIUM3"
#define HELIUM_MAGIC_SIZE 8

// Version information
#define HELIUM_VERSION_MAJOR 1
#define HELIUM_VERSION_MINOR 0

// Module flags
#define HELIUM_FLAG_EXECUTABLE    0x0001  // Module contains executable code
#define HELIUM_FLAG_LIBRARY       0x0002  // Module is a library
#define HELIUM_FLAG_DEBUG         0x0004  // Module contains debug information
#define HELIUM_FLAG_OPTIMIZED     0x0008  // Module is optimized

// Helium3 module header
typedef struct HeliumHeader {
    // Magic number and version
    char magic[8];                    // "HELIUM3"
    uint16_t version_major;           // Major version
    uint16_t version_minor;           // Minor version
    uint32_t flags;                   // Module flags
    
    // Module identification
    uint32_t module_name_offset;      // Offset to module name in string table
    uint32_t module_version_offset;   // Offset to module version in string table
    
    // Section offsets and sizes
    uint32_t string_table_offset;     // Offset to string table
    uint32_t string_table_size;       // Size of string table
    uint32_t type_table_offset;       // Offset to type table
    uint32_t type_table_size;         // Size of type table
    uint32_t method_table_offset;     // Offset to method table
    uint32_t method_table_size;       // Size of method table
    uint32_t field_table_offset;      // Offset to field table
    uint32_t field_table_size;        // Size of field table
    uint32_t bytecode_offset;         // Offset to bytecode section
    uint32_t bytecode_size;           // Size of bytecode section
    
    // Entry point
    uint32_t entry_point_method_id;   // Method ID of entry point
    
    // Reserved for future use
    uint32_t reserved[8];
} HeliumHeader;

// Helium3 module structure
typedef struct HeliumModule {
    HeliumHeader header;              // Module header
    
    // String table
    char* string_table;               // String table data
    uint32_t string_table_size;       // Size of string table
    
    // Tables (reuse from bytecode_format.h)
    StringTable* string_table_obj;    // String table object
    TypeTable* type_table;            // Type table
    MethodTable* method_table;        // Method table
    FieldTable* field_table;          // Field table
    
    // Bytecode
    uint8_t* bytecode;                // Bytecode data
    uint32_t bytecode_size;           // Size of bytecode
} HeliumModule;

// Function declarations
HeliumModule* helium_module_create(void);
void helium_module_destroy(HeliumModule* module);
bool helium_module_save(HeliumModule* module, const char* filename);
HeliumModule* helium_module_load(const char* filename);
bool helium_module_validate(HeliumModule* module);
const char* helium_module_get_string(HeliumModule* module, uint32_t offset);
uint32_t helium_module_add_string(HeliumModule* module, const char* str);
void helium_module_print_info(HeliumModule* module);
