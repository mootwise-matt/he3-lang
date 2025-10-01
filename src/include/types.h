#pragma once
#include <stdint.h>
#include <stdbool.h>

// He³ Type System
// This header defines the type system for the He³ compiler

// Type kinds
typedef enum {
    TYPE_PRIMITIVE = 0,
    TYPE_CLASS = 1,
    TYPE_RECORD = 2,
    TYPE_ENUM = 3,
    TYPE_INTERFACE = 4,
    TYPE_OPTION = 5,
    TYPE_RESULT = 6,
    TYPE_TASK = 7,
    TYPE_ARRAY = 8,
    TYPE_FUNCTION = 9
} TypeKind;

// Primitive types
typedef enum {
    PRIM_INTEGER = 0,
    PRIM_FLOAT = 1,
    PRIM_BOOLEAN = 2,
    PRIM_STRING = 3,
    PRIM_VOID = 4,
    PRIM_OBJECT = 5
} PrimitiveType;

// Type flags
#define TYPE_FLAG_NULLABLE 0x01
#define TYPE_FLAG_GENERIC 0x02
#define TYPE_FLAG_SECURE 0x04
#define TYPE_FLAG_ASYNC 0x08

// Type entry in type table
typedef struct TypeEntry TypeEntry;
struct TypeEntry {
    const char* name;           // Type name
    TypeKind kind;              // Type kind
    PrimitiveType primitive;    // For primitive types
    uint16_t flags;             // Type flags
    uint32_t size;              // Size in bytes
    uint32_t parent;            // Parent type index
    uint32_t method_start;      // First method index
    uint32_t method_count;      // Number of methods
    uint32_t field_start;       // First field index
    uint32_t field_count;       // Number of fields
    
    // Generic type parameters
    TypeEntry** generic_args;
    uint32_t generic_count;
    
    // For function types
    TypeEntry** param_types;
    uint32_t param_count;
    TypeEntry* return_type;
    
    // For array types
    TypeEntry* element_type;
    uint32_t array_size;        // 0 for dynamic arrays
};

// Type table
typedef struct {
    TypeEntry** entries;
    uint32_t count;
    uint32_t capacity;
} TypeTable;

// Built-in types
extern TypeEntry* TYPE_INTEGER;
extern TypeEntry* TYPE_FLOAT;
extern TypeEntry* TYPE_BOOLEAN;
extern TypeEntry* TYPE_STRING;
extern TypeEntry* TYPE_VOID;
extern TypeEntry* TYPE_OBJECT;

// Type table functions
TypeTable* type_table_create(void);
void type_table_destroy(TypeTable* table);
TypeEntry* type_table_add(TypeTable* table, const char* name, TypeKind kind);
TypeEntry* type_table_find(TypeTable* table, const char* name);
TypeEntry* type_table_get(TypeTable* table, uint32_t index);

// Type creation functions
TypeEntry* type_create_primitive(PrimitiveType primitive);
TypeEntry* type_create_class(const char* name);
TypeEntry* type_create_record(const char* name);
TypeEntry* type_create_enum(const char* name);
TypeEntry* type_create_interface(const char* name);
TypeEntry* type_create_option(TypeEntry* inner_type);
TypeEntry* type_create_result(TypeEntry* ok_type, TypeEntry* err_type);
TypeEntry* type_create_task(TypeEntry* result_type);
TypeEntry* type_create_array(TypeEntry* element_type, uint32_t size);
TypeEntry* type_create_function(TypeEntry** param_types, uint32_t param_count, TypeEntry* return_type);

// Type manipulation functions
void type_destroy(TypeEntry* type);
bool type_is_assignable(TypeEntry* from, TypeEntry* to);
bool type_is_compatible(TypeEntry* type1, TypeEntry* type2);
bool type_is_nullable(TypeEntry* type);
TypeEntry* type_make_nullable(TypeEntry* type);
TypeEntry* type_make_non_nullable(TypeEntry* type);

// Type resolution functions
TypeEntry* type_resolve_identifier(TypeTable* table, const char* name);
TypeEntry* type_resolve_qualified_name(TypeTable* table, const char* qualified_name);
bool type_resolve_generic_args(TypeEntry* generic_type, TypeEntry** args, uint32_t arg_count);

// Type checking functions
bool type_check_assignment(TypeEntry* target, TypeEntry* source);
bool type_check_function_call(TypeEntry* function, TypeEntry** args, uint32_t arg_count);
bool type_check_binary_operation(TypeEntry* left, TypeEntry* right, const char* operator);
TypeEntry* type_get_binary_result_type(TypeEntry* left, TypeEntry* right, const char* operator);

// Type printing functions
void type_print(TypeEntry* type);
const char* type_to_string(TypeEntry* type);
const char* type_kind_to_string(TypeKind kind);
const char* primitive_type_to_string(PrimitiveType primitive);

// Initialize built-in types
void types_initialize_builtins(TypeTable* table);
