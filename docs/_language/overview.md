---
layout: page
title: Language Overview
---
# He³ Language Overview

## 🚀 Implementation Status

### ✅ **Fully Implemented**
- **Lexer**: Complete tokenization of all language elements
- **Parser**: AST generation for all language constructs including OO features
- **Type System**: Built-in types and type parsing
- **Object-Oriented Features**: Complete OO system with classes, methods, objects
- **Static Methods**: Complete static method infrastructure with native implementations
- **Built-in System Functions**: Full `Sys` class with static methods for I/O and utilities
- **Method System**: Functions, virtual and static method calls
- **Domain System**: Module boundaries and qualified names
- **IR Generation**: Complete three-address code representation
- **AST to IR Translation**: Expression, statement, and method translation
- **Bytecode Generation**: Complete bytecode writer and IR to bytecode translation
- **Project Packaging**: Complete `.bx` → `.helium3` conversion
- **Module System**: Complete `.helium3` module loading and execution
- **Virtual Machine**: Complete execution engine for bytecode and modules
- **Memory Management**: Garbage collection and object lifecycle
- **Module Registry**: Complete class and method discovery system

### ⚠️ **Known Limitations**
- **Cross-Module Imports**: Not yet implemented
- **Advanced OO Features**: Inheritance and interfaces not yet implemented
- **String Table Loading**: Currently using fallback for string constants

### 🎯 **Current Status: FULLY FUNCTIONAL**
The complete build and execution pipeline is working:
- ✅ Source compilation (`.he3` → `.bx`)
- ✅ Project packaging (`.bx` → `.helium3`)
- ✅ Module execution (`.helium3` → VM)

## Design Goals
- **Simple to learn**, modern syntax.
- **64-bit by default** (integers, floats, pointers).
- **UTF-8 strings** as first-class citizens.
- **Unified object model**: everything inherits `System.Object`.
- **Memory safety**: nullable types (`T?`), no raw pointers in user code.
- **Explicit modularity**: `domain` + `crowd` for namespace + security boundaries.
- **Compile-time security**: Security keys and session-based authentication.
- **Event system**: Cross-thread communication with type-safe events.
- **Async/await** for concurrency.
- **Option/Result** for safe error handling.
- **Pattern matching** for control flow.

## Inspirations
- **Object Pascal**: clean type system, clarity.
- **Java/C#**: familiar syntax, OO model.
- **Rust/Swift**: safe error handling and option types.
- **.NET CLR**: unified runtime, everything as `System.Object`.
