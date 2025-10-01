#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Test basic heap allocation and deallocation
void test_basic_allocation(void) {
    printf("=== Testing Basic Allocation ===\n");
    
    printf("Creating heap...\n");
    Heap* heap = heap_create(1024 * 1024); // 1MB heap
    assert(heap != NULL);
    printf("Heap created successfully\n");
    
    // Test allocation
    printf("Allocating 100 bytes...\n");
    void* ptr1 = heap_allocate(heap, 100);
    assert(ptr1 != NULL);
    printf("Allocation successful: %p\n", ptr1);
    assert(heap_is_valid_pointer(heap, ptr1));
    printf("Pointer validation successful\n");
    size_t size = heap_get_allocation_size(heap, ptr1);
    printf("Allocation size: %zu\n", size);
    assert(size == 100);
    
    void* ptr2 = heap_allocate(heap, 200);
    assert(ptr2 != NULL);
    assert(ptr1 != ptr2);
    
    // Test deallocation
    heap_deallocate(heap, ptr1);
    assert(!heap_is_valid_pointer(heap, ptr1));
    
    // Test reallocation
    void* ptr3 = heap_reallocate(heap, ptr2, 300);
    assert(ptr3 != NULL);
    assert(heap_get_allocation_size(heap, ptr3) == 300);
    
    heap_destroy(heap);
    printf("Basic allocation test passed!\n");
}

// Test memory alignment
void test_alignment(void) {
    printf("=== Testing Memory Alignment ===\n");
    
    Heap* heap = heap_create(1024 * 1024);
    assert(heap != NULL);
    
    // Test different alignments
    void* ptr1 = heap_allocate_aligned(heap, 100, 16);
    assert(ptr1 != NULL);
    assert((uintptr_t)ptr1 % 16 == 0);
    
    void* ptr2 = heap_allocate_aligned(heap, 200, 32);
    assert(ptr2 != NULL);
    assert((uintptr_t)ptr2 % 32 == 0);
    
    void* ptr3 = heap_allocate_aligned(heap, 50, 8);
    assert(ptr3 != NULL);
    assert((uintptr_t)ptr3 % 8 == 0);
    
    heap_destroy(heap);
    printf("Alignment test passed!\n");
}

// Test memory fragmentation and compaction
void test_fragmentation(void) {
    printf("=== Testing Memory Fragmentation ===\n");
    
    Heap* heap = heap_create(1024 * 1024);
    assert(heap != NULL);
    
    // Allocate many small blocks
    void* ptrs[100];
    for (int i = 0; i < 100; i++) {
        ptrs[i] = heap_allocate(heap, 100);
        assert(ptrs[i] != NULL);
    }
    
    // Free every other block
    for (int i = 0; i < 100; i += 2) {
        heap_deallocate(heap, ptrs[i]);
    }
    
    // Try to allocate a large block
    void* large_ptr = heap_allocate(heap, 5000);
    assert(large_ptr != NULL);
    
    // Compact heap
    heap_compact(heap);
    
    heap_destroy(heap);
    printf("Fragmentation test passed!\n");
}

// Test garbage collection
void test_garbage_collection(void) {
    printf("=== Testing Garbage Collection ===\n");
    
    Heap* heap = heap_create(1024 * 1024);
    assert(heap != NULL);
    
    // Allocate many objects
    void* ptrs[1000];
    for (int i = 0; i < 1000; i++) {
        ptrs[i] = heap_allocate(heap, 100);
        assert(ptrs[i] != NULL);
    }
    
    // Print stats before GC
    printf("Before GC:\n");
    heap_print_stats(heap);
    
    // Free half the objects
    for (int i = 0; i < 500; i++) {
        heap_deallocate(heap, ptrs[i]);
    }
    
    // Force garbage collection
    gc_collect(heap);
    
    // Print stats after GC
    printf("After GC:\n");
    heap_print_stats(heap);
    
    heap_destroy(heap);
    printf("Garbage collection test passed!\n");
}

// Test memory regions
void test_memory_regions(void) {
    printf("=== Testing Memory Regions ===\n");
    
    Heap* heap = heap_create(1024 * 1024);
    assert(heap != NULL);
    
    // Allocate and free to create fragmentation
    void* ptr1 = heap_allocate(heap, 1000);
    void* ptr2 = heap_allocate(heap, 2000);
    void* ptr3 = heap_allocate(heap, 3000);
    
    heap_deallocate(heap, ptr2); // Free middle block
    
    // Print regions
    heap_print_regions(heap);
    
    heap_destroy(heap);
    printf("Memory regions test passed!\n");
}

// Test allocation tracking
void test_allocation_tracking(void) {
    printf("=== Testing Allocation Tracking ===\n");
    
    Heap* heap = heap_create(1024 * 1024);
    assert(heap != NULL);
    
    // Allocate several objects
    void* ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = heap_allocate(heap, (i + 1) * 100);
        assert(ptrs[i] != NULL);
    }
    
    // Print allocations
    heap_print_allocations(heap);
    
    // Verify allocation count
    assert(heap->allocation_count == 10);
    
    // Free some allocations
    for (int i = 0; i < 5; i++) {
        heap_deallocate(heap, ptrs[i]);
    }
    
    assert(heap->allocation_count == 5);
    
    heap_destroy(heap);
    printf("Allocation tracking test passed!\n");
}

// Test heap validation
void test_heap_validation(void) {
    printf("=== Testing Heap Validation ===\n");
    
    Heap* heap = heap_create(1024 * 1024);
    assert(heap != NULL);
    
    // Allocate some memory
    void* ptr1 = heap_allocate(heap, 1000);
    void* ptr2 = heap_allocate(heap, 2000);
    
    // Validate heap
    assert(heap_check_integrity(heap));
    heap_validate(heap);
    
    // Test leak detection
    heap_detect_leaks(heap);
    
    heap_destroy(heap);
    printf("Heap validation test passed!\n");
}

// Test out of memory handling
void test_out_of_memory(void) {
    printf("=== Testing Out of Memory Handling ===\n");
    
    Heap* heap = heap_create(1024); // Small heap
    assert(heap != NULL);
    
    // Try to allocate more than available
    void* ptr1 = heap_allocate(heap, 512);
    assert(ptr1 != NULL);
    
    void* ptr2 = heap_allocate(heap, 1024); // This should fail
    assert(ptr2 == NULL);
    
    // Free some memory and try again
    heap_deallocate(heap, ptr1);
    ptr2 = heap_allocate(heap, 512);
    assert(ptr2 != NULL);
    
    heap_destroy(heap);
    printf("Out of memory test passed!\n");
}

// Test stress allocation
void test_stress_allocation(void) {
    printf("=== Testing Stress Allocation ===\n");
    
    Heap* heap = heap_create(10 * 1024 * 1024); // 10MB heap
    assert(heap != NULL);
    
    // Allocate and free randomly
    void* ptrs[1000];
    int allocated = 0;
    
    for (int i = 0; i < 10000; i++) {
        if (allocated < 1000 && (rand() % 2 == 0 || allocated == 0)) {
            // Allocate
            size_t size = (rand() % 1000) + 100;
            ptrs[allocated] = heap_allocate(heap, size);
            if (ptrs[allocated] != NULL) {
                allocated++;
            }
        } else if (allocated > 0) {
            // Deallocate
            int index = rand() % allocated;
            heap_deallocate(heap, ptrs[index]);
            ptrs[index] = ptrs[allocated - 1];
            allocated--;
        }
    }
    
    // Clean up remaining allocations
    for (int i = 0; i < allocated; i++) {
        heap_deallocate(heap, ptrs[i]);
    }
    
    heap_destroy(heap);
    printf("Stress allocation test passed!\n");
}

int main(void) {
    printf("He³ Memory Management Test Suite\n");
    printf("================================\n\n");
    
    // Run all tests
    test_basic_allocation();
    printf("\n");
    
    test_alignment();
    printf("\n");
    
    test_fragmentation();
    printf("\n");
    
    test_garbage_collection();
    printf("\n");
    
    test_memory_regions();
    printf("\n");
    
    test_allocation_tracking();
    printf("\n");
    
    test_heap_validation();
    printf("\n");
    
    test_out_of_memory();
    printf("\n");
    
    test_stress_allocation();
    printf("\n");
    
    printf("All memory management tests passed! 🎉\n");
    return 0;
}
