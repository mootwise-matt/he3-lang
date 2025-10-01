# He³ Object Model and Memory Management

## Overview

The He³ Virtual Machine implements a unified object model where everything inherits from `System.Object`. This document describes the runtime object representation, memory layout, and garbage collection strategy.

## Object Structure

### Base Object Layout

```c
// src/vm/object/object.h
typedef struct Object {
    uint32_t type_id;        // Type identifier (index into type table)
    uint32_t ref_count;      // Reference count for garbage collection
    uint32_t flags;          // Object flags (immutable, pinned, etc.)
    uint32_t size;           // Total object size in bytes
    uint8_t data[];          // Object data (variable size)
} Object;

// Object flags
#define OBJ_FLAG_IMMUTABLE   0x01  // Object cannot be modified
#define OBJ_FLAG_PINNED      0x02  // Object cannot be moved by GC
#define OBJ_FLAG_FINALIZED   0x04  // Object has been finalized
#define OBJ_FLAG_MARKED      0x08  // Marked during GC cycle
```

### Type Information

```c
typedef struct TypeInfo {
    uint32_t type_id;
    char* name;              // Fully qualified type name
    uint32_t parent_type_id; // Parent type (0 for System.Object)
    uint32_t size;           // Instance size in bytes
    uint32_t field_count;
    FieldInfo* fields;       // Field definitions
    uint32_t method_count;
    MethodInfo* methods;     // Method definitions
    VTable* vtable;          // Virtual method table
    uint32_t interface_count;
    InterfaceInfo* interfaces; // Implemented interfaces
} TypeInfo;

typedef struct FieldInfo {
    char* name;              // Field name
    uint32_t type_id;        // Field type
    uint32_t offset;         // Offset in object data
    uint32_t flags;          // Field flags (static, readonly, etc.)
    union {
        void* static_value;  // For static fields
        uint32_t offset;     // For instance fields
    } location;
} FieldInfo;

typedef struct MethodInfo {
    char* name;              // Method name
    uint32_t signature_hash; // Method signature hash
    uint32_t bytecode_offset; // Bytecode location
    uint32_t local_count;    // Number of local variables
    uint32_t param_count;    // Number of parameters
    uint32_t return_type_id; // Return type
    uint32_t flags;          // Method flags
    TypeInfo* param_types[]; // Parameter types
} MethodInfo;

// Method flags
#define METHOD_FLAG_STATIC    0x01  // Static method
#define METHOD_FLAG_VIRTUAL   0x02  // Virtual method
#define METHOD_FLAG_ABSTRACT  0x04  // Abstract method
#define METHOD_FLAG_FINAL     0x08  // Final method (cannot be overridden)
#define METHOD_FLAG_ASYNC     0x10  // Async method
```

### Virtual Method Table (VTable)

```c
typedef struct VTable {
    uint32_t type_id;        // Type this vtable belongs to
    uint32_t method_count;   // Number of virtual methods
    MethodInfo* methods[];   // Virtual method pointers
} VTable;

// VTable layout for inheritance
// Base class methods come first, then derived class methods
// This allows for efficient virtual dispatch
```

## Memory Management

### Heap Layout

```
┌─────────────────────────────────────┐
│           Heap Header               │
├─────────────────────────────────────┤
│         Generation 0 (Young)        │
│  ┌─────────────────────────────────┐ │
│  │        Nursery Space            │ │
│  │  (New objects, short-lived)     │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │        Survivor Space           │ │
│  │  (Objects that survived GC)     │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤
│         Generation 1 (Old)          │
│  ┌─────────────────────────────────┐ │
│  │        Old Space                │ │
│  │  (Long-lived objects)           │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤
│         Large Object Space          │
│  ┌─────────────────────────────────┐ │
│  │        Large Objects            │ │
│  │  (Objects > 85KB)               │ │
│  └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

### Garbage Collection Strategy

```c
// Generational garbage collection
typedef struct GCStats {
    uint64_t total_allocated;
    uint64_t total_freed;
    uint64_t gc_cycles;
    uint64_t last_gc_time;
    size_t heap_size;
    size_t free_space;
} GCStats;

// GC phases
typedef enum {
    GC_PHASE_MARK,      // Mark reachable objects
    GC_PHASE_SWEEP,     // Sweep unreachable objects
    GC_PHASE_COMPACT,   // Compact heap (optional)
    GC_PHASE_FINALIZE   // Finalize objects
} GCPhase;
```

### Object Creation

```c
// Object allocation
Object* allocate_object(uint32_t type_id, size_t size) {
    // Check if GC is needed
    if (should_garbage_collect()) {
        garbage_collect();
    }
    
    // Allocate from appropriate generation
    Object* obj = heap_allocate(size);
    if (!obj) {
        // Try GC and retry
        garbage_collect();
        obj = heap_allocate(size);
    }
    
    // Initialize object header
    obj->type_id = type_id;
    obj->ref_count = 1;
    obj->flags = 0;
    obj->size = size;
    
    // Initialize object data to zero
    memset(obj->data, 0, size - sizeof(Object));
    
    return obj;
}

// Reference counting
void retain_object(Object* obj) {
    if (obj) {
        atomic_increment(&obj->ref_count);
    }
}

void release_object(Object* obj) {
    if (obj && atomic_decrement(&obj->ref_count) == 0) {
        // Object is no longer referenced
        finalize_object(obj);
        free_object(obj);
    }
}
```

## Field Access

### Instance Field Access

```c
// Get field value by offset
void* get_field_ptr(Object* obj, uint32_t field_offset) {
    if (!obj || field_offset >= obj->size) {
        return NULL;
    }
    return obj->data + field_offset;
}

// Type-safe field access
#define GET_FIELD(obj, field_offset, type) \
    (*(type*)get_field_ptr(obj, field_offset))

#define SET_FIELD(obj, field_offset, type, value) \
    (*(type*)get_field_ptr(obj, field_offset) = value)
```

### Static Field Access

```c
// Static fields are stored in a global table
typedef struct StaticFieldTable {
    uint32_t type_id;
    uint32_t field_count;
    StaticField* fields;
} StaticFieldTable;

typedef struct StaticField {
    char* name;
    uint32_t type_id;
    void* value;
    uint32_t flags;
} StaticField;

// Static field access
void* get_static_field(uint32_t type_id, const char* name) {
    StaticFieldTable* table = get_static_field_table(type_id);
    for (uint32_t i = 0; i < table->field_count; i++) {
        if (strcmp(table->fields[i].name, name) == 0) {
            return table->fields[i].value;
        }
    }
    return NULL;
}
```

## Method Dispatch

### Static Method Dispatch

```c
// Direct method lookup
MethodInfo* find_static_method(uint32_t type_id, const char* name, uint32_t signature_hash) {
    TypeInfo* type = get_type_info(type_id);
    for (uint32_t i = 0; i < type->method_count; i++) {
        MethodInfo* method = &type->methods[i];
        if (method->flags & METHOD_FLAG_STATIC &&
            strcmp(method->name, name) == 0 &&
            method->signature_hash == signature_hash) {
            return method;
        }
    }
    return NULL;
}
```

### Virtual Method Dispatch

```c
// Virtual method dispatch using vtable
MethodInfo* find_virtual_method(Object* obj, const char* name, uint32_t signature_hash) {
    TypeInfo* type = get_type_info(obj->type_id);
    VTable* vtable = type->vtable;
    
    for (uint32_t i = 0; i < vtable->method_count; i++) {
        MethodInfo* method = vtable->methods[i];
        if (strcmp(method->name, name) == 0 &&
            method->signature_hash == signature_hash) {
            return method;
        }
    }
    
    // Method not found in vtable, search parent types
    if (type->parent_type_id != 0) {
        return find_virtual_method_in_parent(type->parent_type_id, name, signature_hash);
    }
    
    return NULL;
}
```

### Interface Method Dispatch

```c
// Interface method dispatch
MethodInfo* find_interface_method(Object* obj, uint32_t interface_id, const char* name, uint32_t signature_hash) {
    TypeInfo* type = get_type_info(obj->type_id);
    
    // Check if type implements the interface
    for (uint32_t i = 0; i < type->interface_count; i++) {
        if (type->interfaces[i].interface_id == interface_id) {
            // Look up method in interface implementation
            return find_method_in_interface(type, interface_id, name, signature_hash);
        }
    }
    
    return NULL;
}
```

## Built-in Types

### System.Object

```c
// Base class for all objects
typedef struct SystemObject {
    Object header;
    // No additional fields - all objects inherit this structure
} SystemObject;
```

### System.Int64

```c
typedef struct SystemInt64 {
    Object header;
    int64_t value;
} SystemInt64;
```

### System.Float64

```c
typedef struct SystemFloat64 {
    Object header;
    double value;
} SystemFloat64;
```

### System.Boolean

```c
typedef struct SystemBoolean {
    Object header;
    bool value;
} SystemBoolean;
```

### System.String

```c
typedef struct SystemString {
    Object header;
    uint32_t length;        // String length in characters
    uint32_t byte_length;   // String length in bytes
    char data[];            // UTF-8 encoded string data
} SystemString;
```

### System.Array<T>

```c
typedef struct SystemArray {
    Object header;
    uint32_t element_type_id;
    uint32_t length;
    uint32_t element_size;
    uint8_t data[];         // Array elements
} SystemArray;
```

## Memory Safety

### Bounds Checking

```c
// Array access with bounds checking
void* get_array_element(SystemArray* array, uint32_t index) {
    if (!array || index >= array->length) {
        throw_bounds_exception("Array index out of bounds");
        return NULL;
    }
    return array->data + (index * array->element_size);
}
```

### Null Safety

```c
// Null pointer checking
#define NULL_CHECK(ptr) \
    do { \
        if (!(ptr)) { \
            throw_null_pointer_exception("Null pointer access"); \
            return; \
        } \
    } while(0)
```

## Performance Optimizations

### Object Pooling

```c
// Object pool for frequently allocated objects
typedef struct ObjectPool {
    uint32_t type_id;
    Object** free_objects;
    uint32_t free_count;
    uint32_t capacity;
} ObjectPool;

// Get object from pool
Object* pool_get_object(ObjectPool* pool) {
    if (pool->free_count > 0) {
        return pool->free_objects[--pool->free_count];
    }
    return allocate_object(pool->type_id, get_type_size(pool->type_id));
}

// Return object to pool
void pool_return_object(ObjectPool* pool, Object* obj) {
    if (pool->free_count < pool->capacity) {
        // Reset object state
        memset(obj->data, 0, obj->size - sizeof(Object));
        pool->free_objects[pool->free_count++] = obj;
    } else {
        free_object(obj);
    }
}
```

### Inline Caching

```c
// Inline cache for method calls
typedef struct InlineCache {
    uint32_t type_id;       // Cached type
    MethodInfo* method;     // Cached method
    uint32_t call_count;    // Number of calls
} InlineCache;

// Fast path for method calls
MethodInfo* cached_method_call(Object* obj, const char* name, InlineCache* cache) {
    if (obj->type_id == cache->type_id) {
        cache->call_count++;
        return cache->method;
    }
    
    // Slow path - lookup method and update cache
    MethodInfo* method = find_virtual_method(obj, name, 0);
    cache->type_id = obj->type_id;
    cache->method = method;
    cache->call_count = 1;
    return method;
}
```

This object model provides a solid foundation for the He³ Virtual Machine with efficient memory management, type safety, and performance optimizations.
