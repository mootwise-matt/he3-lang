# He³ Compiler Architecture

## Overview

The He³ compiler is designed as a modular, multi-stage pipeline that transforms He³ source code into executable bytecode. The architecture emphasizes separation of concerns, testability, and maintainability.

## Pipeline Stages

### 1. Lexical Analysis (`src/compiler/lexer/`)

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

### 2. Syntax Analysis (`src/compiler/parser/`)

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
- Comprehensive error reporting
- Support for all He³ language constructs

### 3. Abstract Syntax Tree (`src/compiler/ast/`)

**Purpose**: Represent parsed code as a tree structure

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
- `AST_FUNCTION_CALL` - Function calls
- `AST_BINARY_OP` - Binary operations
- `AST_UNARY_OP` - Unary operations
- `AST_LITERAL` - Literal values
- `AST_IDENTIFIER` - Identifiers

**Features**:
- Modular design with separate header/implementation
- Tree traversal and manipulation functions
- Pretty printing for debugging
- Memory management for tree nodes

## Modular Design Principles

### 1. **Separation of Concerns**
Each stage has a single responsibility:
- Lexer: Tokenization only
- Parser: Syntax analysis only
- AST: Tree representation only

### 2. **Independent Testing**
Each module can be tested in isolation:
- Lexer tests with token streams
- Parser tests with AST validation
- AST tests with tree manipulation

### 3. **Clean Interfaces**
Well-defined APIs between modules:
- Lexer provides token stream
- Parser consumes tokens, produces AST
- AST provides tree operations

### 4. **Error Handling**
Comprehensive error reporting at each stage:
- Lexer: Invalid characters, unterminated strings
- Parser: Syntax errors, missing tokens
- AST: Memory allocation failures

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
1. **Type Resolver** (`src/compiler/type_resolver/`)
2. **Semantic Analyzer** (`src/compiler/semantic/`)
3. **IR Generator** (`src/compiler/ir/`)
4. **Bytecode Emitter** (`src/compiler/emitter/`)
5. **Virtual Machine** (`src/vm/`)

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