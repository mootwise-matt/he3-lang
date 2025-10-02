---
layout: page
title: roadmap
---
# VM Roadmap (Merged)

## Development Phases
- ✅ **Phase 0**: Core VM engine, bytecode loader, stack, interpreter, memory management
- ✅ **Phase 1**: Object system (classes, methods, fields), arithmetic, return
- ✅ **Phase 2**: Shared bytecode format, opcode utilities, VM-compiler integration
- ✅ **Phase 3**: Object-Oriented Programming, Module Registry, Class Discovery
- ✅ **Phase 4**: Method resolution, virtual dispatch, field access implementation, method execution
- Phase 5: Option/Result, match statements
- Phase 6: Async/await, scheduler
- Phase 7: Nullables, boxing/unboxing
- Phase 8: Records, interfaces, properties
- Phase 9: Generics, package manager
- Future: JIT, LLVM backend

## ✅ **Phase 2 Complete: Shared Bytecode Format**
- **Shared Headers**: `bytecode_format.h` and `opcodes.h` with comprehensive structures
- **Instruction Set**: 100+ opcodes covering all language features
- **Table Structures**: String, type, method, and field tables
- **Opcode Utilities**: Complete opcode handling, disassembly, and validation
- **VM Integration**: All VM components now use shared format
- **Build System**: Updated Makefile with new components
- **Status**: Ready for compiler bytecode generation integration

## ✅ **Phase 3 Complete: Object-Oriented Programming & Module Registry**
- **Module Registry System**: Complete runtime class discovery and registration
- **Class Discovery**: Automatic discovery of classes, methods, and fields from loaded modules
- **Bytecode Generation**: Full OO opcode support (NEW, CALL, LOAD_FIELD, STORE_FIELD)
- **VM Integration**: Module loading with automatic class registration
- **Memory Management**: Proper cleanup and memory management for module registry
- **Build System**: Module registry integrated into VM build process
- **Status**: OO features fully functional with module registry support

## ✅ **Phase 4 Complete: Method Resolution & Field Access Implementation**
- **Method Resolution**: Complete virtual and static method dispatch
- **Field Access**: Complete field loading and storage with type safety
- **Method Execution**: Complete method execution with bytecode interpretation
- **Object Creation**: Complete object instantiation with `new` keyword
- **Type Safety**: Runtime type validation and checking
- **Stack Management**: Proper call stack handling for method calls
- **Status**: Complete Object-Oriented Programming system fully functional

## Engineering Guidelines (from He3)
- **Build Process**: modular compilation → `.ohe3` → link → `.bx`.
- **Testing**: golden bytecode tests per opcode; integration via sample projects.
- **Coding Standards**: clear naming, namespaces via domains, avoid hidden state.
