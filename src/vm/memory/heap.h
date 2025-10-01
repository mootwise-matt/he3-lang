#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Forward declarations
struct Heap;
struct MemoryRegion;
struct Allocation;
struct GC;
struct Generation;

// Forward declaration for Object (will be defined later)
typedef struct Object Object;

// Memory region structure
typedef struct MemoryRegion {
    void* start;                    // Start of region
    size_t size;                    // Region size
    bool is_free;                   // Free/allocated status
    struct MemoryRegion* next;      // Linked list
    struct MemoryRegion* prev;      // Doubly linked list
} MemoryRegion;

// Allocation tracking structure
typedef struct Allocation {
    void* ptr;                      // Allocated pointer
    size_t size;                    // Allocation size
    uint32_t type_id;               // Type identifier
    Object* object;                 // Associated object
    struct Allocation* next;        // Linked list
} Allocation;

// Generation structure for generational GC
typedef struct Generation {
    void* start;                    // Generation start
    size_t size;                    // Generation size
    size_t used;                    // Used space
    Object** objects;               // Objects in this generation
    size_t object_count;            // Number of objects
    size_t object_capacity;         // Object array capacity
    struct Generation* next;        // Next generation
} Generation;

// Garbage collection statistics
typedef struct GCStats {
    size_t collections_performed;   // Number of GC cycles
    size_t objects_collected;       // Objects collected
    size_t bytes_freed;             // Bytes freed
    size_t total_allocated;         // Total bytes allocated
    size_t peak_memory;             // Peak memory usage
    double avg_collection_time;     // Average collection time
} GCStats;

// Garbage collector structure
typedef struct GC {
    // Generations (young, old, permanent)
    struct Generation* young_gen;   // Recently allocated objects
    struct Generation* old_gen;     // Survived objects
    struct Generation* perm_gen;    // Class metadata, strings
    
    // Collection statistics
    struct GCStats stats;
    
    // Collection thresholds
    size_t young_threshold;         // Trigger young GC
    size_t old_threshold;           // Trigger full GC
    
    // Collection flags
    bool is_collecting;             // Currently collecting
    bool incremental_mode;          // Incremental collection
    size_t collection_step;         // Current collection step
} GC;

// Heap structure
typedef struct Heap {
    void* memory;                   // Raw heap memory
    size_t total_size;              // Total heap size
    size_t used_size;               // Currently used memory
    size_t free_size;               // Available memory
    
    // Memory regions
    struct MemoryRegion* regions;   // Contiguous memory regions
    size_t region_count;            // Number of regions
    
    // Allocation tracking
    struct Allocation* allocations; // Allocation table
    size_t allocation_count;        // Number of active allocations
    
    // Garbage collection
    struct GC* gc;                  // GC state and statistics
    
    // Memory alignment
    size_t alignment;               // Memory alignment requirement
    
    // Statistics
    size_t peak_usage;              // Peak memory usage
    size_t total_allocations;       // Total allocations made
    size_t total_deallocations;     // Total deallocations made
} Heap;

// Heap creation and destruction
Heap* heap_create(size_t initial_size);
void heap_destroy(Heap* heap);

// Memory allocation
void* heap_allocate(Heap* heap, size_t size);
void* heap_allocate_aligned(Heap* heap, size_t size, size_t alignment);
void* heap_reallocate(Heap* heap, void* ptr, size_t new_size);
void heap_deallocate(Heap* heap, void* ptr);

// Memory management utilities
bool heap_is_valid_pointer(Heap* heap, void* ptr);
size_t heap_get_allocation_size(Heap* heap, void* ptr);
void heap_compact(Heap* heap);
void heap_defragment(Heap* heap);

// Garbage collection
void gc_collect(Heap* heap);
void gc_collect_young(Heap* heap);
void gc_collect_full(Heap* heap);
void gc_collect_incremental(Heap* heap);

// GC utilities
void gc_mark_object(struct Object* object);
void gc_mark_all_roots(Heap* heap);
void gc_sweep_generation(struct Generation* gen);
void gc_move_object(struct Object* object, struct Generation* target_gen);

// Memory region management
MemoryRegion* memory_region_create(void* start, size_t size);
void memory_region_destroy(MemoryRegion* region);
MemoryRegion* memory_region_split(MemoryRegion* region, size_t size);
MemoryRegion* memory_region_merge(MemoryRegion* region1, MemoryRegion* region2);
MemoryRegion* memory_region_find_free(Heap* heap, size_t size);

// Allocation tracking
Allocation* allocation_create(void* ptr, size_t size, uint32_t type_id);
void allocation_destroy(Allocation* allocation);
Allocation* allocation_find(Heap* heap, void* ptr);
void allocation_remove(Heap* heap, void* ptr);

// Generation management
Generation* generation_create(void* start, size_t size);
void generation_destroy(Generation* gen);
bool generation_add_object(Generation* gen, struct Object* object);
void generation_remove_object(Generation* gen, struct Object* object);
void generation_compact(Generation* gen);

// Statistics and debugging
void heap_print_stats(Heap* heap);
void heap_print_regions(Heap* heap);
void heap_print_allocations(Heap* heap);
void gc_print_stats(Heap* heap);
void heap_validate(Heap* heap);

// Memory safety
bool heap_check_integrity(Heap* heap);
void heap_detect_leaks(Heap* heap);
void heap_verify_allocations(Heap* heap);
