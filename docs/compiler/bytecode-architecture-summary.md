# He³ Bytecode Architecture Summary

## Overview

This document provides a comprehensive overview of the He³ bytecode generation architecture, including object representation, memory management, file formats, and the complete compilation pipeline from source code to executable bytecode.

## Architecture Components

### 1. Object Model and Memory Management

**Key Files:**
- `docs/vm/object-model.md` - Complete object representation design
- `docs/vm/memory-management.md` - Garbage collection and memory safety

**Core Concepts:**
- **Unified Object Model**: Everything inherits from `System.Object`
- **Reference Counting**: Automatic memory management with GC
- **Generational Collection**: Young/old generation separation
- **Memory Safety**: Bounds checking, null safety, corruption detection
- **Performance Optimizations**: Object pooling, inline caching, write barriers

**Object Layout:**
```c
typedef struct Object {
    uint32_t type_id;        // Type identifier
    uint32_t ref_count;      // Reference count
    uint32_t flags;          // Object flags
    uint32_t size;           // Object size
    uint8_t data[];          // Object data
} Object;
```

### 2. .helium Module Format

**Key Files:**
- `docs/bytecode/helium-module-format.md` - Complete module file specification

**File Structure:**
```
┌─────────────────────────────────────┐
│            File Header              │
├─────────────────────────────────────┤
│         String Table                │
├─────────────────────────────────────┤
│         Type Table                  │
├─────────────────────────────────────┤
│         Method Table                │
├─────────────────────────────────────┤
│         Field Table                 │
├─────────────────────────────────────┤
│         Interface Table             │
├─────────────────────────────────────┤
│         Import Table                │
├─────────────────────────────────────┤
│         Export Table                │
├─────────────────────────────────────┤
│         Security Table              │
├─────────────────────────────────────┤
│         Debug Information           │
├─────────────────────────────────────┤
│         Bytecode Section            │
└─────────────────────────────────────┘
```

**Key Features:**
- **Modular Design**: Separate sections for different metadata
- **Cross-Platform**: 64-bit alignment, UTF-8 strings
- **Security**: Domain/crowd membership, permission checking
- **Debug Support**: Line numbers, source file references
- **Dependency Management**: Import/export tables

### 3. Bytecode Instruction Set

**Key Files:**
- `docs/bytecode/instruction-set.md` - Complete instruction set reference
- `docs/bytecode/opcodes.md` - Quick opcode reference

**Instruction Categories:**
- **Stack Operations**: PUSH, POP, DUP, SWAP
- **Local Variables**: LOAD_LOCAL, STORE_LOCAL
- **Field Access**: LOAD_FIELD, STORE_FIELD
- **Arithmetic**: ADD, SUB, MUL, DIV, MOD
- **Control Flow**: JMP, JMPF, JMPT
- **Method Calls**: CALL, CALLV, CALLI, RETURN
- **Object Operations**: NEW, CAST, INSTANCEOF
- **Async Operations**: SPAWN, AWAIT, ASYNC_RETURN
- **Pattern Matching**: MATCH_SOME, MATCH_NONE, MATCH_OK, MATCH_ERR
- **Security**: SECURITY_CHECK, DOMAIN_ENTER, CROWD_VERIFY

### 4. Compilation Pipeline

**Current State:**
```
Source Code (.he3) → Lexer → Tokens → Parser → AST
```

**Target State:**
```
Source Code (.he3) → Lexer → Tokens → Parser → AST → IR → Bytecode → .ohe3 → .bx
```

**Pipeline Stages:**
1. **Lexical Analysis**: Source → Tokens
2. **Syntax Analysis**: Tokens → AST
3. **Semantic Analysis**: AST → Type-checked AST
4. **IR Generation**: AST → Three-address code IR
5. **IR Optimization**: IR → Optimized IR
6. **Bytecode Emission**: IR → Bytecode instructions
7. **Object File Generation**: Bytecode → .ohe3 object file
8. **Linking**: .ohe3 files → .bx executable

## Implementation Roadmap

### Phase 1: Core Infrastructure (Weeks 1-2)
- [ ] Bytecode format implementation
- [ ] Object model foundation
- [ ] IR data structures
- [ ] String and symbol management

### Phase 2: AST to IR Translation (Weeks 3-4)
- [ ] Expression compilation
- [ ] Statement compilation
- [ ] Method compilation
- [ ] Control flow compilation

### Phase 3: IR to Bytecode Translation (Weeks 5-6)
- [ ] Bytecode writer implementation
- [ ] Object file generation
- [ ] Optimization passes
- [ ] Instruction emission

### Phase 4: Advanced Features (Weeks 7-8)
- [ ] Object-oriented features
- [ ] Async/await support
- [ ] Pattern matching
- [ ] Generic types

### Phase 5: Integration and Testing (Weeks 9-10)
- [ ] Compiler integration
- [ ] Testing framework
- [ ] Performance optimization
- [ ] Documentation completion

## Key Design Decisions

### 1. Stack-Based VM
- **Rationale**: Simpler implementation, easier to optimize
- **Benefits**: Clear execution model, efficient instruction encoding
- **Trade-offs**: Slightly more complex than register-based

### 2. Generational Garbage Collection
- **Rationale**: Most objects die young, reduces GC overhead
- **Benefits**: Better performance for typical workloads
- **Trade-offs**: More complex than simple mark-sweep

### 3. Reference Counting + GC
- **Rationale**: Immediate cleanup for simple cases, GC for cycles
- **Benefits**: Best of both worlds
- **Trade-offs**: Slight overhead for reference counting

### 4. Three-Address Code IR
- **Rationale**: Standard intermediate representation
- **Benefits**: Easy to optimize, clear semantics
- **Trade-offs**: More verbose than tree-based IR

### 5. .helium Module Format
- **Rationale**: Self-contained modules with all metadata
- **Benefits**: Easy loading, good for security
- **Trade-offs**: Larger file sizes than minimal formats

## Memory Layout

### Heap Organization
```
┌─────────────────────────────────────┐
│         Generation 0 (Young)        │
│  ┌─────────────────────────────────┐ │
│  │        Nursery Space            │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │        Survivor Space           │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤
│         Generation 1 (Old)          │
│  ┌─────────────────────────────────┐ │
│  │        Old Space                │ │
│  └─────────────────────────────────┘ │
├─────────────────────────────────────┤
│         Large Object Space          │
│  ┌─────────────────────────────────┐ │
│  │        Large Objects            │ │
│  └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

### Object Layout
```
Object Header (16 bytes):
├── type_id (4 bytes)
├── ref_count (4 bytes)
├── flags (4 bytes)
└── size (4 bytes)

Object Data (variable):
├── field1 (8 bytes)
├── field2 (8 bytes)
└── ... more fields
```

## Security Model

### Domain Security
- **Compile-time keys**: Embedded in modules
- **Runtime verification**: Handshake between domains
- **Permission checking**: Required permissions per module

### Crowd Membership
- **Crowd tokens**: Membership verification
- **Cross-domain access**: Controlled by crowd membership
- **Security levels**: Hierarchical permission system

## Performance Characteristics

### Expected Performance
- **Compilation Speed**: ~1000 lines/second
- **Memory Usage**: ~1MB per 10,000 lines
- **Runtime Overhead**: <5% vs native C
- **GC Pause Time**: <1ms for typical programs

### Optimization Strategies
- **Inline Caching**: Fast method dispatch
- **Object Pooling**: Reduce allocation overhead
- **Write Barriers**: Efficient generational GC
- **Peephole Optimization**: Instruction-level optimizations

## Testing Strategy

### Unit Tests
- **IR Generation**: Test AST to IR translation
- **Bytecode Emission**: Test IR to bytecode translation
- **Object File Format**: Test .ohe3 file generation

### Integration Tests
- **End-to-End**: Source to executable
- **Example Programs**: All examples must compile
- **Regression Tests**: Prevent breaking changes

### Performance Tests
- **Compilation Speed**: Benchmark compilation time
- **Memory Usage**: Profile memory consumption
- **Runtime Performance**: Benchmark execution speed

## File Extensions

- **`.he3`**: Source code files
- **`.ohe3`**: Object files (compiled modules)
- **`.helium3`**: Published modules (with metadata)
- **`.bx`**: Executable bytecode files

## Next Steps

1. **Implement Phase 1**: Start with core infrastructure
2. **Focus on Simple Cases**: Get basic arithmetic working
3. **Build Incrementally**: Add features one at a time
4. **Test Continuously**: Write tests for each feature
5. **Document Everything**: Keep documentation up to date

This architecture provides a solid foundation for a modern, safe, and efficient programming language implementation.
