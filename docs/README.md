# He³ Documentation (Merged)

Welcome to the **He³ (Helium-3) programming language**.

He³ is a **64-bit, UTF-8, object-oriented language** with Pascal roots and Java/C#-style syntax.
It features async/await, pattern matching, safe error handling (Option/Result), and
domain/crowd-based modular security.

He³ compiles to portable **bytecode (`.bx`)** executed by the **He³ VM**, and can be exported
to native code via a transpiler or LLVM.

---

## 📖 Sections
- [Language Overview](language/overview.md)
- [Syntax & Grammar](language/syntax.md)
- [Type System](language/typesystem.md)
- [Object-Oriented Model](language/oo.md)
- [Async & Concurrency](language/async.md)
- [Error Handling](language/errors.md)
- [Security & Modularity](language/security.md)
- [Event System](language/events.md)
- [Bytecode Format](bytecode/format.md)
- [Opcodes](bytecode/opcodes.md)
- [Object Files](bytecode/objects.md)
- [Bytecode Examples](bytecode/examples.md)
- [VM Design](vm/design.md)
- [Memory Model](vm/memory.md)
- [Dispatch](vm/dispatch.md)
- [VM Roadmap](vm/roadmap.md)
- [Exporter: C Transpiler](exporter/c-transpiler.md)
- [Exporter: LLVM](exporter/llvm.md)
- [Exporter: Native Assembly](exporter/native.md)

## Development
- [Project Guide](compiler/project-guide.md) — Complete guide to project structure, compilation, and building
- [EBNF Grammar](language/ebnf.md)
- [Architecture](compiler/architecture.md)
- [Module Contracts](compiler/module-contracts.md)
- [Testing](compiler/testing.md)

## 🚀 Current Implementation Status

### ✅ **Compiler Implementation**
- **Lexical Analysis** (`src/compiler/lexer/`) - Complete
  - Token generation for all He³ language constructs
  - Comprehensive error handling and reporting
  - Support for strings, numbers, identifiers, keywords, operators
- **Syntax Analysis** (`src/compiler/parser/`) - Complete
  - AST generation for all language features
  - Error recovery and detailed diagnostics
  - Support for classes, methods, fields, statements, expressions
- **AST System** (`src/compiler/ast/`) - Complete
  - Complete AST node definitions for all language constructs
  - Type-safe node creation and manipulation
  - Comprehensive visitor pattern implementation
- **IR System** (`src/compiler/ir/`) - Complete
  - Three-address code intermediate representation
  - IR builder for AST to IR translation
  - Support for all language features and control flow
- **Bytecode Emitter** (`src/compiler/emitter/`) - Complete
  - IR to bytecode translation
  - Bytecode writer with string and relocation tables
  - Complete instruction set support

### ✅ **Virtual Machine Implementation**
- **Core VM Engine** (`src/vm/`) - Complete
  - Bytecode loader and interpreter
  - Stack-based execution model
  - Memory management with generational GC
  - Object system with classes, methods, and fields
  - Cross-platform execution support
- **Shared Bytecode Format** (`src/include/`) - Complete
  - Unified bytecode format between compiler and VM
  - Comprehensive instruction set (100+ opcodes)
  - Complete table structures (string, type, method, field)
  - Opcode utilities and disassembly support

### ✅ **Build System**
- **Makefile** - Complete
  - Full compiler and VM build support
  - Comprehensive test suite
  - Clean build and dependency management
- **Project Structure** - Complete
  - Modular, testable architecture
  - Clear separation of concerns
  - Comprehensive documentation

### 🔄 **Next Phase: Compiler-VM Integration**
- **Bytecode Writer** - In Progress
  - Update compiler to generate shared bytecode format
  - Align bytecode output with VM expectations
  - Enable complete source-to-execution pipeline

## Development Guidelines
- **Temporary Code**: All temporary test files, debug code, and experimental implementations must be placed in the `/tmp/` folder, which is excluded from git
- **Clean Codebase**: Keep the main codebase clean and production-ready
- **Documentation**: Update documentation for all new features and changes
