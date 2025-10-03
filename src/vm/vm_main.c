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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <helium3_module>\n", argv[0]);
        return 1;
    }
    
    // Create VM
    VM* vm = vm_create();
    if (!vm) {
        fprintf(stderr, "Failed to create VM\n");
        return 1;
    }
    
    // Load .helium3 module
    if (vm_load_helium3_module(vm, argv[1]) == 0) {
        fprintf(stderr, "Failed to load .helium3 module: %s\n", argv[1]);
        vm_destroy(vm);
        return 1;
    }
    
    // Execute bytecode
    int result = vm_execute(vm);
    
    // Print result
    if (result == 0) {
        if (!stack_is_empty(vm->stack)) {
            Value result_value = stack_peek(vm->stack, 0);
            printf("Execution completed with result: ");
            switch (result_value.type) {
                case VALUE_I64:
                    printf("%lld\n", result_value.data.i64_value);
                    break;
                case VALUE_F64:
                    printf("%f\n", result_value.data.f64_value);
                    break;
                case VALUE_BOOL:
                    printf("%s\n", result_value.data.bool_value ? "true" : "false");
                    break;
                case VALUE_NULL:
                    printf("null\n");
                    break;
                default:
                    printf("unknown\n");
                    break;
            }
        } else {
            printf("Execution completed with no result\n");
        }
    } else {
        fprintf(stderr, "VM execution failed with code: %d\n", result);
    }
    
    // Cleanup
    vm_destroy(vm);
    
    return result;
}
