# VM Roadmap (Merged)

## Development Phases
- Phase 1: Interpreter, classes, methods, arithmetic, return.
- Phase 2: Control flow, booleans, crowd protection.
- Phase 3: Option/Result, match statements.
- Phase 4: Async/await, scheduler.
- Phase 5: Nullables, boxing/unboxing.
- Phase 6: Records, interfaces, properties.
- Phase 7: Generics, package manager.
- Future: JIT, LLVM backend.

## Engineering Guidelines (from He3)
- **Build Process**: modular compilation → `.ohe3` → link → `.bx`.
- **Testing**: golden bytecode tests per opcode; integration via sample projects.
- **Coding Standards**: clear naming, namespaces via domains, avoid hidden state.
