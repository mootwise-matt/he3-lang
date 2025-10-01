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

## Development Guidelines
- **Temporary Code**: All temporary test files, debug code, and experimental implementations must be placed in the `/tmp/` folder, which is excluded from git
- **Clean Codebase**: Keep the main codebase clean and production-ready
- **Documentation**: Update documentation for all new features and changes
