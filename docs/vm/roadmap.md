# VM Roadmap (Merged)

## Development Phases
- ✅ **Phase 0**: Core VM engine, bytecode loader, stack, interpreter, memory management
- ✅ **Phase 1**: Object system (classes, methods, fields), arithmetic, return
- ✅ **Phase 2**: Shared bytecode format, opcode utilities, VM-compiler integration
- 🔄 **Phase 3**: Control flow, booleans, method execution, field access
- Phase 4: Option/Result, match statements
- Phase 5: Async/await, scheduler
- Phase 6: Nullables, boxing/unboxing
- Phase 7: Records, interfaces, properties
- Phase 8: Generics, package manager
- Future: JIT, LLVM backend

## ✅ **Phase 2 Complete: Shared Bytecode Format**
- **Shared Headers**: `bytecode_format.h` and `opcodes.h` with comprehensive structures
- **Instruction Set**: 100+ opcodes covering all language features
- **Table Structures**: String, type, method, and field tables
- **Opcode Utilities**: Complete opcode handling, disassembly, and validation
- **VM Integration**: All VM components now use shared format
- **Build System**: Updated Makefile with new components
- **Status**: Ready for compiler bytecode generation integration

## Engineering Guidelines (from He3)
- **Build Process**: modular compilation → `.ohe3` → link → `.bx`.
- **Testing**: golden bytecode tests per opcode; integration via sample projects.
- **Coding Standards**: clear naming, namespaces via domains, avoid hidden state.
