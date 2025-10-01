#pragma once
#include <stdint.h>

// .helium3 Module File Format
// This header defines the binary format for He³ modules

#define HELIUM3_MAGIC "H3M1"
#define HELIUM3_VERSION 1

// Module flags
#define HELIUM3_FLAG_SECURE 0x01
#define HELIUM3_FLAG_EVENTS 0x02
#define HELIUM3_FLAG_ASYNC 0x04

// Type kinds
typedef enum {
    TYPE_CLASS = 0,
    TYPE_RECORD = 1,
    TYPE_ENUM = 2,
    TYPE_INTERFACE = 3,
    TYPE_PRIMITIVE = 4
} TypeKind;

// Method flags
#define METHOD_FLAG_STATIC 0x01
#define METHOD_FLAG_VIRTUAL 0x02
#define METHOD_FLAG_ASYNC 0x04
#define METHOD_FLAG_SECURE 0x08

// Helium3 module header
typedef struct {
    char magic[4];           // "H3M1"
    uint16_t version;        // Format version
    uint16_t flags;          // Module flags
    uint32_t domain_offset;  // Domain name offset in string table
    uint32_t type_count;     // Number of types
    uint32_t type_offset;    // Type table offset
    uint32_t method_count;   // Number of methods
    uint32_t method_offset;  // Method table offset
    uint32_t string_offset;  // String table offset
    uint32_t string_size;    // String table size
    uint32_t bytecode_offset;// Bytecode offset
    uint32_t bytecode_size;  // Bytecode size
    uint32_t security_offset;// Security data offset (if present)
    uint32_t security_size;  // Security data size
    uint32_t event_offset;   // Event definitions offset (if present)
    uint32_t event_size;     // Event definitions size
} Helium3Header;

// Type entry in type table
typedef struct {
    uint32_t name_offset;    // String table offset for type name
    uint16_t kind;           // TypeKind
    uint16_t flags;          // Type flags
    uint32_t size;           // Size in bytes
    uint32_t method_start;   // First method index in method table
    uint32_t method_count;   // Number of methods
    uint32_t field_count;    // Number of fields
    uint32_t field_offset;   // Field definitions offset
} TypeEntry;

// Method entry in method table
typedef struct {
    uint32_t name_offset;    // String table offset for method name
    uint32_t type_index;     // Owner type index
    uint16_t flags;          // Method flags
    uint16_t param_count;    // Parameter count
    uint32_t return_type;    // Return type index
    uint32_t bytecode_offset;// Bytecode offset
    uint32_t bytecode_size;  // Bytecode size
    uint32_t local_count;    // Number of local variables
} MethodEntry;

// Field entry
typedef struct {
    uint32_t name_offset;    // String table offset for field name
    uint32_t type_index;     // Field type index
    uint16_t flags;          // Field flags (static, readonly, etc.)
    uint16_t offset;         // Field offset in object
} FieldEntry;

// Security key entry
typedef struct {
    uint32_t name_offset;    // String table offset for key name
    uint32_t key_offset;     // Key data offset
    uint32_t key_size;       // Key data size
    uint32_t permissions;    // Permission flags
} SecurityKeyEntry;

// Event definition entry
typedef struct {
    uint32_t name_offset;    // String table offset for event name
    uint32_t type_index;     // Event data type index
    uint32_t flags;          // Event flags
} EventEntry;

// Complete Helium3 module structure
typedef struct {
    Helium3Header header;
    TypeEntry* types;
    MethodEntry* methods;
    FieldEntry* fields;
    SecurityKeyEntry* security_keys;
    EventEntry* events;
    char* string_table;
    uint8_t* bytecode;
    uint8_t* security_data;
} Helium3Module;

// Function declarations
Helium3Module* helium3_create_module(const char* domain_name);
void helium3_destroy_module(Helium3Module* module);
bool helium3_write_module(Helium3Module* module, const char* filename);
Helium3Module* helium3_read_module(const char* filename);

// Type management
uint32_t helium3_add_type(Helium3Module* module, const char* name, TypeKind kind, uint32_t size);
uint32_t helium3_add_method(Helium3Module* module, uint32_t type_index, const char* name, uint16_t flags);
uint32_t helium3_add_field(Helium3Module* module, uint32_t type_index, const char* name, uint32_t field_type);

// String table management
uint32_t helium3_add_string(Helium3Module* module, const char* str);
const char* helium3_get_string(Helium3Module* module, uint32_t offset);

// Bytecode management
bool helium3_set_method_bytecode(Helium3Module* module, uint32_t method_index, const uint8_t* bytecode, uint32_t size);

// Security management
uint32_t helium3_add_security_key(Helium3Module* module, const char* name, const uint8_t* key_data, uint32_t key_size);

// Event management
uint32_t helium3_add_event(Helium3Module* module, const char* name, uint32_t type_index);
