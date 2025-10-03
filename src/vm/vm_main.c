#include "vm.h"
#include "loader/bytecode_loader.h"
#include "execution/stack.h"
#include "execution/interpreter.h"
#include "execution/context.h"
#include "../shared/bytecode/helium_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    printf("DEBUG: VM main started\n");
    
    if (argc != 2) {
        printf("DEBUG: Invalid arguments\n");
        fprintf(stderr, "Usage: %s <helium3_module>\n", argv[0]);
        return 1;
    }
    
    printf("DEBUG: About to create VM\n");
    // Create VM
    VM* vm = vm_create();
    if (!vm) {
        printf("DEBUG: Failed to create VM\n");
        fprintf(stderr, "Failed to create VM\n");
        return 1;
    }
    
    printf("DEBUG: VM created successfully\n");
    
    // Load .helium3 module
    if (vm_load_helium3_module(vm, argv[1]) == 0) {
        fprintf(stderr, "Failed to load .helium3 module: %s\n", argv[1]);
        vm_destroy(vm);
        return 1;
    }
    
    // Disassemble module to show contents
    vm_disassemble(vm);
    
    // Execute bytecode
    int result = vm_execute(vm);
    
    // Print result
    if (result == 0) {
        printf("Execution completed successfully with result: %d\n", result);
    } else {
        fprintf(stderr, "VM execution failed with code: %d\n", result);
    }
    
    // Cleanup
    vm_destroy(vm);
    
    return result;
}
