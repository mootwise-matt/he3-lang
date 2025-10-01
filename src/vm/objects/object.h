#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Forward declarations
struct Class;
struct Method;
struct Field;
struct Interface;
struct Value;
struct Heap;

// Object header - every object inherits from System.Object
typedef struct ObjectHeader {
    uint32_t type_id;           // Type identifier
    uint32_t ref_count;         // Reference count for GC
    uint32_t size;              // Object size in bytes
    uint32_t flags;             // Object flags (immutable, pinned, etc.)
    struct Class* class_info;   // Pointer to class metadata
} ObjectHeader;

// Complete object structure
typedef struct Object {
    ObjectHeader header;        // Common header
    uint8_t data[];            // Object-specific data
} Object;

// Field information
typedef struct Field {
    char* name;                 // Field name
    uint32_t type_id;           // Field type ID
    uint32_t offset;            // Offset in object
    uint32_t size;              // Field size
    bool is_static;             // Static field flag
    bool is_private;            // Private field flag
    bool is_protected;          // Protected field flag
    bool is_public;             // Public field flag
    struct Field* next;         // Linked list
} Field;

// Method information
typedef struct Method {
    char* name;                 // Method name
    char* signature;            // Method signature (e.g., "():integer")
    uint8_t* bytecode;          // Method bytecode
    uint32_t bytecode_size;     // Bytecode size
    uint32_t local_count;       // Local variable count
    uint32_t param_count;       // Parameter count
    bool is_static;             // Static method flag
    bool is_virtual;            // Virtual method flag
    bool is_abstract;           // Abstract method flag
    bool is_private;            // Private method flag
    bool is_protected;          // Protected method flag
    bool is_public;             // Public method flag
    struct Method* next;        // Linked list
} Method;

// Method table for virtual dispatch
typedef struct MethodTable {
    uint32_t count;             // Number of methods
    struct Method* entries;     // Method entries
} MethodTable;

// Field table for field access
typedef struct FieldTable {
    uint32_t field_count;       // Number of fields
    struct Field* fields;       // Field entries
} FieldTable;

// Interface information
typedef struct Interface {
    char* name;                 // Interface name
    uint32_t interface_id;      // Interface ID
    struct Method* methods;     // Interface methods
    uint32_t method_count;      // Number of methods
    struct Interface* next;     // Linked list
} Interface;

// Class metadata structure
typedef struct Class {
    uint32_t type_id;           // Unique type identifier
    char* name;                 // Class name (e.g., "System.String")
    uint32_t size;              // Instance size
    struct Class* superclass;   // Parent class (for inheritance)
    struct Method* methods;      // Method list
    struct Field* fields;       // Field list
    struct Interface** interfaces; // Implemented interfaces
    uint32_t interface_count;   // Number of interfaces
    uint32_t field_count;       // Number of fields
    uint32_t method_count;      // Number of methods
    bool is_abstract;           // Abstract class flag
    bool is_final;              // Final class flag
    bool is_interface;          // Interface flag
    struct Class* next;         // Linked list for class registry
} Class;

// Object creation and destruction
Object* object_create(struct Heap* heap, Class* class_info);
void object_destroy(Object* object);
Object* object_clone(Object* object);

// Reference counting
void object_add_reference(Object* object);
void object_remove_reference(Object* object);
uint32_t object_get_ref_count(Object* object);

// Object properties
Class* object_get_class(Object* object);
uint32_t object_get_type_id(Object* object);
size_t object_get_size(Object* object);
bool object_is_instance_of(Object* object, Class* class_info);
bool object_is_instance_of_interface(Object* object, Interface* interface);

// Field access
struct Value object_get_field(Object* object, const char* field_name);
void object_set_field(Object* object, const char* field_name, struct Value value);
struct Value object_get_static_field(Class* class_info, const char* field_name);
void object_set_static_field(Class* class_info, const char* field_name, struct Value value);

// Method calls
struct Value object_call_method(Object* object, const char* method_name, struct Value* args, size_t arg_count);
struct Value object_call_static_method(Class* class_info, const char* method_name, struct Value* args, size_t arg_count);
struct Value object_call_virtual_method(Object* object, const char* method_name, struct Value* args, size_t arg_count);

// Class management
Class* class_create(const char* name, uint32_t type_id, uint32_t size);
void class_destroy(Class* class_info);
Class* class_find(const char* name);
void class_register(Class* class_info);
void class_set_superclass(Class* class_info, Class* superclass);
void class_add_interface(Class* class_info, Interface* interface);

// Method management
Method* method_create(const char* name, const char* signature, uint8_t* bytecode, uint32_t bytecode_size);
void method_destroy(Method* method);
void class_add_method(Class* class_info, Method* method);
Method* class_find_method(Class* class_info, const char* name);
Method* class_find_method_by_signature(Class* class_info, const char* name, const char* signature);

// Field management
Field* field_create(const char* name, uint32_t type_id, uint32_t offset, uint32_t size);
void field_destroy(Field* field);
void class_add_field(Class* class_info, Field* field);
Field* class_find_field(Class* class_info, const char* name);

// Interface management
Interface* interface_create(const char* name, uint32_t interface_id);
void interface_destroy(Interface* interface);
void interface_add_method(Interface* interface, Method* method);
Method* interface_find_method(Interface* interface, const char* name);

// Method dispatch
struct Value method_dispatch_static(Method* method, struct Value* args, size_t arg_count);
struct Value method_dispatch_virtual(Object* object, Method* method, struct Value* args, size_t arg_count);
struct Value method_dispatch_interface(Object* object, Interface* interface, const char* method_name, struct Value* args, size_t arg_count);

// Object utilities
void object_print(Object* object);
void object_print_class_hierarchy(Class* class_info);
void object_print_method_table(Class* class_info);
void object_print_field_table(Class* class_info);

// Type checking
bool type_is_assignable(Class* from, Class* to);
bool type_is_subtype(Class* subtype, Class* supertype);
bool type_implements_interface(Class* class_info, Interface* interface);

// Object registry
void object_registry_init(void);
void object_registry_cleanup(void);
void object_registry_clear(void);
void object_registry_register_class(Class* class_info);
Class* object_registry_find_class(const char* name);
Class* object_registry_find_class_by_id(uint32_t type_id);
