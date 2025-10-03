---
layout: page
title: design
---
# VM Design (Merged)

## Model
- **Stack-based interpreter** with PC, SP, FP registers.
- Each frame stores locals + operand stack.
- **Memory management** with generational garbage collection (✅ **IMPLEMENTED**).
- **Object system** with classes, methods, and fields (✅ **IMPLEMENTED**).
- **Static method support** with native implementations (✅ **IMPLEMENTED**).
- **Built-in system functions** via `Sys` class (✅ **IMPLEMENTED**).
- **Bytecode loader** for .bx and .helium3 files (✅ **IMPLEMENTED**).
- **Cross-platform execution** on ARM, Apple Silicon, Intel (✅ **IMPLEMENTED**).
- **Shared architecture** with unified AST and bytecode format (✅ **IMPLEMENTED**).
- **Comprehensive instruction set** with 100+ opcodes (✅ **IMPLEMENTED**).
- Security manager for module authentication (🔄 **PLANNED**).
- Event manager for cross-thread communication (🔄 **PLANNED**).

## Shared Architecture

The VM is designed to work seamlessly with the compiler through a **shared architecture**:

### **Shared Components** (`src/shared/`)
- **AST System** - Unified tree representation used by both compiler and VM
- **Bytecode Format** - Shared instruction set and file format
- **Token Definitions** - Common token types for language processing
- **Type System** - Common type definitions and utilities

### **Benefits**
- **Consistency** - Compiler and VM use identical data structures
- **Reliability** - Eliminates compatibility issues between components
- **Maintainability** - Single source of truth for common definitions

## Linking & Dynamic Linking
- `.ohe3` files carry imports.
- Linker resolves imports across domains.
- Crowds restrict cross-domain calls.
- `.helium3` modules = prelinked libraries.

## Interpreter Loop (C pseudocode)
```c
while (1) {
  uint8_t op = fetch(pc++);
  switch (op) {
    case OP_PUSH_I64: push(read_i64()); break;
    case OP_ADD: { long b=pop(); long a=pop(); push(a+b); } break;
    case OP_CALL: callMethod(read_u32()); break;
    case OP_CALLV: callVirtual(read_u32()); break;
    case OP_CALL_STATIC: callStatic(read_u32()); break;  // Static method calls
    case OP_RET: returnFromMethod(); break;
    case OP_SECURE_CALL: secureCall(read_u32()); break;
    case OP_PUBLISH_EVENT: publishEvent(read_u32()); break;
    case OP_SUBSCRIBE_EVENT: subscribeEvent(read_u32()); break;
  }
}
```

## Static Method Support

The VM includes comprehensive support for static method calls:

### **Native Implementations**
- **`Sys.print(string)`** - Print string without newline
- **`Sys.println(string)`** - Print string with newline  
- **`Sys.currentTimeMillis()`** - Get current timestamp
- **Additional Sys methods** - File I/O, environment, system utilities

### **Method Dispatch**
- **`OP_CALL_STATIC`** - Direct static method calls
- **Native method resolution** - Built-in functions executed natively
- **Stack management** - Proper argument passing and return value handling
- **Type safety** - Runtime type checking for method arguments

## Security Manager
- Validates security keys at module load time.
- Manages session tokens for secure module access.
- Enforces crowd-based security boundaries.
- Throws SecurityException for invalid access attempts.

## Event Manager
- Thread-safe event publishing and subscription.
- Cross-thread callback execution.
- Event context management with mutex protection.
- Secure event support with authentication.
