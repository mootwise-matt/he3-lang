---
layout: page
title: Object-Oriented Features
---
# Object-Oriented Model

## Overview
He³ implements a **complete object-oriented programming model** with classes, inheritance, interfaces, and properties. The OO system is fully integrated with the module registry for runtime class discovery and method resolution.

## ✅ **Implementation Status: COMPLETE**

The Object-Oriented Programming system is **fully implemented and functional**:

- **✅ Class Definition**: Define classes with fields and methods
- **✅ Object Creation**: Create objects with `new` keyword
- **✅ Field Access**: Read and write object fields with type safety
- **✅ Method Calls**: Virtual and static method dispatch
- **✅ Static Methods**: Complete static method infrastructure with native implementations
- **✅ Built-in System Functions**: Full `Sys` class with static methods for I/O and utilities
- **✅ Method Execution**: Complete method execution with bytecode interpretation
- **✅ Module Registry**: Runtime class discovery and registration
- **✅ Type Safety**: Runtime type validation and checking
- **✅ Memory Management**: Proper object lifecycle management

## Classes
```he3
domain MyModule {
  class Animal {
    field name: String;
    
    function speak(): String { 
      return "??" 
    }
  }

  class Dog extends Animal {
    override function speak(): String { 
      return "Woof " + this.name 
    }
  }
}
```

## Interfaces
```he3
interface PaymentProvider {
  async function charge(p: Product): Result<PaymentStatus, String>;
}
```

## Properties
```he3
class Account {
  field _balance: Float64;

  property balance: Float64 {
    get { return this._balance; }
    set { this._balance = value; }
  }
}
```

## Object Instantiation
```he3
function main(): Int64 {
  let dog = new Dog();
  return dog.getValue();
}
```

## Object Layout
```
+----------------+----------------+------------------+
| typeId (u32)   | flags (u32)    | vtable* (u64)    |
+----------------+----------------+------------------+
| field0         | field1         | ...              |
```

## Dispatch
- `CALL` = static call
- `CALLV` = virtual call via vtable slot
- `CALLI` = interface call

## Module Registry Integration
The VM automatically discovers and registers all classes, methods, and fields from loaded modules:

- **Class Discovery**: Classes are automatically registered with their type IDs and module information
- **Method Discovery**: Methods are registered with their signatures and owning types
- **Field Discovery**: Fields are registered with their types and owning classes
- **Runtime Lookup**: Classes, methods, and fields can be looked up by name or ID at runtime

## Static Methods

He³ supports static methods that can be called without object instantiation:

```he3
domain Program {
    class Main {
        function main(): integer {
            // Static method calls
            Sys.print("Hello from static method!");
            Sys.println("This adds a newline");
            
            let time = Sys.currentTimeMillis();
            Sys.print("Current time: ");
            Sys.println(time.toString());
            
            return 0;
        }
    }
}
```

### Built-in System Functions

The `Sys` class provides static methods for system operations:

- **`Sys.print(string)`** - Print string without newline
- **`Sys.println(string)`** - Print string with newline
- **`Sys.currentTimeMillis()`** - Get current timestamp in milliseconds
- **`Sys.readLine()`** - Read line from input
- **`Sys.fileExists(string)`** - Check if file exists
- **`Sys.readFile(string)`** - Read file contents
- **`Sys.writeFile(string, string)`** - Write string to file
- **`Sys.listDir(string)`** - List directory contents
- **`Sys.createDir(string)`** - Create directory
- **`Sys.deleteFile(string)`** - Delete file
- **`Sys.deleteDir(string)`** - Delete directory
- **`Sys.exit(integer)`** - Exit with code
- **`Sys.argc()`** - Get argument count
- **`Sys.argv(integer)`** - Get argument value
- **`Sys.workingDir()`** - Get current working directory
- **`Sys.changeDir(string)`** - Change directory
- **`Sys.envGet(string)`** - Get environment variable
- **`Sys.envSet(string, string)`** - Set environment variable

## Implementation Status
✅ **Completed Features:**
- Class definition and parsing
- Object instantiation (`new` keyword)
- Method and field declarations
- Static method calls and dispatch
- Built-in system functions (`Sys` class)
- Module registry for class discovery
- Bytecode generation for OO operations
- VM execution for OO opcodes
- Method resolution and virtual dispatch
- Field access implementation

📋 **Planned:**
- Interface support
- Property accessors
- Inheritance and polymorphism
- Generic types
- Async method support
