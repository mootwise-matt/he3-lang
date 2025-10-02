# He³ Compiler Architecture

## Overview

The He³ compiler implements a **three-stage build process** that transforms source code into executable modules:

1. **Compilation**: `.he3` → `.bx` (raw bytecode)
2. **Packaging**: `.bx` → `.helium3` (packaged module)
3. **Execution**: `.helium3` → VM execution

## Architecture Diagram

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   .he3      │    │     .bx     │    │  .helium3   │    │   VM        │
│  Source     │───▶│   Bytecode  │───▶│   Module    │───▶│ Execution   │
│   Files     │    │   (Raw)     │    │ (Packaged)  │    │             │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │                   │
       ▼                   ▼                   ▼                   ▼
   Compiler            Packager            Module Loader        VM Engine
   (he3)              (he3build)          (VM .helium3)        (he3vm)
```

## Stage 1: Compilation (.he3 → .bx)

### Components

#### Lexer (`src/compiler/lexer/`)
- **Purpose**: Tokenize source code into tokens
- **Input**: He³ source code (`.he3` files)
- **Output**: Token stream
- **Status**: ✅ Complete

#### Parser (`src/compiler/parser/`)
- **Purpose**: Build Abstract Syntax Tree (AST) from tokens
- **Input**: Token stream
- **Output**: AST representation
- **Status**: ⚠️ Partial (basic constructs work, complex OO has issues)

#### AST to IR (`src/compiler/emitter/ast_to_ir.c`)
- **Purpose**: Convert AST to Intermediate Representation
- **Input**: AST
- **Output**: IR instructions
- **Status**: ✅ Complete

#### IR to Bytecode (`src/compiler/emitter/ir_to_bytecode.c`)
- **Purpose**: Convert IR to executable bytecode
- **Input**: IR instructions
- **Output**: Raw bytecode (`.bx` files)
- **Status**: ✅ Complete

### Compilation Pipeline

```
Source Code (.he3)
       ↓
   Lexical Analysis
       ↓
   Parsing (AST)
       ↓
   IR Generation
       ↓
   Bytecode Generation
       ↓
   Raw Bytecode (.bx)
```

### Output Format (.bx)

The `.bx` format contains:
- **Header**: Magic number, version, basic metadata
- **String Table**: Identifiers and string literals
- **Constant Table**: Numeric and string constants
- **Type Table**: Class and type definitions
- **Method Table**: Function and method definitions
- **Bytecode**: Executable instructions

## Stage 2: Packaging (.bx → .helium3)

### Components

#### Packager (`src/compiler/packager/`)
- **Purpose**: Package multiple `.bx` files into `.helium3` modules
- **Input**: `he3project.json` + `.bx` files
- **Output**: `.helium3` module files
- **Status**: ❌ Broken (parser issues)

#### Project Configuration
- **File**: `he3project.json`
- **Purpose**: Define project structure and dependencies
- **Features**: Module names, entry points, build settings

### Packaging Process

```
Project Configuration (he3project.json)
       ↓
   Source Discovery
       ↓
   Dependency Resolution
       ↓
   Sys Proxy Integration
       ↓
   Module Creation
       ↓
   Packaged Module (.helium3)
```

### Output Format (.helium3)

The `.helium3` format contains:
- **Header**: Complete module metadata
- **Manifest**: Module information and dependencies
- **Sys Proxy**: Built-in functions and system classes
- **String Table**: All identifiers and literals
- **Type Table**: Complete type information
- **Method Table**: All methods with metadata
- **Bytecode**: Executable instructions
- **Security**: Security boundaries and permissions

## Stage 3: Execution (.helium3 → VM)

### Components

#### VM Module Loader (`src/vm/bytecode/helium_module.c`)
- **Purpose**: Load `.helium3` modules into VM
- **Input**: `.helium3` module files
- **Output**: Loaded module in memory
- **Status**: ❌ Broken (segfaults)

#### VM Engine (`src/vm/`)
- **Purpose**: Execute loaded modules
- **Input**: Loaded modules
- **Output**: Program execution results
- **Status**: ✅ Complete (for .bx files)

### Execution Process

```
Packaged Module (.helium3)
       ↓
   Module Loading
       ↓
   Sys Initialization
       ↓
   Method Resolution
       ↓
   Execution
       ↓
   Program Results
```

## Current Status

### ✅ **Working Components**
- **Lexer**: Complete tokenization
- **Basic Parser**: Simple constructs work
- **IR Generation**: Complete IR pipeline
- **Bytecode Generation**: Complete bytecode output
- **VM .bx Execution**: Raw bytecode execution works
- **Memory Management**: Garbage collection and reference counting

### ⚠️ **Partially Working Components**
- **Complex Parser**: OO syntax causes issues
- **Module Registry**: Basic functionality works
- **Object System**: Basic classes and methods work

### ❌ **Broken Components**
- **Packager**: Parser issues prevent packaging
- **VM .helium3 Loader**: Segfaults when loading modules
- **Sys Integration**: Built-in functions not available
- **Cross-Module Imports**: Don't work properly

## File Structure

```
src/
├── compiler/                 # Compilation stage
│   ├── lexer/               # Lexical analysis
│   ├── parser/              # Parsing and AST
│   ├── emitter/             # IR and bytecode generation
│   └── packager/            # Packaging stage
├── vm/                      # Execution stage
│   ├── bytecode/            # Module loading
│   ├── execution/           # VM engine
│   ├── memory/              # Memory management
│   └── modules/             # Module registry
└── shared/                  # Shared components
    ├── ast/                 # AST definitions
    ├── bytecode/            # Bytecode formats
    └── stdlib/              # Standard library
```

## Data Flow

### Compilation Data Flow
```
.he3 source → tokens → AST → IR → bytecode → .bx file
```

### Packaging Data Flow
```
he3project.json + .bx files → project analysis → module creation → .helium3 file
```

### Execution Data Flow
```
.helium3 file → module loading → VM initialization → execution → results
```

## Error Handling

### Compilation Errors
- **Lexical Errors**: Invalid tokens or syntax
- **Parse Errors**: Grammar violations or missing declarations
- **Type Errors**: Incompatible types or missing methods
- **IR Errors**: Invalid intermediate representation

### Packaging Errors
- **Project Errors**: Invalid project configuration
- **Dependency Errors**: Missing or circular dependencies
- **Module Errors**: Invalid bytecode or metadata

### Execution Errors
- **Loading Errors**: Invalid module format or missing dependencies
- **Runtime Errors**: Execution-time issues or memory problems
- **System Errors**: VM or system-level failures

## Debugging Support

### Compiler Debugging
- **Lexical Debugging**: Token-level analysis
- **Parse Debugging**: AST construction analysis
- **IR Debugging**: Intermediate representation inspection
- **Bytecode Debugging**: Generated bytecode analysis

### VM Debugging
- **Module Debugging**: Module loading and registration
- **Execution Debugging**: Instruction-level execution
- **Memory Debugging**: Memory allocation and garbage collection
- **Stack Debugging**: Call stack and variable inspection

## Performance Considerations

### Compilation Performance
- **Lexical Analysis**: O(n) tokenization
- **Parsing**: O(n) AST construction
- **IR Generation**: O(n) IR creation
- **Bytecode Generation**: O(n) bytecode output

### Packaging Performance
- **Project Analysis**: O(n) dependency resolution
- **Module Creation**: O(n) module construction
- **Sys Integration**: O(1) built-in function addition

### Execution Performance
- **Module Loading**: O(n) module parsing and registration
- **Execution**: O(n) instruction execution
- **Memory Management**: O(n) garbage collection

## Future Enhancements

### Compiler Improvements
- **Incremental Compilation**: Only recompile changed files
- **Parallel Compilation**: Compile multiple files simultaneously
- **Advanced Optimizations**: Code optimization and dead code elimination
- **Better Error Recovery**: Improved error handling and recovery

### VM Improvements
- **JIT Compilation**: Just-in-time compilation for performance
- **Advanced Garbage Collection**: More sophisticated memory management
- **Profiling Support**: Performance profiling and analysis
- **Debugging Tools**: Enhanced debugging capabilities

## Conclusion

The He³ compiler architecture provides a solid foundation for a modern programming language with:

- **Clear Separation**: Distinct stages for compilation, packaging, and execution
- **Modular Design**: Each component can be developed and tested independently
- **Extensibility**: Easy to add new features and optimizations
- **Debugging**: Comprehensive debugging support at each stage

The current implementation successfully demonstrates the core concepts, with the main focus now on completing the packaging and module loading stages to achieve full end-to-end functionality.
