# He³ Examples

This directory contains example He³ programs and projects to demonstrate the language features and compiler capabilities.

## 📁 Directory Structure

```
examples/
├── standalone/              # Individual He³ source files
│   ├── 01_hello.he3        # Basic hello world
│   ├── 02_calculator.he3   # Calculator with classes
│   └── 03_security_events.he3 # Security and events demo
├── projects/               # Complete He³ projects
│   └── 01_hello_project/   # Hello project with structure
│       ├── he3project.json # Project configuration
│       ├── src/            # Source files
│       │   ├── main.he3    # Entry point
│       │   └── utils.he3   # Utility classes
│       ├── lib/            # External dependencies
│       ├── keys/           # Security keys
│       ├── build/          # Generated files
│       └── README.md       # Project documentation
├── test_examples.sh        # Test script
└── README.md              # This file
```

## 🚀 Standalone Examples

### 01_hello.he3
**Basic Hello World Program**
- Domain declaration
- Class definition
- Method with return type
- Print statement

```he3
domain app.hello;

class Program {
  function main(): integer {
    print("Hello, He³ World!");
    return 0;
  }
}
```

### 02_calculator.he3
**Calculator with Classes**
- Multiple classes
- Method parameters and return types
- Arithmetic operations
- Conditional statements
- Object instantiation

```he3
domain app.calculator;

class Calculator {
  function add(a: integer, b: integer): integer {
    return a + b;
  }
  // ... more methods
}
```

### 03_security_events.he3
**Security and Events Demo**
- Security key declarations
- Event system usage
- Record definitions
- Pattern matching with Result types
- Cross-thread communication

```he3
domain app.security;

security key UserModuleKey = "user_security_key_12345";

let userLoginEvent = EventManager.createEvent<UserLoginEvent>("user.login");
```

## 📦 Project Examples

### 01_hello_project
**Complete He³ Project Structure**
- Project configuration (`he3project.json`)
- Multiple source files
- Cross-domain imports
- Build system integration

**Features:**
- Domain-based organization
- Import system
- String utilities
- Project configuration
- Build output

## 🧪 Testing Examples

Run the test script to verify all examples:

```bash
cd examples
./test_examples.sh
```

## 🔧 Compilation (When Ready)

### Standalone Files
```bash
# Compile individual files
he3 compile examples/standalone/01_hello.he3
he3 compile examples/standalone/02_calculator.he3
he3 compile examples/standalone/03_security_events.he3

# Run compiled bytecode
he3vm examples/standalone/01_hello.bx
he3vm examples/standalone/02_calculator.bx
he3vm examples/standalone/03_security_events.bx
```

### Project Compilation
```bash
# Build complete project
cd examples/projects/01_hello_project
he3 build

# Run project executable
he3vm build/hello-project.bx
```

## 📋 Language Features Demonstrated

### Core Features
- ✅ Domain declarations
- ✅ Class definitions
- ✅ Method declarations
- ✅ Variable declarations
- ✅ Type annotations
- ✅ Return statements
- ✅ Arithmetic operations
- ✅ String operations
- ✅ Conditional statements

### Advanced Features
- ✅ Import statements
- ✅ Object instantiation
- ✅ Method calls
- ✅ Security keys
- ✅ Event system
- ✅ Record definitions
- ✅ Pattern matching
- ✅ Result types
- ✅ Array operations
- ✅ Static methods

### Project Features
- ✅ Project configuration
- ✅ Build system
- ✅ Multi-file compilation
- ✅ Cross-domain references
- ✅ Module organization

## 🎯 Development Goals

These examples serve as:

1. **Test Cases**: Verify compiler functionality
2. **Documentation**: Show language capabilities
3. **Tutorials**: Learn He³ programming
4. **Benchmarks**: Performance testing
5. **Regression Tests**: Prevent breaking changes

## 📝 Adding New Examples

When adding new examples:

1. **Standalone**: Add `.he3` files to `standalone/`
2. **Projects**: Create new directories in `projects/`
3. **Update**: Add tests to `test_examples.sh`
4. **Document**: Update this README
5. **Test**: Verify compilation and execution

## 🔍 Example Categories

- **Basic**: Simple language features
- **Intermediate**: Classes, methods, types
- **Advanced**: Security, events, async
- **Projects**: Complete applications
- **Libraries**: Reusable modules
- **Tests**: Compiler validation

These examples provide a comprehensive test suite for the He³ compiler and demonstrate the full range of language capabilities.
