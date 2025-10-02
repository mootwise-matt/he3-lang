---
layout: default
title: He³ (Helium-3) Programming Language
description: A modern, object-oriented programming language with Pascal roots and Java/C#-style syntax
---

# He³ (Helium-3) Programming Language

<div align="center">

![He³ Logo](https://img.shields.io/badge/He³-Helium--3-blue?style=for-the-badge&logo=atom&logoColor=white)
![Version](https://img.shields.io/badge/version-0.1.0-green?style=for-the-badge)
![License](https://img.shields.io/badge/license-Apache%202.0-blue?style=for-the-badge)
![Platform](https://img.shields.io/badge/platform-cross--platform-lightgrey?style=for-the-badge)

**A modern, object-oriented programming language with Pascal roots and Java/C#-style syntax**

### Navigation
[![View on GitHub](https://img.shields.io/badge/View%20on-GitHub-black?style=for-the-badge&logo=github)](https://github.com/mootwise-matt/he3-lang) • 
[Quick Start](#-quick-start) • 
[Documentation](#-documentation) • 
[Examples](#-examples) • 
[Contributing](#-contributing)

</div>

---

## 🚀 What is He³?

He³ (Helium-3) is a **64-bit, UTF-8, object-oriented programming language** designed for modern software development. It combines the elegance of Pascal with the power of Java/C#, featuring:

- **🔄 Async/Await** - Built-in concurrency and asynchronous programming
- **🎯 Pattern Matching** - Powerful `match` statements with `when` clauses  
- **🛡️ Memory Safety** - No raw pointers, nullable types, and safe error handling
- **🔒 Security** - Compile-time cryptographic keys for module boundaries
- **🌐 Cross-Platform** - Compiles to portable bytecode for ARM, Apple Silicon, and Intel
- **⚡ Performance** - Generational garbage collection and optimized execution

## ✨ Key Features

### **Object-Oriented Design** ✅ **FULLY IMPLEMENTED**
```he3
domain app.calculator;

class Calculator {
    var value: float64 = 0.0;
    
    function add(x: float64): Calculator {
        this.value += x;
        return this;
    }
    
    function getValue(): float64 {
        return this.value;
    }
}
```

### **Async Programming**
```he3
async function fetchData(url: string): Task<Result<string, Error>> {
    let response = await http.get(url);
    return response.isOk() ? Ok(response.data) : Err(response.error);
}
```

### **Pattern Matching**
```he3
function processResult(result: Result<Data, Error>): string {
    match result {
        Ok(data) when data.isValid() => "Success: " + data.toString(),
        Ok(data) => "Warning: Invalid data",
        Err(error) => "Error: " + error.message
    }
}
```

### **Memory Safety**
```he3
function safeAccess(items: Option<Array<string>>, index: int64): Option<string> {
    return items.andThen(array => 
        array.get(index).ok()
    );
}
```

## 🏗️ Architecture

### **Compiler Pipeline**
```
Source Code (.he3) 
    ↓
Lexer → Tokens
    ↓
Parser → AST
    ↓
Type Resolver → Type Table
    ↓
Semantic Analysis → Diagnostics
    ↓
IR Lowering → IR Functions
    ↓
Emitter → .ohe3 Object
    ↓
Linker → .bx Executable
```

### **Virtual Machine**
```
Bytecode (.bx)
    ↓
VM Loader → Module Manifest
    ↓
Object System → Classes & Methods
    ↓
Memory Manager → Heap & GC
    ↓
Interpreter → Execution
```

## 🚀 Quick Start

### **Installation**
```bash
git clone https://github.com/mootwise-matt/he3-lang.git
cd he3-lang
make
```

### **Your First Program**
Create `hello.he3`:
```he3
domain app.hello;

class Program {
    function main(): int64 {
        print("Hello, He³ World!");
        return 0;
    }
}
```

### **Compile and Run**
```bash
# Compile to bytecode
./he3 hello.he3

# Run the bytecode
./he3vm hello.bx

# Show compilation details
./he3 --ast hello.he3
./he3 --tokens hello.he3
```

## 📚 Documentation

### **Language Reference**
- [**Language Overview**]({{ site.baseurl }}/language/overview/) - Core concepts and design philosophy
- [**Syntax Guide**]({{ site.baseurl }}/language/syntax/) - Complete syntax reference
- [**Type System**]({{ site.baseurl }}/language/typesystem/) - Types, generics, and type safety
- [**Object-Oriented Features**]({{ site.baseurl }}/language/oo/) - Classes, interfaces, inheritance
- [**Async Programming**]({{ site.baseurl }}/language/async/) - Concurrency and async patterns
- [**Error Handling**]({{ site.baseurl }}/language/errors/) - Option/Result types and error management
- [**Security Model**]({{ site.baseurl }}/language/security/) - Module boundaries and cryptographic keys
- [**Event System**]({{ site.baseurl }}/language/events/) - Cross-thread communication

### **Compiler Documentation**
- [**Project Guide**]({{ site.baseurl }}/compiler/project-guide/) - Project structure and build process
- [**Architecture**]({{ site.baseurl }}/compiler/architecture/) - Compiler design and components
- [**Module Contracts**]({{ site.baseurl }}/compiler/module-contracts/) - Module system and dependencies
- [**Testing Guide**]({{ site.baseurl }}/compiler/testing/) - Testing strategies and tools

### **Virtual Machine Documentation**
- [**VM Design**]({{ site.baseurl }}/vm/design/) - Virtual machine architecture
- [**Module Registry**]({{ site.baseurl }}/vm/module-registry/) - Runtime class discovery and registration system

### **Source Code**
- [**GitHub Repository**](https://github.com/mootwise-matt/he3-lang) - Complete source code
- [**Browse Source**](https://github.com/mootwise-matt/he3-lang/tree/main/src) - Source code structure
- [**Issues & Discussions**](https://github.com/mootwise-matt/he3-lang/issues) - Bug reports and feature requests
- [**Memory Management**]({{ site.baseurl }}/vm/memory-management/) - Heap, GC, and memory safety
- [**Object Model**]({{ site.baseurl }}/vm/object-model/) - Object system and method dispatch
- [**Bytecode Format**]({{ site.baseurl }}/bytecode/format/) - Instruction set and file format
- [**VM Roadmap**]({{ site.baseurl }}/vm/roadmap/) - Development phases and future plans

### **Bytecode Reference**
- [**Instruction Set**]({{ site.baseurl }}/bytecode/instruction-set/) - Complete opcode reference
- [**Object Files**]({{ site.baseurl }}/bytecode/objects/) - .ohe3 object file format
- [**Module Format**]({{ site.baseurl }}/bytecode/helium-module-format/) - .helium3 module format
- [**Examples**]({{ site.baseurl }}/bytecode/examples/) - Bytecode examples and patterns

### **Exporters**
- [**C Transpiler**]({{ site.baseurl }}/exporter/c-transpiler/) - C code generation
- [**LLVM Backend**]({{ site.baseurl }}/exporter/llvm/) - LLVM IR generation
- [**Native Assembly**]({{ site.baseurl }}/exporter/native/) - Direct assembly output

## 🧪 Examples

### **Standalone Examples**
- [**Hello World**](https://github.com/mootwise-matt/he3-lang/blob/main/examples/standalone/01_hello.he3) - Basic program structure
- [**Calculator**](https://github.com/mootwise-matt/he3-lang/blob/main/examples/standalone/02_calculator.he3) - Object-oriented programming
- [**Security Events**](https://github.com/mootwise-matt/he3-lang/blob/main/examples/standalone/03_security_events.he3) - Module security

### **Project Examples**
- [**Hello Project**](https://github.com/mootwise-matt/he3-lang/tree/main/examples/projects/01_hello_project) - Multi-file project structure
  - [main.he3](https://github.com/mootwise-matt/he3-lang/blob/main/examples/projects/01_hello_project/src/main.he3) - Main program entry point
  - [utils.he3](https://github.com/mootwise-matt/he3-lang/blob/main/examples/projects/01_hello_project/src/utils.he3) - Utility functions
  - `he3project.json` - Project configuration

## 🛠️ Development Status

### ✅ **Completed Components**

#### **Compiler Pipeline**
- **Lexer** - Complete tokenization with UTF-8 support
- **Parser** - Full AST generation for all language constructs
- **IR System** - Three-address code representation
- **Bytecode Generation** - Complete .bx executable generation
- **Error Recovery** - Robust error handling and reporting

#### **Virtual Machine**
- **Core Engine** - Stack-based interpreter with PC/SP/FP registers
- **Memory Management** - Generational garbage collection (16MB heap)
- **Object System** - Classes, methods, fields, and inheritance
- **Module Registry** - Runtime class discovery and registration system
- **Bytecode Loader** - .bx and .helium3 file loading and module management
- **Cross-Platform** - ARM, Apple Silicon, Intel support

#### **Shared Architecture**
- **Unified AST** - Shared AST definitions between compiler and VM
- **Token System** - Unified token definitions for all components
- **Bytecode Format** - Shared bytecode format and instruction set
- **Type System** - Common type definitions and utilities
- **Memory Safety** - Consistent memory management across components

#### **Language Features**
- **Domain System** - Secure module boundaries
- **Object-Oriented Programming** - Complete OO system with classes, objects, methods, fields
  - **Class Definition** - Define classes with fields and methods
  - **Object Creation** - Create objects with `new` keyword
  - **Field Access** - Read and write object fields with type safety
  - **Method Calls** - Virtual and static method dispatch
  - **Method Execution** - Complete method execution with bytecode interpretation
- **Type System** - Built-in types and generics
- **Methods** - Async methods with parameters and return types
- **Fields** - Mutable and immutable field declarations
- **Constructors** - Parameterized object construction

### ✅ **Completed Integration**
- **Shared Architecture** - Unified AST and bytecode format between compiler and VM
- **Compiler-VM Pipeline** - Complete source-to-execution pipeline working
- **End-to-End Testing** - All examples compile and execute successfully

### ✅ **Object-Oriented Programming - COMPLETE**
- **Class Discovery & Registration** - Automatic class discovery from modules
- **Object Creation** - Complete object instantiation with `new` keyword
- **Field Access** - Complete field loading and storage with type safety
- **Method Resolution** - Virtual and static method dispatch
- **Method Execution** - Complete method execution with bytecode interpretation
- **Module Registry** - Runtime class, method, and field discovery system

### 📋 **Next Phase**
- **Type Resolution** - Semantic analysis and type checking
- **Library Support** - Multi-module project support
- **Build System** - Enhanced build system for complex projects

### 📋 **Planned Features**
- **Pattern Matching** - `match` statements with `when` clauses
- **Error Handling** - `Option<T>` and `Result<T,E>` types
- **Async/Await** - Concurrency and async programming
- **Security** - Compile-time cryptographic keys
- **Events** - Cross-thread communication system

## 🧪 Testing

The He³ compiler includes comprehensive regression testing:

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

# Test specific examples
./he3 --ast examples/standalone/01_hello.he3
```

## 🤝 Contributing

We welcome contributions! Here's how to get started:

**[📁 View Source Code on GitHub](https://github.com/mootwise-matt/he3-lang)** • **[🐛 Report Issues](https://github.com/mootwise-matt/he3-lang/issues)** • **[💬 Join Discussions](https://github.com/mootwise-matt/he3-lang/discussions)**

### **Getting Started**

1. **Fork the repository** from [GitHub](https://github.com/mootwise-matt/he3-lang)
2. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
3. **Make your changes** following our coding standards
4. **Add tests** for new functionality
5. **Update documentation** as needed
6. **Commit your changes** (`git commit -m 'Add amazing feature'`)
7. **Push to the branch** (`git push origin feature/amazing-feature`)
8. **Open a Pull Request**

### **Browse Source Code**
- **[📁 Complete Repository](https://github.com/mootwise-matt/he3-lang)** - Full source code
- **[🔧 Compiler Source](https://github.com/mootwise-matt/he3-lang/tree/main/src/compiler)** - Lexer, parser, AST, IR, emitter
- **[🖥️ VM Source](https://github.com/mootwise-matt/he3-lang/tree/main/src/vm)** - Virtual machine, memory management, object system
- **[📚 Documentation](https://github.com/mootwise-matt/he3-lang/tree/main/docs)** - All documentation files
- **[📝 Examples](https://github.com/mootwise-matt/he3-lang/tree/main/examples)** - Sample He³ programs
- **[⚙️ Build System](https://github.com/mootwise-matt/he3-lang/blob/main/Makefile)** - Makefile and build configuration

### **Development Guidelines**
- **Temporary Code**: All test files and debug code go in `/tmp/` (excluded from git)
- **Clean Codebase**: Keep the main codebase production-ready
- **Documentation**: Update docs for all new features
- **Testing**: Add tests for new functionality
- **C99 Standard**: Use modern C99 for cross-platform compatibility

## 📄 License

This project is licensed under the **Apache License 2.0** - see the [LICENSE](https://github.com/mootwise-matt/he3-lang/blob/main/LICENSE) file for details.

## 🙏 Acknowledgments

- **Language Design**: Inspired by Pascal, Java, and C#
- **Implementation**: Built with modern C99 for cross-platform compatibility
- **Purpose**: Designed for educational and research purposes
- **Community**: Open source development and collaboration

---

<div align="center">

**He³** - *Where modern language design meets cross-platform execution*

[![GitHub](https://img.shields.io/badge/GitHub-Repository-black?style=for-the-badge&logo=github)](https://github.com/mootwise-matt/he3-lang)
[![Issues](https://img.shields.io/badge/Issues-Report%20Bug-red?style=for-the-badge&logo=github)](https://github.com/mootwise-matt/he3-lang/issues)
[![Discussions](https://img.shields.io/badge/Discussions-Community-blue?style=for-the-badge&logo=github)](https://github.com/mootwise-matt/he3-lang/discussions)

</div>