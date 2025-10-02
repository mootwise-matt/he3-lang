---
layout: page
title: memory-management
---
# He³ Virtual Machine Memory Management

## Overview

The He³ Virtual Machine implements a sophisticated memory management system designed for safety, performance, and cross-platform compatibility. This document describes the memory layout, garbage collection strategy, and memory safety mechanisms.

**Status**: ✅ **IMPLEMENTED** - The memory management system is fully functional with generational garbage collection, heap allocation, and memory safety features.

## Memory Layout

### Virtual Address Space

```
┌─────────────────────────────────────┐ 0x0000000000000000
│            Reserved                 │
├─────────────────────────────────────┤ 0x0000000000100000
│         Code Segment                │
│  ┌─────────────────────────────────┐ │
│  │        Bytecode                 │ │
│  │  (Read-only, executable)        │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤ 0x0000000000200000
│         Data Segment                │
│  ┌─────────────────────────────────┐ │
│  │        String Table             │ │
│  │  (Read-only, UTF-8 strings)     │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │        Type Tables              │ │
│  │  (Read-only, metadata)          │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤ 0x0000000000300000
│         Heap Segment                │
│  ┌─────────────────────────────────┐ │
│  │        Generation 0             │ │
│  │  (Young objects)                │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │        Generation 1             │ │
│  │  (Old objects)                  │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │        Large Object Space       │ │
│  │  (Objects > 85KB)               │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤ 0x0000000000400000
│         Stack Segment               │
│  ┌─────────────────────────────────┐ │
│  │        Call Stack               │ │
│  │  (Method frames, locals)        │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤ 0x0000000000500000
│         Thread Local Storage        │
│  ┌─────────────────────────────────┐ │
│  │        Thread Data              │ │
│  │  (Per-thread state)             │ │
│  └─────────────────────────────────┘ │
└─────────────────────────────────────┘ 0x0000000000600000
```

### Heap Organization

```c
// src/vm/memory/heap.h
typedef struct Heap {
    uint8_t* base_address;      // Base address of heap
    size_t total_size;          // Total heap size
    size_t used_size;           // Currently used size
    size_t free_size;           // Available free size
    
    // Generation management
    Generation* gen0;           // Young generation
    Generation* gen1;           // Old generation
    LargeObjectSpace* los;      // Large object space
    
    // GC state
    bool gc_in_progress;        // GC currently running
    uint64_t gc_cycles;         // Number of GC cycles
    uint64_t last_gc_time;      // Last GC timestamp
    
    // Statistics
    HeapStats stats;            // Heap statistics
} Heap;

typedef struct Generation {
    uint8_t* start;             // Start of generation
    uint8_t* end;               // End of generation
    uint8_t* alloc_ptr;         // Current allocation pointer
    size_t size;                // Generation size
    size_t used;                // Used space
    size_t free;                // Free space
    
    // Survivor spaces for copying GC
    uint8_t* survivor_space1;
    uint8_t* survivor_space2;
    bool use_space1;            // Which survivor space is active
    
    // Collection statistics
    uint64_t collections;       // Number of collections
    uint64_t promoted_objects;  // Objects promoted to next generation
} Generation;
```

## Garbage Collection Strategy

### Generational Collection

The VM uses a generational garbage collector based on the observation that most objects die young:

1. **Generation 0 (Young)**: Newly allocated objects
2. **Generation 1 (Old)**: Objects that survived one or more GC cycles
3. **Large Object Space**: Objects larger than 85KB

### Collection Algorithms

#### Young Generation (Copying Collector)

```c
// Copying garbage collection for young generation
void collect_generation0(Heap* heap) {
    Generation* gen0 = heap->gen0;
    uint8_t* from_space = gen0->use_space1 ? gen0->start : gen0->survivor_space1;
    uint8_t* to_space = gen0->use_space1 ? gen0->survivor_space1 : gen0->start;
    
    // Mark phase - mark all reachable objects
    mark_reachable_objects(heap, from_space);
    
    // Copy phase - copy live objects to to_space
    uint8_t* scan_ptr = to_space;
    uint8_t* free_ptr = to_space;
    
    for (Object* obj = (Object*)from_space; obj < (Object*)gen0->alloc_ptr; obj = next_object(obj)) {
        if (is_marked(obj)) {
            // Copy object to to_space
            Object* new_obj = (Object*)free_ptr;
            memcpy(new_obj, obj, obj->size);
            
            // Update forwarding pointer
            set_forwarding_pointer(obj, new_obj);
            
            // Update object references
            update_object_references(new_obj);
            
            free_ptr += obj->size;
        }
    }
    
    // Switch survivor spaces
    gen0->use_space1 = !gen0->use_space1;
    gen0->alloc_ptr = free_ptr;
}
```

#### Old Generation (Mark-Sweep-Compact)

```c
// Mark-sweep-compact collection for old generation
void collect_generation1(Heap* heap) {
    Generation* gen1 = heap->gen1;
    
    // Mark phase - mark all reachable objects
    mark_reachable_objects(heap, gen1->start);
    
    // Sweep phase - free unmarked objects
    sweep_generation1(gen1);
    
    // Compact phase - defragment heap
    compact_generation1(gen1);
}

void sweep_generation1(Generation* gen1) {
    for (Object* obj = (Object*)gen1->start; obj < (Object*)gen1->alloc_ptr; obj = next_object(obj)) {
        if (!is_marked(obj)) {
            // Object is dead, add to free list
            add_to_free_list(obj);
        } else {
            // Object is live, unmark it
            unmark_object(obj);
        }
    }
}

void compact_generation1(Generation* gen1) {
    uint8_t* compact_ptr = gen1->start;
    
    // Move live objects to beginning of generation
    for (Object* obj = (Object*)gen1->start; obj < (Object*)gen1->alloc_ptr; obj = next_object(obj)) {
        if (is_live(obj)) {
            if (obj != (Object*)compact_ptr) {
                // Move object
                memmove(compact_ptr, obj, obj->size);
                update_object_references((Object*)compact_ptr);
            }
            compact_ptr += obj->size;
        }
    }
    
    gen1->alloc_ptr = compact_ptr;
}
```

### Root Set Identification

```c
// Root set for garbage collection
typedef struct RootSet {
    // Global variables
    Object** global_roots;
    uint32_t global_root_count;
    
    // Stack roots
    Object** stack_roots;
    uint32_t stack_root_count;
    
    // Static field roots
    Object** static_roots;
    uint32_t static_root_count;
    
    // Thread-local roots
    Object** thread_roots;
    uint32_t thread_root_count;
} RootSet;

void mark_reachable_objects(Heap* heap, uint8_t* start) {
    RootSet* roots = get_root_set();
    
    // Mark all root objects
    for (uint32_t i = 0; i < roots->global_root_count; i++) {
        mark_object(roots->global_roots[i]);
    }
    
    for (uint32_t i = 0; i < roots->stack_root_count; i++) {
        mark_object(roots->stack_roots[i]);
    }
    
    for (uint32_t i = 0; i < roots->static_root_count; i++) {
        mark_object(roots->static_roots[i]);
    }
    
    for (uint32_t i = 0; i < roots->thread_root_count; i++) {
        mark_object(roots->thread_roots[i]);
    }
}
```

## Memory Allocation

### Object Allocation

```c
// Fast path allocation
Object* allocate_object(uint32_t type_id, size_t size) {
    Heap* heap = get_current_heap();
    
    // Check if allocation fits in current generation
    if (size > LARGE_OBJECT_THRESHOLD) {
        return allocate_large_object(type_id, size);
    }
    
    // Try to allocate from generation 0
    Object* obj = try_allocate_generation0(heap, size);
    if (obj) {
        return obj;
    }
    
    // Generation 0 is full, trigger collection
    collect_generation0(heap);
    
    // Try again after collection
    obj = try_allocate_generation0(heap, size);
    if (obj) {
        return obj;
    }
    
    // Still no space, promote to generation 1
    return allocate_generation1(heap, type_id, size);
}

Object* try_allocate_generation0(Heap* heap, size_t size) {
    Generation* gen0 = heap->gen0;
    
    // Align size to 8-byte boundary
    size = align_size(size, 8);
    
    // Check if allocation fits
    if (gen0->alloc_ptr + size > gen0->end) {
        return NULL;
    }
    
    // Allocate object
    Object* obj = (Object*)gen0->alloc_ptr;
    gen0->alloc_ptr += size;
    
    // Initialize object header
    obj->type_id = 0; // Will be set by caller
    obj->ref_count = 1;
    obj->flags = 0;
    obj->size = size;
    
    // Clear object data
    memset(obj->data, 0, size - sizeof(Object));
    
    return obj;
}
```

### Large Object Allocation

```c
// Large object allocation (objects > 85KB)
Object* allocate_large_object(uint32_t type_id, size_t size) {
    LargeObjectSpace* los = get_large_object_space();
    
    // Allocate directly from OS
    void* memory = os_allocate(size);
    if (!memory) {
        return NULL;
    }
    
    Object* obj = (Object*)memory;
    obj->type_id = type_id;
    obj->ref_count = 1;
    obj->flags = OBJ_FLAG_LARGE;
    obj->size = size;
    
    // Add to large object list
    add_large_object(los, obj);
    
    return obj;
}
```

## Memory Safety

### Bounds Checking

```c
// Array bounds checking
void* get_array_element(SystemArray* array, uint32_t index) {
    if (!array) {
        throw_null_pointer_exception("Array is null");
        return NULL;
    }
    
    if (index >= array->length) {
        throw_bounds_exception("Array index out of bounds");
        return NULL;
    }
    
    return array->data + (index * array->element_size);
}

// Object field bounds checking
void* get_field_ptr(Object* obj, uint32_t field_offset) {
    if (!obj) {
        throw_null_pointer_exception("Object is null");
        return NULL;
    }
    
    if (field_offset >= obj->size - sizeof(Object)) {
        throw_bounds_exception("Field offset out of bounds");
        return NULL;
    }
    
    return obj->data + field_offset;
}
```

### Null Pointer Safety

```c
// Null pointer checking macros
#define NULL_CHECK(ptr) \
    do { \
        if (!(ptr)) { \
            throw_null_pointer_exception("Null pointer access"); \
            return; \
        } \
    } while(0)

#define NULL_CHECK_RET(ptr, ret) \
    do { \
        if (!(ptr)) { \
            throw_null_pointer_exception("Null pointer access"); \
            return ret; \
        } \
    } while(0)

// Safe object access
Object* safe_get_object(Object* obj, uint32_t field_offset) {
    NULL_CHECK_RET(obj, NULL);
    
    void* field_ptr = get_field_ptr(obj, field_offset);
    if (!field_ptr) {
        return NULL;
    }
    
    return *(Object**)field_ptr;
}
```

### Memory Corruption Detection

```c
// Object integrity checking
bool validate_object(Object* obj) {
    if (!obj) {
        return false;
    }
    
    // Check object header
    if (obj->type_id == 0 || obj->type_id > MAX_TYPE_ID) {
        return false;
    }
    
    if (obj->size < sizeof(Object) || obj->size > MAX_OBJECT_SIZE) {
        return false;
    }
    
    // Check type consistency
    TypeInfo* type = get_type_info(obj->type_id);
    if (!type || obj->size < type->size) {
        return false;
    }
    
    return true;
}

// Heap integrity checking
bool validate_heap(Heap* heap) {
    // Check all objects in heap
    for (Object* obj = (Object*)heap->gen0->start; 
         obj < (Object*)heap->gen0->alloc_ptr; 
         obj = next_object(obj)) {
        if (!validate_object(obj)) {
            return false;
        }
    }
    
    return true;
}
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
    pthread_mutex_t mutex;
} ObjectPool;

Object* pool_get_object(ObjectPool* pool) {
    pthread_mutex_lock(&pool->mutex);
    
    if (pool->free_count > 0) {
        Object* obj = pool->free_objects[--pool->free_count];
        pthread_mutex_unlock(&pool->mutex);
        
        // Reset object state
        memset(obj->data, 0, obj->size - sizeof(Object));
        obj->ref_count = 1;
        
        return obj;
    }
    
    pthread_mutex_unlock(&pool->mutex);
    
    // Pool is empty, allocate new object
    return allocate_object(pool->type_id, get_type_size(pool->type_id));
}

void pool_return_object(ObjectPool* pool, Object* obj) {
    if (!obj || obj->ref_count > 0) {
        return; // Object still referenced
    }
    
    pthread_mutex_lock(&pool->mutex);
    
    if (pool->free_count < pool->capacity) {
        pool->free_objects[pool->free_count++] = obj;
        pthread_mutex_unlock(&pool->mutex);
    } else {
        pthread_mutex_unlock(&pool->mutex);
        free_object(obj);
    }
}
```

### Write Barriers

```c
// Write barrier for generational GC
void write_barrier(Object* obj, uint32_t field_offset, Object* new_value) {
    if (!obj || !new_value) {
        return;
    }
    
    // Check if we're storing a reference to a young object in an old object
    if (is_old_object(obj) && is_young_object(new_value)) {
        // Add old object to remembered set
        add_to_remembered_set(obj);
    }
}

// Macro for field assignments
#define SET_FIELD_WITH_BARRIER(obj, field_offset, type, value) \
    do { \
        write_barrier(obj, field_offset, (Object*)value); \
        *(type*)get_field_ptr(obj, field_offset) = value; \
    } while(0)
```

### Inline Caching

```c
// Inline cache for method calls
typedef struct InlineCache {
    uint32_t type_id;       // Cached type
    MethodInfo* method;     // Cached method
    uint32_t call_count;    // Number of calls
    uint32_t hit_count;     // Number of cache hits
} InlineCache;

// Fast path for method calls
MethodInfo* cached_method_call(Object* obj, const char* name, InlineCache* cache) {
    if (obj->type_id == cache->type_id) {
        cache->hit_count++;
        cache->call_count++;
        return cache->method;
    }
    
    // Slow path - lookup method and update cache
    MethodInfo* method = find_virtual_method(obj, name, 0);
    cache->type_id = obj->type_id;
    cache->method = method;
    cache->call_count++;
    
    return method;
}
```

## Thread Safety

### Per-Thread Heaps

```c
// Thread-local heap for lock-free allocation
typedef struct ThreadHeap {
    Heap* heap;              // Thread's heap
    ObjectPool* pools;       // Thread-local object pools
    uint32_t pool_count;     // Number of pools
    pthread_key_t tls_key;   // Thread-local storage key
} ThreadHeap;

// Get thread-local heap
Heap* get_thread_heap() {
    ThreadHeap* thread_heap = pthread_getspecific(thread_heap_key);
    if (!thread_heap) {
        thread_heap = create_thread_heap();
        pthread_setspecific(thread_heap_key, thread_heap);
    }
    return thread_heap->heap;
}
```

### Lock-Free Data Structures

```c
// Lock-free object allocation
Object* lock_free_allocate(uint32_t type_id, size_t size) {
    ThreadHeap* thread_heap = get_thread_heap();
    
    // Try thread-local allocation first
    Object* obj = try_allocate_thread_local(thread_heap, size);
    if (obj) {
        return obj;
    }
    
    // Fall back to global heap with locking
    return allocate_object(type_id, size);
}
```

This comprehensive memory management system provides the foundation for a safe, efficient, and scalable He³ Virtual Machine.
