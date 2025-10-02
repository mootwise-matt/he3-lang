---
layout: page
title: roadmap
---
# He³ Compiler Development Roadmap

## 🎯 Primary Goal
**Quickly reach a minimal .helium module generator** to enable parallel VM development in `/src/vm/`

## 🏗️ Architecture Overview

```
src/
├── compiler/          # Compiler implementation
│   ├── lexer/        # Tokenization
│   ├── parser/       # AST construction
│   ├── resolver/     # Type resolution
│   ├── emitter/      # .helium module generation
│   ├── common/       # Shared utilities
│   └── tests/        # Test suite
├── vm/               # Virtual machine (parallel development)
│   ├── interpreter/  # Bytecode execution
│   ├── memory/       # Memory management
│   ├── security/     # Security manager
│   ├── events/       # Event manager
│   └── tests/        # VM tests
└── include/          # Shared headers
    ├── helium3.h     # .helium3 file format
    ├── bytecode.h    # Bytecode definitions
    ├── tokens.h      # Token definitions
    ├── ast.h         # AST definitions
    └── types.h       # Type system
```

## 🚀 Phased Development Plan

### **Phase 1: Minimal Lexer + Parser (Week 1)** ✅ **COMPLETE**
**Goal**: Parse simple He³ programs into AST

**Deliverables**:
- [x] Basic lexer for core tokens
- [x] Simple parser for classes and functions
- [x] AST representation
- [x] Basic test framework

**Success Criteria**: Parse `class Program { function main(): integer { return 42; } }` ✅ **ACHIEVED**

### **Phase 2: Type Resolution (Week 2)**
**Goal**: Resolve types and validate basic semantics

**Deliverables**:
- [ ] Type table implementation
- [ ] Basic type resolution
- [ ] Simple semantic validation
- [ ] Error reporting

**Success Criteria**: Validate types in simple programs

### **Phase 3: Minimal .helium3 Emitter (Week 3)**
**Goal**: Generate .helium3 modules from AST

**Deliverables**:
- [ ] .helium3 file format implementation
- [ ] Basic bytecode generation
- [ ] Module metadata generation
- [ ] Simple test programs

**Success Criteria**: Generate .helium3 module for `class Program { function main(): integer { return 42; } }`

### **Phase 4: Enhanced Features (Week 4+)** ✅ **OO FEATURES COMPLETE**
**Goal**: Add more language features incrementally

**Deliverables**:
- [x] **Object-Oriented Programming** - Complete OO system with classes, objects, methods, fields
- [x] **Module Registry System** - Runtime class discovery and method resolution
- [x] **Bytecode Generation** - Complete OO opcode support (NEW, CALL, LOAD_FIELD, etc.)
- [x] **VM Integration** - Module loading and class registration
- [x] **Method Execution** - Complete method execution with bytecode interpretation
- [x] **Field Access** - Complete field loading and storage with type safety
- [ ] Security annotations
- [ ] Event system
- [ ] Async/await
- [ ] Pattern matching
- [ ] Full type system

## 📋 Phase 1 Detailed Tasks

### **Task 1.1: Project Structure Setup**
```bash
mkdir -p src/compiler/{lexer,parser,resolver,emitter,common,tests}
mkdir -p src/vm/{interpreter,memory,security,events,tests}
mkdir -p src/include
```

### **Task 1.2: Shared Headers**
- [ ] Create `src/include/helium3.h` - .helium3 file format
- [ ] Create `src/include/bytecode.h` - Bytecode opcodes
- [ ] Enhance `src/include/tokens.h` - Complete token definitions
- [ ] Enhance `src/include/ast.h` - Complete AST definitions
- [ ] Create `src/include/types.h` - Type system definitions

### **Task 1.3: Basic Lexer**
- [ ] Implement `src/compiler/lexer/lexer.c`
- [ ] Support core tokens: identifiers, keywords, literals, operators
- [ ] Handle comments and whitespace
- [ ] Create lexer test suite

### **Task 1.4: Simple Parser**
- [ ] Implement `src/compiler/parser/parser.c`
- [ ] Parse class declarations
- [ ] Parse method declarations
- [ ] Parse basic expressions
- [ ] Create parser test suite

### **Task 1.5: Build System**
- [ ] Create `CMakeLists.txt` or `Makefile`
- [ ] Setup compilation flags and dependencies
- [ ] Create test runner script
- [ ] Setup CI/CD pipeline

## 🔧 .helium3 File Format

### **Header Structure**
```c
typedef struct {
    char magic[4];           // "H3M1"
    uint16_t version;        // Format version
    uint16_t flags;          // Module flags
    uint32_t domain_offset;  // Domain name offset
    uint32_t type_count;     // Number of types
    uint32_t type_offset;    // Type table offset
    uint32_t method_count;   // Number of methods
    uint32_t method_offset;  // Method table offset
    uint32_t string_offset;  // String table offset
    uint32_t string_size;    // String table size
    uint32_t bytecode_offset;// Bytecode offset
    uint32_t bytecode_size;  // Bytecode size
} Helium3Header;
```

### **Type Entry**
```c
typedef struct {
    uint32_t name_offset;    // String table offset
    uint16_t kind;           // Type kind
    uint16_t flags;          // Type flags
    uint32_t size;           // Size in bytes
    uint32_t method_start;   // First method index
    uint32_t method_count;   // Number of methods
} TypeEntry;
```

### **Method Entry**
```c
typedef struct {
    uint32_t name_offset;    // String table offset
    uint32_t type_index;     // Owner type index
    uint16_t flags;          // Method flags
    uint16_t param_count;    // Parameter count
    uint32_t bytecode_offset;// Bytecode offset
    uint32_t bytecode_size;  // Bytecode size
} MethodEntry;
```

## 🧪 Testing Strategy

### **Unit Tests**
- Each module has comprehensive unit tests
- Golden tests for lexer tokens
- AST structure validation
- Bytecode generation verification

### **Integration Tests**
- End-to-end compilation tests
- .helium3 file format validation
- Cross-module compatibility

### **Regression Tests**
- Automated test suite runs on every commit
- Performance benchmarks
- Memory leak detection

## 📊 Success Metrics

### **Phase 1 Success**
- [ ] Lexer handles all He³ tokens correctly
- [ ] Parser builds valid AST for simple programs
- [ ] Test suite passes 100%
- [ ] Build system works on Linux/macOS/Windows

### **Phase 2 Success**
- [ ] Type resolution works for basic types
- [ ] Semantic validation catches errors
- [ ] Error messages are clear and helpful

### **Phase 3 Success**
- [ ] Generates valid .helium3 modules
- [ ] VM can load and execute generated modules
- [ ] Cross-platform compatibility

## 🔄 Parallel Development

### **Compiler Team Focus**
- Language parsing and analysis
- .helium3 module generation
- Type system implementation
- Security and event integration

### **VM Team Focus**
- Bytecode interpreter
- Memory management
- Security manager
- Event system
- Performance optimization

### **Shared Interfaces**
- .helium3 file format
- Bytecode opcodes
- Type system definitions
- Security protocols
- Event system APIs

## 📅 Timeline

| Phase | Duration | Key Deliverable |
|-------|----------|-----------------|
| Phase 1 | Week 1 | Basic lexer + parser |
| Phase 2 | Week 2 | Type resolution |
| Phase 3 | Week 3 | .helium3 emitter |
| Phase 4 | Week 4+ | Enhanced features |

## 🎯 Immediate Next Steps

1. **Create project structure**
2. **Implement shared headers**
3. **Build basic lexer**
4. **Create simple parser**
5. **Setup test framework**
6. **Generate first .helium3 module**

This roadmap ensures we can quickly reach a working .helium3 module generator while maintaining clean separation between compiler and VM development.
