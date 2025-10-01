# He³ (Helium-3) Programming Language

A modern, object-oriented programming language with Pascal roots and Java/C#-style syntax, featuring async/await, pattern matching, safe error handling, and cross-platform bytecode execution.

## 🚀 Current Implementation Status

### ✅ **Completed Components**

#### **Compiler Pipeline**
- **Lexer** (`src/compiler/lexer/`) - Complete
  - Tokenizes all He³ language elements
  - Handles keywords, operators, literals, strings, comments
  - UTF-8 string support
  - Proper whitespace and semicolon handling

- **Parser** (`src/compiler/parser/`) - Complete
  - AST generation for all language constructs
  - Domain declarations with qualified names
  - Class, record, enum, interface declarations
  - Method, constructor, property declarations
  - Field declarations with types and initializers
  - Expression parsing with precedence climbing
  - Type parsing (built-in types, identifiers, generics)

- **AST Module** (`src/compiler/ast/`) - Complete
  - Modular AST node definitions
  - Tree manipulation functions
  - Pretty printing and debugging support

#### **Language Features Implemented**
- **Domain System**: Secure module boundaries with qualified names
- **Object-Oriented**: Classes, records, interfaces, inheritance
- **Type System**: Built-in types (integer, float, boolean, string, void, object)
- **Generics**: Type parameters and type arguments
- **Enums**: Simple variants and associated data types
- **Methods**: Async methods, parameters, return types
- **Properties**: Getter/setter blocks
- **Constructors**: Parameterized constructors
- **Fields**: Mutable (`var`) and immutable (`let`) fields

#### **IR and Bytecode Generation**
- **IR Module** (`src/compiler/ir/`) - Complete
  - Three-address code representation
  - Function, block, and instruction management
  - Comprehensive instruction set
  - Type-safe value handling

- **AST to IR Translation** (`src/compiler/emitter/ast_to_ir.c`) - Complete
  - Expression translation (arithmetic, logical, comparisons)
  - Statement translation (declarations, assignments, control flow)
  - Method/function translation (calls, invocations, parameters)
  - Field and array access translation
  - Return value handling

- **Bytecode Writer** (`src/vm/bytecode/`) - Complete
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

### 🔄 **In Progress**
- **Type Resolution** - Planned
- **Semantic Analysis** - Planned
- **VM Integration** - Connect compiler pipeline to VM execution

### ✅ **Virtual Machine Implementation**
- **Core VM Engine** (`src/vm/`) - Complete
  - Bytecode loader and interpreter
  - Stack-based execution model
  - Memory management with generational GC
  - Object system with classes, methods, and fields
  - Cross-platform execution support

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
