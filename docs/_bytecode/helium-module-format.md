---
layout: page
title: helium-module-format
---
# He³ .helium Module Format

## Overview

The `.helium3` file format is the compiled module format for He³ programs. It contains all the metadata and bytecode necessary for the Virtual Machine to load and execute the module at runtime.

## File Structure

```
┌─────────────────────────────────────┐
│            File Header              │
├─────────────────────────────────────┤
│         String Table                │
├─────────────────────────────────────┤
│         Type Table                  │
├─────────────────────────────────────┤
│         Method Table                │
├─────────────────────────────────────┤
│         Field Table                 │
├─────────────────────────────────────┤
│         Interface Table             │
├─────────────────────────────────────┤
│         Import Table                │
├─────────────────────────────────────┤
│         Export Table                │
├─────────────────────────────────────┤
│         Security Table              │
├─────────────────────────────────────┤
│         Debug Information           │
├─────────────────────────────────────┤
│         Bytecode Section            │
└─────────────────────────────────────┘
```

## File Header

```c
// src/vm/bytecode/helium_format.h
typedef struct HeliumHeader {
    // Magic number and version
    char magic[8];           // "HELIUM3"
    uint16_t version_major;  // Major version
    uint16_t version_minor;  // Minor version
    uint32_t flags;          // Module flags
    
    // Module identification
    uint32_t module_name_offset;    // Offset to module name in string table
    uint32_t module_version_offset; // Offset to module version in string table
    
    // Section offsets and sizes
    uint32_t string_table_offset;
    uint32_t string_table_size;
    uint32_t type_table_offset;
    uint32_t type_table_size;
    uint32_t method_table_offset;
    uint32_t method_table_size;
    uint32_t field_table_offset;
    uint32_t field_table_size;
    uint32_t interface_table_offset;
    uint32_t interface_table_size;
    uint32_t import_table_offset;
    uint32_t import_table_size;
    uint32_t export_table_offset;
    uint32_t export_table_size;
    uint32_t security_table_offset;
    uint32_t security_table_size;
    uint32_t debug_info_offset;
    uint32_t debug_info_size;
    uint32_t bytecode_offset;
    uint32_t bytecode_size;
    
    // Module metadata
    uint32_t entry_point;    // Entry point method ID
    uint32_t main_class_id;  // Main class ID (for executables)
    uint32_t dependency_count;
    uint32_t dependency_table_offset;
    
    // Security and permissions
    uint32_t required_permissions;
    uint32_t security_level;
    uint32_t crowd_membership;
    
    // Reserved for future use
    uint32_t reserved[16];
} HeliumHeader;

// Module flags
#define MODULE_FLAG_EXECUTABLE    0x01  // Executable module
#define MODULE_FLAG_LIBRARY       0x02  // Library module
#define MODULE_FLAG_DEBUG         0x04  // Debug information included
#define MODULE_FLAG_OPTIMIZED     0x08  // Optimized bytecode
#define MODULE_FLAG_SECURE        0x10  // Security features enabled
```

## String Table

```c
typedef struct StringTable {
    uint32_t count;          // Number of strings
    uint32_t total_size;     // Total size of all strings
    StringEntry entries[];   // String entries
} StringTable;

typedef struct StringEntry {
    uint32_t offset;         // Offset in string data
    uint32_t length;         // String length in bytes
    uint32_t hash;           // String hash for fast lookup
} StringEntry;

// String data follows the string table
// All strings are UTF-8 encoded and null-terminated
```

## Type Table

```c
typedef struct TypeTable {
    uint32_t count;          // Number of types
    TypeEntry entries[];     // Type entries
} TypeTable;

typedef struct TypeEntry {
    uint32_t type_id;        // Unique type identifier
    uint32_t name_offset;    // Offset to type name in string table
    uint32_t parent_type_id; // Parent type ID (0 for System.Object)
    uint32_t size;           // Instance size in bytes
    uint32_t field_count;    // Number of fields
    uint32_t method_count;   // Number of methods
    uint32_t interface_count; // Number of implemented interfaces
    uint32_t flags;          // Type flags
    uint32_t vtable_offset;  // Offset to vtable in method table
} TypeEntry;

// Type flags
#define TYPE_FLAG_CLASS        0x01  // Class type
#define TYPE_FLAG_INTERFACE    0x02  // Interface type
#define TYPE_FLAG_RECORD       0x04  // Record type
#define TYPE_FLAG_ENUM         0x08  // Enum type
#define TYPE_FLAG_ABSTRACT     0x10  // Abstract type
#define TYPE_FLAG_FINAL        0x20  // Final type
#define TYPE_FLAG_GENERIC      0x40  // Generic type
```

## Method Table

```c
typedef struct MethodTable {
    uint32_t count;          // Number of methods
    MethodEntry entries[];   // Method entries
} MethodTable;

typedef struct MethodEntry {
    uint32_t method_id;      // Unique method identifier
    uint32_t type_id;        // Owning type ID
    uint32_t name_offset;    // Offset to method name in string table
    uint32_t signature_offset; // Offset to signature in string table
    uint32_t bytecode_offset; // Offset to bytecode
    uint32_t bytecode_size;  // Size of bytecode
    uint32_t local_count;    // Number of local variables
    uint32_t param_count;    // Number of parameters
    uint32_t return_type_id; // Return type ID
    uint32_t flags;          // Method flags
    uint32_t line_number;    // Source line number
    uint32_t column_number;  // Source column number
} MethodEntry;

// Method flags
#define METHOD_FLAG_STATIC     0x01  // Static method
#define METHOD_FLAG_VIRTUAL    0x02  // Virtual method
#define METHOD_FLAG_ABSTRACT   0x04  // Abstract method
#define METHOD_FLAG_FINAL      0x08  // Final method
#define METHOD_FLAG_ASYNC      0x10  // Async method
#define METHOD_FLAG_CONSTRUCTOR 0x20 // Constructor
#define METHOD_FLAG_PROPERTY   0x40  // Property accessor
```

## Field Table

```c
typedef struct FieldTable {
    uint32_t count;          // Number of fields
    FieldEntry entries[];    // Field entries
} FieldTable;

typedef struct FieldEntry {
    uint32_t field_id;       // Unique field identifier
    uint32_t type_id;        // Owning type ID
    uint32_t name_offset;    // Offset to field name in string table
    uint32_t field_type_id;  // Field type ID
    uint32_t offset;         // Offset in object data
    uint32_t flags;          // Field flags
    uint32_t default_value_offset; // Offset to default value
} FieldEntry;

// Field flags
#define FIELD_FLAG_STATIC     0x01  // Static field
#define FIELD_FLAG_READONLY   0x02  // Read-only field
#define FIELD_FLAG_VOLATILE   0x04  // Volatile field
#define FIELD_FLAG_CONST      0x08  // Constant field
#define FIELD_FLAG_PUBLIC     0x10  // Public field
#define FIELD_FLAG_PRIVATE    0x20  // Private field
#define FIELD_FLAG_PROTECTED  0x40  // Protected field
```

## Interface Table

```c
typedef struct InterfaceTable {
    uint32_t count;          // Number of interfaces
    InterfaceEntry entries[]; // Interface entries
} InterfaceTable;

typedef struct InterfaceEntry {
    uint32_t interface_id;   // Unique interface identifier
    uint32_t name_offset;    // Offset to interface name in string table
    uint32_t parent_count;   // Number of parent interfaces
    uint32_t method_count;   // Number of methods
    uint32_t flags;          // Interface flags
} InterfaceEntry;
```

## Import Table

```c
typedef struct ImportTable {
    uint32_t count;          // Number of imports
    ImportEntry entries[];   // Import entries
} ImportTable;

typedef struct ImportEntry {
    uint32_t module_name_offset; // Offset to module name in string table
    uint32_t symbol_name_offset; // Offset to symbol name in string table
    uint32_t import_type;    // Type of import (type, method, field, etc.)
    uint32_t target_id;      // Target ID in this module
    uint32_t flags;          // Import flags
} ImportEntry;

// Import types
#define IMPORT_TYPE_TYPE      0x01  // Type import
#define IMPORT_TYPE_METHOD    0x02  // Method import
#define IMPORT_TYPE_FIELD     0x03  // Field import
#define IMPORT_TYPE_CONSTANT  0x04  // Constant import
```

## Export Table

```c
typedef struct ExportTable {
    uint32_t count;          // Number of exports
    ExportEntry entries[];   // Export entries
} ExportTable;

typedef struct ExportEntry {
    uint32_t symbol_id;      // Symbol ID (type, method, or field)
    uint32_t name_offset;    // Offset to export name in string table
    uint32_t export_type;    // Type of export
    uint32_t flags;          // Export flags
} ExportEntry;
```

## Security Table

```c
typedef struct SecurityTable {
    uint32_t count;          // Number of security entries
    SecurityEntry entries[]; // Security entries
} SecurityTable;

typedef struct SecurityEntry {
    uint32_t type;           // Security type
    uint32_t data_offset;    // Offset to security data
    uint32_t data_size;      // Size of security data
    uint32_t flags;          // Security flags
} SecurityEntry;

// Security types
#define SECURITY_TYPE_DOMAIN_KEY    0x01  // Domain security key
#define SECURITY_TYPE_CROWD_TOKEN   0x02  // Crowd membership token
#define SECURITY_TYPE_PERMISSION    0x03  // Permission requirement
#define SECURITY_TYPE_SIGNATURE     0x04  // Digital signature
```

## Debug Information

```c
typedef struct DebugInfo {
    uint32_t source_file_offset; // Offset to source file name
    uint32_t line_count;         // Number of line entries
    LineEntry lines[];           // Line number entries
} DebugInfo;

typedef struct LineEntry {
    uint32_t bytecode_offset;    // Offset in bytecode
    uint32_t line_number;        // Source line number
    uint32_t column_number;      // Source column number
    uint32_t file_id;            // Source file ID
} LineEntry;
```

## Bytecode Section

```c
typedef struct BytecodeSection {
    uint32_t method_count;   // Number of methods with bytecode
    MethodBytecode methods[]; // Method bytecode entries
} BytecodeSection;

typedef struct MethodBytecode {
    uint32_t method_id;      // Method ID
    uint32_t bytecode_size;  // Size of bytecode
    uint8_t bytecode[];      // Actual bytecode
} MethodBytecode;
```

## Module Loading Process

### 1. Header Validation

```c
bool validate_helium_header(const HeliumHeader* header) {
    // Check magic number
    if (memcmp(header->magic, "HELIUM3", 8) != 0) {
        return false;
    }
    
    // Check version compatibility
    if (header->version_major > CURRENT_VERSION_MAJOR) {
        return false;
    }
    
    // Check required flags
    if (!(header->flags & MODULE_FLAG_EXECUTABLE) && 
        !(header->flags & MODULE_FLAG_LIBRARY)) {
        return false;
    }
    
    return true;
}
```

### 2. String Table Loading

```c
StringTable* load_string_table(const uint8_t* data, const HeliumHeader* header) {
    StringTable* table = (StringTable*)(data + header->string_table_offset);
    
    // Validate string table
    if (table->count == 0 || table->total_size == 0) {
        return NULL;
    }
    
    return table;
}
```

### 3. Type Table Loading

```c
TypeTable* load_type_table(const uint8_t* data, const HeliumHeader* header) {
    TypeTable* table = (TypeTable*)(data + header->type_table_offset);
    
    // Validate type table
    if (table->count == 0) {
        return NULL;
    }
    
    // Build type lookup table
    for (uint32_t i = 0; i < table->count; i++) {
        register_type(&table->entries[i]);
    }
    
    return table;
}
```

### 4. Method Table Loading

```c
MethodTable* load_method_table(const uint8_t* data, const HeliumHeader* header) {
    MethodTable* table = (MethodTable*)(data + header->method_table_offset);
    
    // Validate method table
    if (table->count == 0) {
        return NULL;
    }
    
    // Build method lookup table
    for (uint32_t i = 0; i < table->count; i++) {
        register_method(&table->entries[i]);
    }
    
    return table;
}
```

## Module Dependencies

### Dependency Resolution

```c
typedef struct DependencyEntry {
    uint32_t module_name_offset; // Offset to module name
    uint32_t version_offset;     // Offset to version string
    uint32_t required_flags;     // Required module flags
    uint32_t load_order;         // Load order priority
} DependencyEntry;

bool resolve_dependencies(const HeliumHeader* header) {
    DependencyEntry* deps = (DependencyEntry*)((uint8_t*)header + header->dependency_table_offset);
    
    for (uint32_t i = 0; i < header->dependency_count; i++) {
        const char* module_name = get_string(header, deps[i].module_name_offset);
        const char* version = get_string(header, deps[i].version_offset);
        
        if (!load_dependency_module(module_name, version)) {
            return false;
        }
    }
    
    return true;
}
```

## Security and Permissions

### Permission Checking

```c
bool check_module_permissions(const HeliumHeader* header) {
    // Check required permissions
    if (!has_permissions(header->required_permissions)) {
        return false;
    }
    
    // Check security level
    if (header->security_level > get_max_security_level()) {
        return false;
    }
    
    // Check crowd membership
    if (!is_crowd_member(header->crowd_membership)) {
        return false;
    }
    
    return true;
}
```

## File Format Validation

### Complete Validation

```c
bool validate_helium_module(const uint8_t* data, size_t size) {
    if (size < sizeof(HeliumHeader)) {
        return false;
    }
    
    const HeliumHeader* header = (const HeliumHeader*)data;
    
    // Validate header
    if (!validate_helium_header(header)) {
        return false;
    }
    
    // Validate section offsets and sizes
    if (!validate_section_offsets(header, size)) {
        return false;
    }
    
    // Validate string table
    if (!validate_string_table(data, header)) {
        return false;
    }
    
    // Validate type table
    if (!validate_type_table(data, header)) {
        return false;
    }
    
    // Validate method table
    if (!validate_method_table(data, header)) {
        return false;
    }
    
    // Validate bytecode
    if (!validate_bytecode(data, header)) {
        return false;
    }
    
    return true;
}
```

This comprehensive .helium module format provides all the metadata and bytecode necessary for the Virtual Machine to efficiently load and execute He³ modules at runtime.
