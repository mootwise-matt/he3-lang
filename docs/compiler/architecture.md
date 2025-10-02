# He³ Compiler Architecture

## Overview

The He³ compiler is designed as a modular, multi-stage pipeline that transforms He³ source code into executable bytecode. The architecture emphasizes separation of concerns, testability, and maintainability.

## Shared Architecture

The He³ project uses a **shared architecture** where common components are consolidated in the `/src/shared/` directory to ensure consistency between the compiler and virtual machine:

### **Shared Components** (`src/shared/`)
- **`ast/`** - Unified AST definitions and utilities
- **`tokens.h`** - Common token definitions for lexer and parser
- **`bytecode/`** - Shared bytecode format, opcodes, and utilities
- **`types/`** - Common type definitions and type table structures

### **Benefits of Shared Architecture**
- **Consistency** - Compiler and VM use identical data structures
- **Maintainability** - Single source of truth for common definitions
- **Reliability** - Eliminates compatibility issues between components
- **Development Speed** - Changes to shared components automatically propagate

## Pipeline Stages

### 1. Lexical Analysis (`src/compiler/lexer/`) ✅

**Purpose**: Convert source code into a stream of tokens

**Key Components**:
- `lexer.h` - Lexer structure and public interface
- `lexer.c` - Tokenization logic

**Features**:
- UTF-8 string literal support
- Comprehensive keyword recognition
- Operator and delimiter tokenization
- Line/column tracking for error reporting
- Comment handling (line and block comments)

**Token Types**:
- Identifiers and literals (int, float, string, bool, null)
- Keywords (domain, class, function, async, etc.)
- Operators (arithmetic, comparison, logical, assignment)
- Delimiters (parentheses, braces, brackets, semicolons)
- Type keywords (integer, float, boolean, string, void, object)

### 2. Syntax Analysis (`src/compiler/parser/`) ✅

**Purpose**: Parse token stream into Abstract Syntax Tree (AST)

**Key Components**:
- `parser.h` - Parser structure and function declarations
- `parser.c` - Parsing logic for all language constructs

**Parsing Functions**:
- `parse_compilation_unit()` - Top-level parsing
- `parse_domain_declaration()` - Domain declarations
- `parse_class_declaration()` - Class definitions
- `parse_method_declaration()` - Method definitions
- `parse_interface_declaration()` - Interface definitions
- `parse_record_declaration()` - Record definitions
- `parse_enum_declaration()` - Enum definitions
- `parse_expression()` - Expression parsing with precedence climbing
- `parse_type()` - Type parsing

**Features**:
- Precedence climbing for expressions
- Error recovery and synchronization
- Enhanced error reporting with context
- Support for all He³ language constructs

### 3. Shared Abstract Syntax Tree (`src/shared/ast/`) ✅

**Purpose**: Represent parsed code as a tree structure (shared between compiler and VM)

**Key Components**:
- `ast.h` - AST node definitions and interfaces
- `ast.c` - Tree manipulation and utility functions

**AST Node Types**:
- `AST_COMPUNIT` - Compilation unit root
- `AST_DOMAIN` - Domain declarations
- `AST_CLASS` - Class definitions
- `AST_METHOD` - Method definitions
- `AST_INTERFACE` - Interface definitions
- `AST_RECORD` - Record definitions
- `AST_ENUM` - Enum definitions
- `AST_VAR_DECL` - Variable declarations
- `AST_CALL` - Function calls
- `AST_BINARY` - Binary operations
- `AST_UNARY` - Unary operations
- `AST_LITERAL` - Literal values
- `AST_IDENTIFIER` - Identifiers

**Features**:
- **Shared between compiler and VM** for consistency
- Modular design with separate header/implementation
- Tree traversal and manipulation functions
- Pretty printing for debugging
- Memory management for tree nodes
- Unified literal value handling

### 4. Intermediate Representation (`src/compiler/ir/`) ✅

**Purpose**: Three-address code representation for optimization and code generation

**Key Components**:
- `ir.h` - IR instruction definitions and interfaces
- `ir.c` - IR builder and manipulation functions

**IR Features**:
- Three-address code representation
- Comprehensive instruction set (arithmetic, logical, memory, control flow)
- Function and block management
- Type-safe value handling
- Symbol table integration

**Instruction Types**:
- Load/Store operations (local, field, array, static)
- Arithmetic operations (add, sub, mul, div, mod, neg)
- Comparison operations (eq, ne, lt, le, gt, ge)
- Logical operations (and, or, not)
- Control flow (jmp, jmpf, jmpt, call, return)
- Memory operations (new, new_array, load_field, store_field)

### 5. AST to IR Translation (`src/compiler/emitter/ast_to_ir.c`) ✅

**Purpose**: Convert AST nodes into IR instructions

**Key Features**:
- Expression translation (arithmetic, logical, comparisons)
- Statement translation (declarations, assignments, control flow)
- Method/function translation (calls, invocations, parameters)
- Field and array access translation
- Return value handling
- Symbol table management

**Translation Functions**:
- `ast_to_ir_translate_expression()` - Expression translation
- `ast_to_ir_translate_statement()` - Statement translation
- `ast_to_ir_translate_function()` - Function translation
- `ast_to_ir_translate_method_call()` - Method call translation
- `ast_to_ir_translate_field_access()` - Field access translation

### 6. Bytecode Generation (`src/vm/bytecode/`) ✅

**Purpose**: Convert IR instructions into executable bytecode

**Key Components**:
- `bytecode.h` - Bytecode instruction definitions
- `bytecode.c` - Bytecode writer and utilities

**Features**:
- Bytecode instruction encoding
- String and relocation tables
- Disassembly and debugging support
- Hex dump output
- Cross-platform bytecode format

### 7. IR to Bytecode Translation (`src/compiler/emitter/ir_to_bytecode.c`) ✅

**Purpose**: Map IR instructions to bytecode instructions

**Key Features**:
- IR instruction to bytecode mapping
- Function and block translation
- Operand handling and optimization
- String table management
- Relocation table generation

## Modular Design Principles

### 1. **Separation of Concerns**
Each stage has a single responsibility:
- Lexer: Tokenization only
- Parser: Syntax analysis only
- AST: Tree representation only
- IR: Three-address code representation
- AST to IR: Translation from AST to IR
- Bytecode: Executable bytecode generation
- IR to Bytecode: Translation from IR to bytecode

### 2. **Independent Testing**
Each module can be tested in isolation:
- Lexer tests with token streams
- Parser tests with AST validation
- AST tests with tree manipulation
- IR tests with instruction validation
- AST to IR tests with translation validation
- Bytecode tests with bytecode validation
- IR to Bytecode tests with mapping validation

### 3. **Clean Interfaces**
Well-defined APIs between modules:
- Lexer provides token stream
- Parser consumes tokens, produces AST
- AST provides tree operations
- IR provides instruction operations
- AST to IR consumes AST, produces IR
- Bytecode provides bytecode operations
- IR to Bytecode consumes IR, produces bytecode

### 4. **Error Handling**
Comprehensive error reporting at each stage:
- Lexer: Invalid characters, unterminated strings
- Parser: Syntax errors, missing tokens, error recovery
- AST: Memory allocation failures
- IR: Invalid instruction creation, type mismatches
- AST to IR: Translation errors, symbol table issues
- Bytecode: Encoding errors, table overflow
- IR to Bytecode: Mapping errors, operand issues

## Build System

### Makefile Structure
```makefile
# Source organization
LEXER_SOURCES = src/compiler/lexer/lexer.c
PARSER_SOURCES = src/compiler/parser/parser.c
AST_SOURCES = src/compiler/ast/ast.c

# Compilation flags
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Isrc/include

# Build targets
he3: lexer parser ast main
    $(CC) $(CFLAGS) -o $@ $^
```

### Dependencies
- **C99 Compiler**: GCC, Clang, or MSVC
- **Make**: GNU Make or compatible
- **Standard Library**: Only C99 standard library

## Testing Strategy

### 1. **Unit Tests**
- Individual module testing
- Mock dependencies
- Edge case coverage

### 2. **Integration Tests**
- End-to-end compilation
- Example program validation
- Error case testing

### 3. **Regression Tests**
- Golden file comparisons
- AST snapshot testing
- Performance benchmarks

## Future Extensions

### Planned Modules
1. **Type Resolver** (`src/compiler/resolver/`)
2. **Semantic Analyzer** (`src/compiler/semantic/`)
3. **Virtual Machine Execution Engine** (`src/vm/execution/`)
4. **Memory Management** (`src/vm/memory/`)
5. **Object System** (`src/vm/object/`)

### Extension Points
- Plugin architecture for language extensions
- Custom optimization passes
- Alternative backends (LLVM, native code)

## Performance Considerations

### Memory Management
- Pool allocation for AST nodes
- String interning for identifiers
- Garbage collection for VM

### Parsing Performance
- Recursive descent with memoization
- Lazy evaluation for large files
- Streaming parser for very large files

## Error Recovery

### Lexer Recovery
- Skip invalid characters
- Continue after errors
- Report multiple errors

### Parser Recovery
- Synchronization points
- Panic mode recovery
- Best-effort parsing

## Debugging Support

### AST Visualization
- Tree printing with indentation
- Node type identification
- Source location tracking

### Token Debugging
- Token stream printing
- Position information
- Error context

---

This architecture provides a solid foundation for the He³ compiler while maintaining flexibility for future enhancements and optimizations.