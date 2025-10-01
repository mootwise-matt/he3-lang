#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// Default heap configuration
#define DEFAULT_HEAP_SIZE (16 * 1024 * 1024)  // 16MB
#define DEFAULT_ALIGNMENT 8
#define YOUNG_GEN_SIZE (4 * 1024 * 1024)      // 4MB
#define OLD_GEN_SIZE (8 * 1024 * 1024)        // 8MB
#define PERM_GEN_SIZE (4 * 1024 * 1024)       // 4MB

// Heap creation and destruction
Heap* heap_create(size_t initial_size) {
    if (initial_size == 0) {
        initial_size = DEFAULT_HEAP_SIZE;
    }
    
    Heap* heap = malloc(sizeof(Heap));
    if (!heap) {
        return NULL;
    }
    
    // Initialize heap
    memset(heap, 0, sizeof(Heap));
    
    // Allocate raw memory
    heap->memory = malloc(initial_size);
    if (!heap->memory) {
        free(heap);
        return NULL;
    }
    
    heap->total_size = initial_size;
    heap->free_size = initial_size;
    heap->alignment = DEFAULT_ALIGNMENT;
    
    // Create initial free region
    heap->regions = memory_region_create(heap->memory, initial_size);
    if (!heap->regions) {
        free(heap->memory);
        free(heap);
        return NULL;
    }
    
    heap->region_count = 1;
    
    // Initialize garbage collector
    heap->gc = malloc(sizeof(GC));
    if (!heap->gc) {
        memory_region_destroy(heap->regions);
        free(heap->memory);
        free(heap);
        return NULL;
    }
    
    memset(heap->gc, 0, sizeof(GC));
    heap->gc->young_threshold = YOUNG_GEN_SIZE;
    heap->gc->old_threshold = OLD_GEN_SIZE;
    
    // Create generations (only if heap is large enough)
    if (initial_size >= YOUNG_GEN_SIZE + OLD_GEN_SIZE + PERM_GEN_SIZE) {
        void* young_start = heap->memory;
        void* old_start = (char*)young_start + YOUNG_GEN_SIZE;
        void* perm_start = (char*)old_start + OLD_GEN_SIZE;
        
        heap->gc->young_gen = generation_create(young_start, YOUNG_GEN_SIZE);
        heap->gc->old_gen = generation_create(old_start, OLD_GEN_SIZE);
        heap->gc->perm_gen = generation_create(perm_start, PERM_GEN_SIZE);
    } else {
        // For small heaps, just create a single generation
        heap->gc->young_gen = generation_create(heap->memory, initial_size);
        heap->gc->old_gen = NULL;
        heap->gc->perm_gen = NULL;
    }
    
    if (!heap->gc->young_gen) {
        heap_destroy(heap);
        return NULL;
    }
    
    return heap;
}

void heap_destroy(Heap* heap) {
    if (!heap) return;
    
    // Destroy generations
    if (heap->gc) {
        if (heap->gc->young_gen) generation_destroy(heap->gc->young_gen);
        if (heap->gc->old_gen) generation_destroy(heap->gc->old_gen);
        if (heap->gc->perm_gen) generation_destroy(heap->gc->perm_gen);
        free(heap->gc);
    }
    
    // Destroy regions
    MemoryRegion* region = heap->regions;
    while (region) {
        MemoryRegion* next = region->next;
        memory_region_destroy(region);
        region = next;
    }
    
    // Destroy allocations
    Allocation* alloc = heap->allocations;
    while (alloc) {
        Allocation* next = alloc->next;
        allocation_destroy(alloc);
        alloc = next;
    }
    
    // Free raw memory
    if (heap->memory) {
        free(heap->memory);
    }
    
    free(heap);
}

// Memory allocation
void* heap_allocate(Heap* heap, size_t size) {
    if (!heap || size == 0) {
        return NULL;
    }
    
    // Align size
    size_t original_size = size;
    size = (size + heap->alignment - 1) & ~(heap->alignment - 1);
    
    // Find free region
    MemoryRegion* region = memory_region_find_free(heap, size);
    if (!region) {
        // Try garbage collection
        gc_collect(heap);
        region = memory_region_find_free(heap, size);
        if (!region) {
            return NULL; // Out of memory
        }
    }
    
    // Split region if necessary
    if (region->size > size) {
        MemoryRegion* remaining = memory_region_split(region, size);
        if (remaining) {
            // Insert remaining region after current
            remaining->next = region->next;
            remaining->prev = region;
            if (region->next) {
                region->next->prev = remaining;
            }
            region->next = remaining;
            heap->region_count++;
        }
    }
    
    // Mark region as allocated
    region->is_free = false;
    
    // Update heap statistics
    heap->used_size += size;
    heap->free_size -= size;
    heap->total_allocations++;
    
    if (heap->used_size > heap->peak_usage) {
        heap->peak_usage = heap->used_size;
    }
    
    // Create allocation record
    Allocation* alloc = allocation_create(region->start, original_size, 0);
    if (alloc) {
        alloc->next = heap->allocations;
        heap->allocations = alloc;
        heap->allocation_count++;
    }
    
    return region->start;
}

void* heap_allocate_aligned(Heap* heap, size_t size, size_t alignment) {
    if (!heap || size == 0) return NULL;
    
    // Calculate aligned size
    size_t aligned_size = size + alignment - 1;
    void* ptr = heap_allocate(heap, aligned_size);
    if (!ptr) return NULL;
    
    // Align pointer
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    
    return (void*)aligned_addr;
}

void* heap_reallocate(Heap* heap, void* ptr, size_t new_size) {
    if (!ptr) {
        return heap_allocate(heap, new_size);
    }
    
    if (new_size == 0) {
        heap_deallocate(heap, ptr);
        return NULL;
    }
    
    // Find allocation
    Allocation* alloc = allocation_find(heap, ptr);
    if (!alloc) {
        return NULL; // Invalid pointer
    }
    
    // If new size is smaller, just update allocation
    if (new_size <= alloc->size) {
        alloc->size = new_size;
        return ptr;
    }
    
    // Allocate new memory
    void* new_ptr = heap_allocate(heap, new_size);
    if (!new_ptr) {
        return NULL;
    }
    
    // Copy data
    memcpy(new_ptr, ptr, alloc->size);
    
    // Free old allocation
    heap_deallocate(heap, ptr);
    
    return new_ptr;
}

void heap_deallocate(Heap* heap, void* ptr) {
    if (!heap || !ptr) return;
    
    // Find allocation
    Allocation* alloc = allocation_find(heap, ptr);
    if (!alloc) {
        return; // Invalid pointer
    }
    
    // Find corresponding region
    MemoryRegion* region = heap->regions;
    while (region) {
        if (region->start == ptr) {
            break;
        }
        region = region->next;
    }
    
    if (region) {
        // Mark region as free
        region->is_free = true;
        
        // Update heap statistics
        heap->used_size -= alloc->size;
        heap->free_size += alloc->size;
        heap->total_deallocations++;
        
        // Try to merge with adjacent free regions
        if (region->prev && region->prev->is_free) {
            MemoryRegion* merged = memory_region_merge(region->prev, region);
            if (merged) {
                // Remove old regions
                if (region->prev->prev) {
                    region->prev->prev->next = merged;
                } else {
                    heap->regions = merged;
                }
                memory_region_destroy(region->prev);
                memory_region_destroy(region);
                heap->region_count--;
                region = merged;
            }
        }
        
        if (region->next && region->next->is_free) {
            MemoryRegion* merged = memory_region_merge(region, region->next);
            if (merged) {
                // Remove old regions
                if (region->prev) {
                    region->prev->next = merged;
                } else {
                    heap->regions = merged;
                }
                if (region->next->next) {
                    region->next->next->prev = merged;
                }
                memory_region_destroy(region);
                memory_region_destroy(region->next);
                heap->region_count--;
            }
        }
    }
    
    // Remove allocation record
    allocation_remove(heap, ptr);
}

// Memory management utilities
bool heap_is_valid_pointer(Heap* heap, void* ptr) {
    if (!heap || !ptr) return false;
    
    // Check if pointer is within heap bounds
    if (ptr < heap->memory || ptr >= (char*)heap->memory + heap->total_size) {
        return false;
    }
    
    // Check if pointer is allocated
    return allocation_find(heap, ptr) != NULL;
}

size_t heap_get_allocation_size(Heap* heap, void* ptr) {
    Allocation* alloc = allocation_find(heap, ptr);
    return alloc ? alloc->size : 0;
}

void heap_compact(Heap* heap) {
    if (!heap) return;
    
    // Move all allocated regions to the beginning
    MemoryRegion* current = heap->regions;
    void* next_free = heap->memory;
    
    while (current) {
        if (!current->is_free) {
            if (current->start != next_free) {
                // Move data
                memmove(next_free, current->start, current->size);
                current->start = next_free;
            }
            next_free = (char*)next_free + current->size;
        }
        current = current->next;
    }
    
    // Create single free region at the end
    if (next_free < (char*)heap->memory + heap->total_size) {
        size_t free_size = (char*)heap->memory + heap->total_size - (char*)next_free;
        
        // Remove all free regions
        current = heap->regions;
        while (current) {
            MemoryRegion* next = current->next;
            if (current->is_free) {
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    heap->regions = current->next;
                }
                if (current->next) {
                    current->next->prev = current->prev;
                }
                memory_region_destroy(current);
                heap->region_count--;
            }
            current = next;
        }
        
        // Add single free region
        MemoryRegion* free_region = memory_region_create(next_free, free_size);
        if (free_region) {
            free_region->next = heap->regions;
            if (heap->regions) {
                heap->regions->prev = free_region;
            }
            heap->regions = free_region;
            heap->region_count++;
        }
    }
}

void heap_defragment(Heap* heap) {
    heap_compact(heap);
}

// Garbage collection
void gc_collect(Heap* heap) {
    if (!heap || !heap->gc) return;
    
    clock_t start = clock();
    
    // Check if we need collection
    if (heap->used_size < heap->gc->young_threshold) {
        return;
    }
    
    // Mark all reachable objects
    gc_mark_all_roots(heap);
    
    // Sweep generations
    gc_sweep_generation(heap->gc->young_gen);
    gc_sweep_generation(heap->gc->old_gen);
    
    // Update statistics
    heap->gc->stats.collections_performed++;
    clock_t end = clock();
    double collection_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    heap->gc->stats.avg_collection_time = 
        (heap->gc->stats.avg_collection_time * (heap->gc->stats.collections_performed - 1) + collection_time) 
        / heap->gc->stats.collections_performed;
}

void gc_collect_young(Heap* heap) {
    if (!heap || !heap->gc || !heap->gc->young_gen) return;
    
    // Mark young generation objects
    gc_mark_all_roots(heap);
    
    // Sweep young generation
    gc_sweep_generation(heap->gc->young_gen);
}

void gc_collect_full(Heap* heap) {
    if (!heap || !heap->gc) return;
    
    // Mark all objects
    gc_mark_all_roots(heap);
    
    // Sweep all generations
    gc_sweep_generation(heap->gc->young_gen);
    gc_sweep_generation(heap->gc->old_gen);
    gc_sweep_generation(heap->gc->perm_gen);
}

void gc_collect_incremental(Heap* heap) {
    if (!heap || !heap->gc) return;
    
    // Incremental collection - process one generation at a time
    switch (heap->gc->collection_step % 3) {
        case 0:
            gc_collect_young(heap);
            break;
        case 1:
            gc_sweep_generation(heap->gc->old_gen);
            break;
        case 2:
            gc_sweep_generation(heap->gc->perm_gen);
            break;
    }
    
    heap->gc->collection_step++;
}

// GC utilities
void gc_mark_object(struct Object* object) {
    // TODO: Implement object marking
    // This will be implemented when we have the object system
    (void)object;
}

void gc_mark_all_roots(Heap* heap) {
    // TODO: Mark all root objects
    // This includes:
    // - Stack variables
    // - Global variables
    // - Static variables
    // - Method parameters
    (void)heap;
}

void gc_sweep_generation(struct Generation* gen) {
    if (!gen) return;
    
    // TODO: Implement generation sweeping
    // This will be implemented when we have the object system
}

void gc_move_object(struct Object* object, struct Generation* target_gen) {
    // TODO: Implement object movement between generations
    (void)object;
    (void)target_gen;
}

// Memory region management
MemoryRegion* memory_region_create(void* start, size_t size) {
    MemoryRegion* region = malloc(sizeof(MemoryRegion));
    if (!region) return NULL;
    
    region->start = start;
    region->size = size;
    region->is_free = true;
    region->next = NULL;
    region->prev = NULL;
    
    return region;
}

void memory_region_destroy(MemoryRegion* region) {
    if (region) {
        free(region);
    }
}

MemoryRegion* memory_region_split(MemoryRegion* region, size_t size) {
    if (!region || region->size <= size) return NULL;
    
    // Create new region for remaining space
    void* new_start = (char*)region->start + size;
    size_t new_size = region->size - size;
    
    MemoryRegion* new_region = memory_region_create(new_start, new_size);
    if (!new_region) return NULL;
    
    // Update original region size
    region->size = size;
    
    return new_region;
}

MemoryRegion* memory_region_merge(MemoryRegion* region1, MemoryRegion* region2) {
    if (!region1 || !region2) return NULL;
    if (!region1->is_free || !region2->is_free) return NULL;
    
    // Check if regions are adjacent
    if ((char*)region1->start + region1->size != region2->start) {
        return NULL;
    }
    
    // Merge region2 into region1
    region1->size += region2->size;
    region1->next = region2->next;
    if (region2->next) {
        region2->next->prev = region1;
    }
    
    // Destroy region2
    memory_region_destroy(region2);
    
    return region1;
}

MemoryRegion* memory_region_find_free(Heap* heap, size_t size) {
    MemoryRegion* region = heap->regions;
    
    while (region) {
        if (region->is_free && region->size >= size) {
            return region;
        }
        region = region->next;
    }
    
    return NULL;
}

// Allocation tracking
Allocation* allocation_create(void* ptr, size_t size, uint32_t type_id) {
    Allocation* alloc = malloc(sizeof(Allocation));
    if (!alloc) return NULL;
    
    alloc->ptr = ptr;
    alloc->size = size;
    alloc->type_id = type_id;
    alloc->object = NULL;
    alloc->next = NULL;
    
    return alloc;
}

void allocation_destroy(Allocation* allocation) {
    if (allocation) {
        free(allocation);
    }
}

Allocation* allocation_find(Heap* heap, void* ptr) {
    Allocation* alloc = heap->allocations;
    
    while (alloc) {
        if (alloc->ptr == ptr) {
            return alloc;
        }
        alloc = alloc->next;
    }
    
    return NULL;
}

void allocation_remove(Heap* heap, void* ptr) {
    Allocation* alloc = heap->allocations;
    Allocation* prev = NULL;
    
    while (alloc) {
        if (alloc->ptr == ptr) {
            if (prev) {
                prev->next = alloc->next;
            } else {
                heap->allocations = alloc->next;
            }
            allocation_destroy(alloc);
            heap->allocation_count--;
            return;
        }
        prev = alloc;
        alloc = alloc->next;
    }
}

// Generation management
Generation* generation_create(void* start, size_t size) {
    printf("Creating generation at %p with size %zu\n", start, size);
    
    Generation* gen = malloc(sizeof(Generation));
    if (!gen) {
        printf("Failed to allocate generation structure\n");
        return NULL;
    }
    
    gen->start = start;
    gen->size = size;
    gen->used = 0;
    gen->object_count = 0;
    gen->object_capacity = 1024; // Initial capacity
    gen->next = NULL;
    
    printf("Allocating object array...\n");
    gen->objects = malloc(sizeof(struct Object*) * gen->object_capacity);
    if (!gen->objects) {
        printf("Failed to allocate object array\n");
        free(gen);
        return NULL;
    }
    
    printf("Generation created successfully\n");
    return gen;
}

void generation_destroy(Generation* gen) {
    if (gen) {
        if (gen->objects) {
            free(gen->objects);
        }
        free(gen);
    }
}

bool generation_add_object(Generation* gen, struct Object* object) {
    if (!gen || !object) return false;
    
    // Resize if necessary
    if (gen->object_count >= gen->object_capacity) {
        size_t new_capacity = gen->object_capacity * 2;
        struct Object** new_objects = realloc(gen->objects, sizeof(struct Object*) * new_capacity);
        if (!new_objects) return false;
        
        gen->objects = new_objects;
        gen->object_capacity = new_capacity;
    }
    
    gen->objects[gen->object_count] = object;
    gen->object_count++;
    
    return true;
}

void generation_remove_object(Generation* gen, struct Object* object) {
    if (!gen || !object) return;
    
    for (size_t i = 0; i < gen->object_count; i++) {
        if (gen->objects[i] == object) {
            // Move last object to this position
            gen->objects[i] = gen->objects[gen->object_count - 1];
            gen->object_count--;
            break;
        }
    }
}

void generation_compact(Generation* gen) {
    if (!gen) return;
    
    // TODO: Implement generation compaction
    // This will be implemented when we have the object system
}

// Statistics and debugging
void heap_print_stats(Heap* heap) {
    if (!heap) return;
    
    printf("=== Heap Statistics ===\n");
    printf("Total Size: %zu bytes (%.2f MB)\n", 
           heap->total_size, heap->total_size / (1024.0 * 1024.0));
    printf("Used Size: %zu bytes (%.2f MB)\n", 
           heap->used_size, heap->used_size / (1024.0 * 1024.0));
    printf("Free Size: %zu bytes (%.2f MB)\n", 
           heap->free_size, heap->free_size / (1024.0 * 1024.0));
    printf("Peak Usage: %zu bytes (%.2f MB)\n", 
           heap->peak_usage, heap->peak_usage / (1024.0 * 1024.0));
    printf("Allocations: %zu\n", heap->total_allocations);
    printf("Deallocations: %zu\n", heap->total_deallocations);
    printf("Active Allocations: %zu\n", heap->allocation_count);
    printf("Memory Regions: %zu\n", heap->region_count);
    printf("Utilization: %.2f%%\n", 
           (double)heap->used_size / heap->total_size * 100.0);
    
    if (heap->gc) {
        printf("\n=== GC Statistics ===\n");
        printf("Collections: %zu\n", heap->gc->stats.collections_performed);
        printf("Objects Collected: %zu\n", heap->gc->stats.objects_collected);
        printf("Bytes Freed: %zu\n", heap->gc->stats.bytes_freed);
        printf("Avg Collection Time: %.6f seconds\n", heap->gc->stats.avg_collection_time);
    }
}

void heap_print_regions(Heap* heap) {
    if (!heap) return;
    
    printf("=== Memory Regions ===\n");
    MemoryRegion* region = heap->regions;
    size_t index = 0;
    
    while (region) {
        printf("Region %zu: %p - %p (%zu bytes) %s\n", 
               index++,
               region->start, 
               (char*)region->start + region->size,
               region->size,
               region->is_free ? "[FREE]" : "[ALLOCATED]");
        region = region->next;
    }
}

void heap_print_allocations(Heap* heap) {
    if (!heap) return;
    
    printf("=== Active Allocations ===\n");
    Allocation* alloc = heap->allocations;
    size_t index = 0;
    
    while (alloc) {
        printf("Allocation %zu: %p (%zu bytes) type=%u\n", 
               index++,
               alloc->ptr,
               alloc->size,
               alloc->type_id);
        alloc = alloc->next;
    }
}

void gc_print_stats(Heap* heap) {
    if (!heap || !heap->gc) return;
    
    printf("=== Garbage Collection Statistics ===\n");
    printf("Collections Performed: %zu\n", heap->gc->stats.collections_performed);
    printf("Objects Collected: %zu\n", heap->gc->stats.objects_collected);
    printf("Bytes Freed: %zu\n", heap->gc->stats.bytes_freed);
    printf("Total Allocated: %zu\n", heap->gc->stats.total_allocated);
    printf("Peak Memory: %zu\n", heap->gc->stats.peak_memory);
    printf("Average Collection Time: %.6f seconds\n", heap->gc->stats.avg_collection_time);
    
    if (heap->gc->young_gen) {
        printf("Young Generation: %zu objects\n", heap->gc->young_gen->object_count);
    }
    if (heap->gc->old_gen) {
        printf("Old Generation: %zu objects\n", heap->gc->old_gen->object_count);
    }
    if (heap->gc->perm_gen) {
        printf("Permanent Generation: %zu objects\n", heap->gc->perm_gen->object_count);
    }
}

void heap_validate(Heap* heap) {
    if (!heap) return;
    
    printf("=== Heap Validation ===\n");
    
    // Validate regions
    MemoryRegion* region = heap->regions;
    size_t total_region_size = 0;
    
    while (region) {
        total_region_size += region->size;
        region = region->next;
    }
    
    if (total_region_size != heap->total_size) {
        printf("ERROR: Region size mismatch! Expected %zu, got %zu\n", 
               heap->total_size, total_region_size);
    } else {
        printf("Region sizes: OK\n");
    }
    
    // Validate allocations
    Allocation* alloc = heap->allocations;
    size_t total_allocated = 0;
    
    while (alloc) {
        total_allocated += alloc->size;
        alloc = alloc->next;
    }
    
    if (total_allocated != heap->used_size) {
        printf("ERROR: Allocation size mismatch! Expected %zu, got %zu\n", 
               heap->used_size, total_allocated);
    } else {
        printf("Allocation sizes: OK\n");
    }
    
    printf("Heap validation complete.\n");
}

// Memory safety
bool heap_check_integrity(Heap* heap) {
    if (!heap) return false;
    
    // Check basic invariants
    if (heap->used_size + heap->free_size != heap->total_size) {
        return false;
    }
    
    if (heap->used_size > heap->total_size) {
        return false;
    }
    
    if (heap->free_size > heap->total_size) {
        return false;
    }
    
    return true;
}

void heap_detect_leaks(Heap* heap) {
    if (!heap) return;
    
    printf("=== Memory Leak Detection ===\n");
    
    if (heap->allocation_count > 0) {
        printf("WARNING: %zu potential memory leaks detected!\n", heap->allocation_count);
        heap_print_allocations(heap);
    } else {
        printf("No memory leaks detected.\n");
    }
}

void heap_verify_allocations(Heap* heap) {
    if (!heap) return;
    
    printf("=== Allocation Verification ===\n");
    
    Allocation* alloc = heap->allocations;
    size_t verified = 0;
    
    while (alloc) {
        if (heap_is_valid_pointer(heap, alloc->ptr)) {
            verified++;
        } else {
            printf("ERROR: Invalid allocation pointer %p\n", alloc->ptr);
        }
        alloc = alloc->next;
    }
    
    printf("Verified %zu allocations out of %zu\n", verified, heap->allocation_count);
}
