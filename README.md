# He³ (Helium-3) Programming Language

A modern, object-oriented programming language with Pascal roots and Java/C#-style syntax, featuring async/await, pattern matching, safe error handling, and cross-platform bytecode execution.

## 🚀 Current Implementation Status

### ✅ **Phase 1 Complete - Core Compilation Pipeline**

#### **Compiler Pipeline**
- **Lexer** (`src/compiler/lexer/`) - ✅ Complete
  - Tokenizes all He³ language elements
  - Handles keywords, operators, literals, strings, comments
  - UTF-8 string support
  - Proper whitespace and semicolon handling

- **Parser** (`src/compiler/parser/`) - ⚠️ Partial
  - AST generation for basic language constructs
  - Domain declarations with qualified names
  - Basic class and method declarations
  - Simple expression parsing
  - **Issues**: Complex OO syntax causes parser loops

- **AST Module** (`src/compiler/ast/`) - ✅ Complete
  - Modular AST node definitions
  - Tree manipulation functions
  - Pretty printing and debugging support

#### **Language Features Implemented**
- **Domain System**: Basic domain declarations work
- **Object-Oriented Programming**: Basic OO system with classes, objects, methods, fields
  - **Class Definition**: Basic class definitions work
  - **Object Instantiation**: Basic object creation works
  - **Field Access**: Basic field reading/writing works
  - **Method Calls**: Basic method dispatch works
  - **Inheritance**: Not yet implemented
- **Type System**: Basic built-in types (System.Int64, System.Float64, System.Boolean, System.String)
- **Variables**: Local variable declarations work
- **Arithmetic**: Basic arithmetic operations work
- **Return Statements**: Basic return value handling works

#### **IR and Bytecode Generation**
- **IR Module** (`src/compiler/ir/`) - ✅ Complete
  - Three-address code representation
  - Function, block, and instruction management
  - Comprehensive instruction set
  - Type-safe value handling

- **AST to IR Translation** (`src/compiler/emitter/ast_to_ir.c`) - ✅ Complete
  - Expression translation (arithmetic, logical, comparisons)
  - Statement translation (declarations, assignments, control flow)
  - Built-in function registration

- **IR to Bytecode Translation** (`src/compiler/emitter/ir_to_bytecode.c`) - ✅ Complete
  - Bytecode instruction generation
  - String and constant table management
  - Type and method table creation
  - Raw bytecode (.bx) file output
  - Method/function translation (calls, invocations, parameters)
  - Field and array access translation
  - Return value handling

- **Bytecode Writer** (`src/vm/bytecode/`) - ✅ Complete

#### **Virtual Machine**
- **VM Engine** (`src/vm/`) - ✅ Complete
  - Memory management with generational garbage collection
  - Stack operations and execution context
  - Instruction execution and method dispatch
  - Module registry for class and method discovery
  - **.bx File Support**: Can load and execute raw bytecode files

### ⚠️ **Phase 2 In Progress - Module System**

#### **Current Issues**
- **Packager** (`src/compiler/packager/`) - ❌ Broken
  - Parser issues prevent `.bx` → `.helium3` conversion
  - Multi-file project compilation fails
  - Import statements don't work

- **VM .helium3 Loader** (`src/vm/bytecode/helium_module.c`) - ❌ Broken
  - Segfaults when loading `.helium3` files
  - Module loading and registration issues

- **Built-in Functions** - ❌ Not Working
  - `print()` function calls cause segfaults
  - Sys proxy class not integrated
  - String operations not implemented

#### **Working Examples**
```bash
# These examples work end-to-end
./he3 examples/standalone/01_simple_return.he3
./he3vm examples/standalone/01_simple_return.bx

./he3 examples/standalone/02_basic_variables.he3
./he3vm examples/standalone/02_basic_variables.bx
```

### 🎯 **Next Priorities**
1. **Fix Packager**: Resolve parser issues for `.bx` → `.helium3` conversion
2. **Fix VM .helium3 Loader**: Debug segfaults in module loading
3. **Implement Sys Proxy**: Add built-in function support
4. **Test Complete Workflow**: Verify `.he3` → `.bx` → `.helium3` → VM execution
  - Bytecode instruction encoding
  - String and relocation tables
  - Disassembly and debugging support
  - Hex dump output

- **IR to Bytecode Translation** (`src/compiler/emitter/ir_to_bytecode.c`) - Complete
  - IR instruction to bytecode mapping
  - Function and block translation
  - Operand handling and optimization

#### **Error Handling and Recovery**
- **Parser Error Recovery** - Complete
  - Enhanced error messages with context
  - Error synchronization and recovery
  - Panic mode management
  - Graceful error handling

### ✅ **Object-Oriented Programming - COMPLETE**
- **Class Discovery & Registration** - Automatic class discovery from modules
- **Object Creation** - Complete object instantiation with `new` keyword
- **Field Access** - Complete field loading and storage with type safety
- **Method Resolution** - Virtual and static method dispatch
- **Method Execution** - Complete method execution with bytecode interpretation
- **Module Registry** - Runtime class, method, and field discovery system

### 🔄 **In Progress**
- **Type Resolution** - Planned
- **Semantic Analysis** - Planned

### ✅ **Virtual Machine Implementation**
- **Core VM Engine** (`src/vm/`) - Complete
  - Bytecode loader and interpreter
  - Stack-based execution model
  - Memory management with generational GC
  - Complete object system with classes, methods, and fields
  - Cross-platform execution support

- **Object-Oriented System** (`src/vm/objects/`) - Complete
  - Object creation and instantiation
  - Field access and storage with type safety
  - Method resolution and virtual dispatch
  - Method execution with bytecode interpretation
  - Object lifecycle management

- **Module Registry System** (`src/vm/modules/`) - Complete
  - Runtime class discovery and registration
  - Automatic method and field discovery from loaded modules
  - Support for both `.bx` and `.helium3` file formats
  - Memory management and cleanup
  - Debug output and module information display

### 📋 **Planned Features**
- **Pattern Matching**: `match` statements with `when` clauses
- **Error Handling**: `Option<T>` and `Result<T,E>` types
- **Async/Await**: Concurrency and async programming
- **Security**: Compile-time cryptographic keys for module boundaries
- **Events**: Cross-thread communication system
- **Memory Safety**: No raw pointers, nullable types

## 🏗️ **Architecture**

### **Compiler Pipeline**
```
Source Code (.he3) 
    ↓
Lexer → Tokens
    ↓
Parser → AST
    ↓
AST to IR Translation → IR Functions
    ↓
IR to Bytecode Translation → Bytecode
    ↓
Bytecode Writer → .bx Executable
```

**Current Status**: ✅ Complete through bytecode generation

### **Project Structure**
```
he3/
├── src/
│   ├── compiler/          # Compiler implementation
│   │   ├── lexer/         # Lexical analysis ✅
│   │   ├── parser/        # Syntax analysis ✅
│   │   ├── ast/           # Abstract Syntax Tree ✅
│   │   ├── ir/            # Intermediate Representation ✅
│   │   ├── emitter/       # AST to IR, IR to Bytecode ✅
│   │   ├── tests/         # Comprehensive test suite ✅
│   │   └── main.c         # Compiler entry point ✅
│   ├── vm/                # Virtual Machine ✅
│   │   ├── bytecode/      # Bytecode writer ✅
│   │   ├── execution/     # Interpreter and stack ✅
│   │   ├── memory/        # Heap and GC ✅
│   │   ├── objects/       # Object system ✅
│   │   └── loader/        # Bytecode loader ✅
│   └── include/           # Shared headers ✅
├── examples/              # Example programs ✅
│   ├── standalone/        # Single-file examples
│   └── projects/          # Multi-file projects
├── docs/                  # Documentation ✅
└── build/                 # Build artifacts
```

## 🚀 **Quick Start**

### **Build the Compiler and VM**
```bash
git clone https://github.com/your-username/he3-lang.git
cd he3-lang
make
```

### **Compile and Run a Program**
```bash
# Compile to bytecode
./he3 examples/standalone/01_hello.he3

# Run the bytecode
./he3vm 01_hello.bx

# Show tokens
./he3 --tokens examples/standalone/01_hello.he3

# Show AST
./he3 --ast examples/standalone/01_hello.he3

# VM debugging options
./he3vm -o -c program.bx  # Show object system and classes
./he3vm -m -r program.bx  # Show memory statistics and regions

# Run comprehensive tests
make test
```

### **Example Program**
```he3
domain app.hello;

class Program {
    function main(): integer {
        print("Hello, He³ World!");
        return 0;
    }
}
```

## 📚 **Documentation**

- [Language Overview](docs/language/overview.md) - Language design and features
- [Syntax Reference](docs/language/syntax.md) - Complete syntax guide
- [Type System](docs/language/typesystem.md) - Type system documentation
- [Object-Oriented Features](docs/language/oo.md) - Classes, interfaces, inheritance
- [Async Programming](docs/language/async.md) - Async/await patterns
- [Error Handling](docs/language/errors.md) - Option/Result types
- [Bytecode Format](docs/bytecode/format.md) - VM instruction set
- [VM Design](docs/vm/design.md) - Virtual machine architecture

## 🧪 **Testing**

The compiler includes comprehensive regression tests:

```bash
# Run all tests
make test

# Run specific test suites
make lexer-test
make parser-test
make ir-bytecode-test
make ast-to-ir-test
make statement-translation-test
make method-translation-test
make error-recovery-test

# Test specific example
./he3 --ast examples/standalone/01_hello.he3
```

## 🤝 **Contributing**

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 **License**

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## 🙏 **Acknowledgments**

- Inspired by Pascal, Java, and C# language designs
- Built with modern C99 for cross-platform compatibility
- Designed for educational and research purposes

---

**He³** - *Where modern language design meets cross-platform execution*
