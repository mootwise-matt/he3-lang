#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../objects/object.h"

// Forward declarations
struct BytecodeFile;
struct StringTable;
struct TypeTable;
struct MethodTable;

// Bytecode file header structure
typedef struct {
    uint32_t magic;              // "BX01" magic number
    uint16_t version;            // Bytecode version
    uint16_t flags;              // File flags
    uint32_t domain_name_offset; // Offset to domain name in string table
    uint32_t crowd_list_offset;  // Offset to crowd list
    uint32_t import_count;       // Number of imports
    uint32_t import_table_offset; // Offset to import table
    uint32_t type_count;         // Number of types
    uint32_t type_table_offset;  // Offset to type table
    uint32_t method_count;       // Number of methods
    uint32_t method_table_offset; // Offset to method table
    uint32_t string_table_offset; // Offset to string table
    uint32_t string_table_size;  // Size of string table
    uint32_t bytecode_offset;    // Offset to bytecode instructions
    uint32_t bytecode_size;      // Size of bytecode
} BytecodeHeader;

// String table entry
typedef struct {
    uint32_t offset;             // Offset in string table
    uint32_t length;             // String length
} StringEntry;

// String table
typedef struct StringTable {
    char* data;                  // String data
    size_t size;                 // Total size
    StringEntry* entries;        // String entries
    size_t entry_count;          // Number of entries
} StringTable;

// Type table entry
typedef struct {
    uint32_t name_offset;        // Offset to type name in string table
    uint32_t size;               // Type size in bytes
    uint32_t field_count;        // Number of fields
    uint32_t field_offset;       // Offset to field table
} TypeEntry;

// Type table
typedef struct TypeTable {
    TypeEntry* entries;          // Type entries
    size_t count;                // Number of types
} TypeTable;

// Method table entry
typedef struct {
    uint32_t name_offset;        // Offset to method name in string table
    uint32_t type_index;         // Type index
    uint32_t bytecode_offset;    // Offset to method bytecode
    uint32_t bytecode_size;      // Size of method bytecode
    uint32_t local_count;        // Number of local variables
    uint32_t param_count;        // Number of parameters
} MethodEntry;

// Method table (defined in objects/object.h)

// Complete bytecode file structure
typedef struct BytecodeFile {
    BytecodeHeader header;       // File header
    StringTable* string_table;   // String table
    TypeTable* type_table;       // Type table
    MethodTable* method_table;   // Method table
    uint8_t* bytecode;           // Bytecode instructions
    size_t bytecode_size;        // Size of bytecode
    char* filename;              // Source filename
} BytecodeFile;

// Bytecode loader functions
BytecodeFile* bytecode_load_file(const char* filename);
void bytecode_destroy_file(BytecodeFile* file);

// String table functions
StringTable* string_table_create(void);
void string_table_destroy(StringTable* table);
const char* string_table_get(StringTable* table, uint32_t index);
uint32_t string_table_add(StringTable* table, const char* string);
uint32_t string_table_add_string(StringTable* table, const char* str);
uint32_t string_table_add_string_with_length(StringTable* table, const char* str, size_t length);

// Type table functions
TypeTable* type_table_create(void);
void type_table_destroy(TypeTable* table);
TypeEntry* type_table_get(TypeTable* table, uint32_t index);

// Method table functions
MethodTable* method_table_create(void);
void method_table_destroy(MethodTable* table);
MethodEntry* method_table_get(MethodTable* table, uint32_t index);
MethodEntry* method_table_find_by_name(MethodTable* table, const char* name);

// Utility functions
bool bytecode_validate_header(const BytecodeHeader* header);
const char* bytecode_get_domain_name(BytecodeFile* file);
void bytecode_print_info(BytecodeFile* file);
