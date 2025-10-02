#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ============================================================================
// SHARED BYTECODE FORMAT
// ============================================================================
// This header defines the shared bytecode format used by both the compiler
// and the virtual machine. It ensures compatibility between compilation
// and execution phases.

// ============================================================================
// BYTECODE FILE HEADER
// ============================================================================

#define BYTECODE_MAGIC "BX01"
#define BYTECODE_MAGIC_SIZE 4
#define BYTECODE_VERSION_MAJOR 1
#define BYTECODE_VERSION_MINOR 0

// Bytecode file header structure
typedef struct {
    // Magic number and version
    char magic[4];              // "BX01" magic number
    uint16_t version_major;     // Major version
    uint16_t version_minor;     // Minor version
    uint32_t flags;             // File flags
    
    // Module identification
    uint32_t domain_name_offset; // Offset to domain name in string table
    uint32_t module_name_offset; // Offset to module name in string table
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
    uint32_t bytecode_offset;
    uint32_t bytecode_size;
    
    // Entry point and metadata
    uint32_t entry_point_method_id; // Entry point method ID
    uint32_t main_class_id;         // Main class ID (for executables)
    uint32_t dependency_count;      // Number of dependencies
    uint32_t dependency_table_offset; // Offset to dependency table
    
    // Reserved for future use
    uint32_t reserved[8];
} BytecodeHeader;

// File flags
#define BYTECODE_FLAG_EXECUTABLE    0x01  // Executable bytecode
#define BYTECODE_FLAG_LIBRARY       0x02  // Library bytecode
#define BYTECODE_FLAG_DEBUG         0x04  // Debug information included
#define BYTECODE_FLAG_OPTIMIZED     0x08  // Optimized bytecode
#define BYTECODE_FLAG_SECURE        0x10  // Security features enabled

// ============================================================================
// STRING TABLE
// ============================================================================

// String table entry
typedef struct {
    uint32_t offset;             // Offset in string data
    uint32_t length;             // String length in bytes
    uint32_t hash;               // String hash for fast lookup
} StringEntry;

// String table
typedef struct {
    uint32_t count;              // Number of strings
    uint32_t total_size;         // Total size of all strings
    StringEntry* entries;        // String entries
    char* data;                  // String data (UTF-8, null-terminated)
} StringTable;

// ============================================================================
// TYPE TABLE
// ============================================================================

// Type table entry
typedef struct {
    uint32_t type_id;            // Unique type identifier
    uint32_t name_offset;        // Offset to type name in string table
    uint32_t parent_type_id;     // Parent type ID (0 for System.Object)
    uint32_t size;               // Instance size in bytes
    uint32_t field_count;        // Number of fields
    uint32_t method_count;       // Number of methods
    uint32_t interface_count;    // Number of implemented interfaces
    uint32_t flags;              // Type flags
    uint32_t vtable_offset;      // Offset to vtable in method table
} TypeEntry;

// Type table
typedef struct {
    uint32_t count;              // Number of types
    TypeEntry* entries;          // Type entries
} TypeTable;

// Type flags
#define TYPE_FLAG_CLASS        0x01  // Class type
#define TYPE_FLAG_INTERFACE    0x02  // Interface type
#define TYPE_FLAG_RECORD       0x04  // Record type
#define TYPE_FLAG_ENUM         0x08  // Enum type
#define TYPE_FLAG_ABSTRACT     0x10  // Abstract type
#define TYPE_FLAG_FINAL        0x20  // Final type
#define TYPE_FLAG_GENERIC      0x40  // Generic type

// ============================================================================
// METHOD TABLE
// ============================================================================

// Method table entry
typedef struct {
    uint32_t method_id;          // Unique method identifier
    uint32_t type_id;            // Owning type ID
    uint32_t name_offset;        // Offset to method name in string table
    uint32_t signature_offset;   // Offset to signature in string table
    uint32_t bytecode_offset;    // Offset to bytecode
    uint32_t bytecode_size;      // Size of bytecode
    uint32_t local_count;        // Number of local variables
    uint32_t param_count;        // Number of parameters
    uint32_t return_type_id;     // Return type ID
    uint32_t flags;              // Method flags
    uint32_t line_number;        // Source line number
    uint32_t column_number;      // Source column number
} MethodEntry;

// Method table
typedef struct {
    uint32_t count;              // Number of methods
    MethodEntry* entries;        // Method entries
} MethodTable;

// Method flags
#define METHOD_FLAG_STATIC     0x01  // Static method
#define METHOD_FLAG_VIRTUAL    0x02  // Virtual method
#define METHOD_FLAG_ABSTRACT   0x04  // Abstract method
#define METHOD_FLAG_FINAL      0x08  // Final method
#define METHOD_FLAG_ASYNC      0x10  // Async method
#define METHOD_FLAG_CONSTRUCTOR 0x20 // Constructor
#define METHOD_FLAG_PROPERTY   0x40  // Property accessor

// ============================================================================
// FIELD TABLE
// ============================================================================

// Field table entry
typedef struct {
    uint32_t field_id;           // Unique field identifier
    uint32_t type_id;            // Owning type ID
    uint32_t name_offset;        // Offset to field name in string table
    uint32_t field_type_id;      // Field type ID
    uint32_t offset;             // Offset in object data
    uint32_t flags;              // Field flags
    uint32_t default_value_offset; // Offset to default value
} FieldEntry;

// Field table
typedef struct {
    uint32_t count;              // Number of fields
    FieldEntry* entries;         // Field entries
} FieldTable;

// Field flags
#define FIELD_FLAG_STATIC     0x01  // Static field
#define FIELD_FLAG_READONLY   0x02  // Read-only field
#define FIELD_FLAG_VOLATILE   0x04  // Volatile field
#define FIELD_FLAG_CONST      0x08  // Constant field
#define FIELD_FLAG_PUBLIC     0x10  // Public field
#define FIELD_FLAG_PRIVATE    0x20  // Private field
#define FIELD_FLAG_PROTECTED  0x40  // Protected field

// ============================================================================
// DEPENDENCY TABLE
// ============================================================================

// Dependency entry
typedef struct {
    uint32_t module_name_offset; // Offset to module name in string table
    uint32_t version_offset;     // Offset to version string in string table
    uint32_t required_flags;     // Required module flags
    uint32_t load_order;         // Load order priority
} DependencyEntry;

// Dependency table
typedef struct {
    uint32_t count;              // Number of dependencies
    DependencyEntry* entries;    // Dependency entries
} DependencyTable;

// ============================================================================
// BYTECODE FILE STRUCTURE
// ============================================================================

// Complete bytecode file structure
typedef struct {
    BytecodeHeader header;       // File header
    StringTable* string_table;   // String table
    TypeTable* type_table;       // Type table
    MethodTable* method_table;   // Method table
    FieldTable* field_table;     // Field table
    DependencyTable* dependency_table; // Dependency table
    uint8_t* bytecode;           // Bytecode instructions
    size_t bytecode_size;        // Size of bytecode
} BytecodeFile;

// ============================================================================
// BYTECODE FILE OPERATIONS
// ============================================================================

// Create a new bytecode file
BytecodeFile* bytecode_file_create(void);

// Destroy a bytecode file
void bytecode_file_destroy(BytecodeFile* file);

// Load bytecode file from disk
BytecodeFile* bytecode_file_load(const char* filename);

// Save bytecode file to disk
bool bytecode_file_save(BytecodeFile* file, const char* filename);

// Validate bytecode file
bool bytecode_file_validate(BytecodeFile* file);

// ============================================================================
// STRING TABLE OPERATIONS
// ============================================================================

// Create string table
StringTable* string_table_create(void);

// Destroy string table
void string_table_destroy(StringTable* table);

// Add string to table
uint32_t string_table_add_string(StringTable* table, const char* str);

// Get string from table
const char* string_table_get_string(StringTable* table, uint32_t index);

// Find string in table
int32_t string_table_find_string(StringTable* table, const char* str);

// ============================================================================
// TYPE TABLE OPERATIONS
// ============================================================================

// Create type table
TypeTable* type_table_create(void);

// Destroy type table
void type_table_destroy(TypeTable* table);

// Add type to table
bool type_table_add_type(TypeTable* table, const TypeEntry* entry);

// Get type from table
const TypeEntry* type_table_get_type(TypeTable* table, uint32_t type_id);

// Find type by name
int32_t type_table_find_type_by_name(TypeTable* table, const char* name);

// ============================================================================
// METHOD TABLE OPERATIONS
// ============================================================================

// Create method table
MethodTable* method_table_create(void);

// Destroy method table
void method_table_destroy(MethodTable* table);

// Add method to table
bool method_table_add_method(MethodTable* table, const MethodEntry* entry);

// Get method from table
const MethodEntry* method_table_get_method(MethodTable* table, uint32_t method_id);

// Find method by name and type
int32_t method_table_find_method(MethodTable* table, uint32_t type_id, const char* name);

// ============================================================================
// FIELD TABLE OPERATIONS
// ============================================================================

// Create field table
FieldTable* field_table_create(void);

// Destroy field table
void field_table_destroy(FieldTable* table);

// Add field to table
bool field_table_add_field(FieldTable* table, const FieldEntry* entry);

// Get field from table
const FieldEntry* field_table_get_field(FieldTable* table, uint32_t field_id);

// Find field by name and type
int32_t field_table_find_field(FieldTable* table, uint32_t type_id, const char* name);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Calculate string hash
uint32_t string_hash(const char* str);

// Align size to boundary
size_t align_size(size_t size, size_t alignment);

// Validate magic number
bool validate_magic(const char* magic);

// Get string from bytecode file
const char* bytecode_get_string(BytecodeFile* file, uint32_t offset);

// Get type from bytecode file
const TypeEntry* bytecode_get_type(BytecodeFile* file, uint32_t type_id);

// Get method from bytecode file
const MethodEntry* bytecode_get_method(BytecodeFile* file, uint32_t method_id);

// Get field from bytecode file
const FieldEntry* bytecode_get_field(BytecodeFile* file, uint32_t field_id);

// ============================================================================
// DEBUGGING AND DISASSEMBLY
// ============================================================================

// Print bytecode file information
void bytecode_file_print_info(BytecodeFile* file);

// Print string table
void string_table_print(StringTable* table);

// Print type table
void type_table_print(TypeTable* table);

// Print method table
void method_table_print(MethodTable* table);

// Print field table
void field_table_print(FieldTable* table);

// Disassemble bytecode
void bytecode_disassemble(BytecodeFile* file, uint32_t method_id);

// ============================================================================
// CONSTANTS
// ============================================================================

// Built-in type IDs
#define TYPE_ID_OBJECT    1
#define TYPE_ID_STRING    2
#define TYPE_ID_INT64     3
#define TYPE_ID_FLOAT64   4
#define TYPE_ID_BOOLEAN   5
#define TYPE_ID_VOID      6

// Built-in method IDs
#define METHOD_ID_MAIN    1
#define METHOD_ID_PRINT   2

// Built-in field IDs
#define FIELD_ID_NONE     0

// Maximum values
#define MAX_STRING_LENGTH     65535
#define MAX_TYPE_COUNT        65535
#define MAX_METHOD_COUNT      65535
#define MAX_FIELD_COUNT       65535
#define MAX_BYTECODE_SIZE     16777216  // 16MB
#define MAX_STRING_TABLE_SIZE 16777216  // 16MB

// Alignment
#define BYTECODE_ALIGNMENT    8
#define STRING_ALIGNMENT      1
#define TYPE_ALIGNMENT        8
#define METHOD_ALIGNMENT      8
#define FIELD_ALIGNMENT       8