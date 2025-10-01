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

### 🔄 **In Progress**
- **Virtual Machine** (`src/vm/`) - Planned
- **Type Resolution** - Planned
- **IR Generation** - Planned
- **Bytecode Emission** - Planned

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
Type Resolver → TypeTable
    ↓
Semantic Analysis → Diagnostics
    ↓
IR Lowering → IR Functions
    ↓
Emitter → .ohe3 Object
    ↓
Linker → .bx Executable
```

### **Project Structure**
```
he3/
├── src/
│   ├── compiler/          # Compiler implementation
│   │   ├── lexer/         # Lexical analysis
│   │   ├── parser/        # Syntax analysis
│   │   ├── ast/           # Abstract Syntax Tree
│   │   └── main.c         # Compiler entry point
│   ├── vm/                # Virtual Machine (planned)
│   └── include/           # Shared headers
├── examples/              # Example programs
│   ├── standalone/        # Single-file examples
│   └── projects/          # Multi-file projects
├── docs/                  # Documentation
└── build/                 # Build artifacts
```

## 🚀 **Quick Start**

### **Build the Compiler**
```bash
git clone https://github.com/your-username/he3-lang.git
cd he3-lang
make
```

### **Compile a Program**
```bash
# Compile to bytecode
./he3 examples/standalone/01_hello.he3

# Show tokens
./he3 --tokens examples/standalone/01_hello.he3

# Show AST
./he3 --ast examples/standalone/01_hello.he3
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
# Run all examples
make test

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
