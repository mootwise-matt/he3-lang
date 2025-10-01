#include "vm.h"
#include "loader/bytecode_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Print usage information
void print_usage(const char* program_name) {
    printf("He³ Virtual Machine (he3vm)\n");
    printf("Usage: %s [options] <bytecode_file.bx>\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --version  Show version information\n");
    printf("  -d, --debug    Enable debug output\n");
    printf("  -s, --stack    Show stack state\n");
    printf("  -m, --memory   Show memory statistics\n");
    printf("  -r, --regions  Show memory regions\n");
    printf("  -o, --objects  Show object system information\n");
    printf("  -c, --classes  Show loaded classes\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s program.bx\n", program_name);
    printf("  %s -d program.bx\n", program_name);
    printf("  %s -s program.bx\n", program_name);
    printf("  %s -m program.bx\n", program_name);
    printf("  %s -r program.bx\n", program_name);
    printf("  %s -o program.bx\n", program_name);
    printf("  %s -c program.bx\n", program_name);
}

// Print version information
void print_version(void) {
    printf("He³ Virtual Machine (he3vm) version 0.1.0\n");
    printf("He³ Language Compiler and Runtime\n");
    printf("Copyright (c) 2024 He³ Language Project\n");
    printf("Licensed under the Apache License, Version 2.0\n");
}

// Main function
int main(int argc, char* argv[]) {
    const char* program_name = argv[0];
    const char* bytecode_file = NULL;
    bool debug_mode = false;
    bool show_stack = false;
    bool show_memory = false;
    bool show_regions = false;
    bool show_objects = false;
    bool show_classes = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(program_name);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug_mode = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stack") == 0) {
            show_stack = true;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--memory") == 0) {
            show_memory = true;
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--regions") == 0) {
            show_regions = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--objects") == 0) {
            show_objects = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--classes") == 0) {
            show_classes = true;
        } else if (argv[i][0] != '-') {
            // This is the bytecode file
            bytecode_file = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(program_name);
            return 1;
        }
    }
    
    // Check if bytecode file was provided
    if (!bytecode_file) {
        fprintf(stderr, "Error: No bytecode file specified\n");
        print_usage(program_name);
        return 1;
    }
    
    if (debug_mode) {
        printf("Debug mode enabled\n");
        printf("Loading bytecode file: %s\n", bytecode_file);
    }
    
    // Create VM
    VM* vm = vm_create();
    if (!vm) {
        fprintf(stderr, "Error: Failed to create virtual machine\n");
        return 1;
    }
    
    if (debug_mode) {
        printf("VM created successfully\n");
    }
    
    // Initialize object system
    vm_initialize_object_system(vm);
    if (debug_mode) {
        printf("Object system initialized\n");
    }
    
    // Check if file exists
    FILE* file = fopen(bytecode_file, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", bytecode_file);
        
        // Show object system info even if file doesn't exist
        if (show_objects) {
            printf("\nObject system (file not found):\n");
            vm_print_object_system(vm);
        }
        
        if (show_classes) {
            printf("\nClasses (file not found):\n");
            vm_print_classes(vm);
        }
        
        vm_destroy(vm);
        return 1;
    }
    fclose(file);
    
    // Load bytecode
    if (!vm_load_bytecode(vm, bytecode_file)) {
        fprintf(stderr, "Error: Failed to load bytecode file\n");
        
        // Show memory stats even if bytecode loading fails
        if (show_memory) {
            printf("\nMemory state after failed bytecode load:\n");
            vm_print_heap_stats(vm);
        }
        
        if (show_regions) {
            printf("\nMemory regions after failed bytecode load:\n");
            vm_print_heap_regions(vm);
        }
        
        if (show_objects) {
            printf("\nObject system after failed bytecode load:\n");
            vm_print_object_system(vm);
        }
        
        if (show_classes) {
            printf("\nClasses after failed bytecode load:\n");
            vm_print_classes(vm);
        }
        
        vm_destroy(vm);
        return 1;
    }
    
    if (debug_mode) {
        printf("Bytecode loaded successfully\n");
        bytecode_print_info(vm->bytecode);
    }
    
    // Show initial state if requested
    if (show_stack) {
        printf("\nInitial stack state:\n");
        vm_print_stack(vm);
    }
    
    if (show_memory) {
        printf("\nInitial memory state:\n");
        vm_print_heap_stats(vm);
    }
    
        if (show_regions) {
            printf("\nInitial memory regions:\n");
            vm_print_heap_regions(vm);
        }
        
        if (show_objects) {
            printf("\nInitial object system:\n");
            vm_print_object_system(vm);
        }
        
        if (show_classes) {
            printf("\nInitial classes:\n");
            vm_print_classes(vm);
        }
    
    // Execute bytecode
    int result = vm_execute(vm);
    
    if (debug_mode) {
        printf("Execution completed with result: %d\n", result);
    }
    
    // Show final state if requested
    if (show_stack) {
        printf("\nFinal stack state:\n");
        vm_print_stack(vm);
    }
    
    if (show_memory) {
        printf("\nFinal memory state:\n");
        vm_print_heap_stats(vm);
    }
    
        if (show_regions) {
            printf("\nFinal memory regions:\n");
            vm_print_heap_regions(vm);
        }
        
        if (show_objects) {
            printf("\nFinal object system:\n");
            vm_print_object_system(vm);
        }
        
        if (show_classes) {
            printf("\nFinal classes:\n");
            vm_print_classes(vm);
        }
    
    // Clean up
    vm_destroy(vm);
    
    if (debug_mode) {
        printf("VM destroyed\n");
    }
    
    return result;
}
