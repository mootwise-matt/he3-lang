# He³ Examples

This directory contains example programs demonstrating various He³ language features and compiler capabilities.

## Directory Structure

```
examples/
├── standalone/          # Single-file examples
│   ├── 01_hello.he3    # Basic hello world
│   └── 02_types.he3    # Type system examples
└── projects/            # Multi-file project examples
    └── 01_hello_project/
        ├── he3project.json
        ├── main.he3
        └── utils.he3
```

## Standalone Examples

### 01_minimal - Basic Hello World
```he3
domain Program {
    class Main {
        function main(): integer {
            return 0;
        }
    }
}
```

**Features Demonstrated**:
- Domain declarations
- Class definitions
- Method declarations
- Return statements

### 03_print_demo - String Literals and Static Methods
```he3
domain Program {
    class Main {
        function main(): integer {
            Sys.print("Hello from He³!");
            return 0;
        }
    }
}
```

**Features Demonstrated**:
- String literals
- Static method calls (`Sys.print()`)
- Basic output

### 04_operations_demo - Arithmetic, Comparison, and Logical Operations
```he3
domain Program {
    class Main {
        function main(): System.Int64 {
            let a: System.Int64 = 15;
            let b: System.Int64 = 4;
            
            // Arithmetic operations
            let sum: System.Int64 = a + b;        // 19
            let diff: System.Int64 = a - b;       // 11
            let prod: System.Int64 = a * b;       // 60
            let quot: System.Int64 = a / b;       // 3
            let mod: System.Int64 = a % b;        // 3
            let neg: System.Int64 = -a;           // -15
            
            // Comparison operations
            let eq: System.Boolean = (a == b);    // false
            let ne: System.Boolean = (a != b);    // true
            let lt: System.Boolean = (a < b);     // false
            let gt: System.Boolean = (a > b);     // true
            
            // Logical operations
            let true_val: System.Boolean = true;
            let false_val: System.Boolean = false;
            let and_result: System.Boolean = (true_val && false_val);  // false
            let or_result: System.Boolean = (true_val || false_val);   // true
            let not_result: System.Boolean = !true_val;                // false
            
            return 0;
        }
    }
}
```

**Features Demonstrated**:
- Complete arithmetic operations (+, -, *, /, %, -)
- Complete comparison operations (==, !=, <, >, <=, >=)
- Complete logical operations (&&, ||, !)
- Mixed-type operations (integer + float)
- Boolean literals (true, false)
- Type checking and validation

### 05_control_flow_demo - Control Flow Constructs
```he3
domain Program {
    class Main {
        function main(): System.Int64 {
            let age: System.Int64 = 18;
            
            // If-else statements
            if (age >= 18) {
                Sys.println("You are an adult");
            } else {
                Sys.println("You are a minor");
            }
            
            // While loops
            let counter: System.Int64 = 0;
            while (counter < 5) {
                Sys.println("Counter: " + counter);
                counter = counter + 1;
            }
            
            // For loops
            let sum: System.Int64 = 0;
            for (let i: System.Int64 = 1; i <= 10; i = i + 1) {
                sum = sum + i;
            }
            Sys.println("Sum of 1 to 10: " + sum);
            
            return 0;
        }
    }
}
```

**Features Demonstrated**:
- Complete if/else conditional statements
- Complete while loop parsing and execution
- Complete for loop parsing and execution with init/condition/update/body
- Complex logical expressions in conditions
- Nested control flow structures
- Jump instruction generation and execution

### 02_types.he3 - Type System Examples
```he3
domain app.types;

class TypeExamples {
    var count: integer = 42;
    var pi: float = 3.14159;
    var name: string = "He³";
    var isActive: boolean = true;
    
    function processData(data: string): integer {
        return data.length;
    }
}
```

**Features Demonstrated**:
- Built-in types (integer, float, string, boolean)
- Variable declarations with types
- Variable initializers
- Method parameters and return types

## Project Examples

### 01_hello_project - Multi-file Project
A simple project demonstrating modular organization:

**he3project.json**:
```json
{
    "name": "hello_project",
    "version": "1.0.0",
    "main": "main.he3",
    "dependencies": []
}
```

**main.he3**:
```he3
domain app.hello_project;

import utils;

class Program {
    function main(): integer {
        var greeter = new Greeter();
        greeter.sayHello("He³ World");
        return 0;
    }
}
```

**utils.he3**:
```he3
domain app.hello_project.utils;

class Greeter {
    function sayHello(name: string): void {
        print("Hello, " + name + "!");
    }
}
```

**Features Demonstrated**:
- Project configuration
- Module imports
- Object instantiation
- Method calls
- String concatenation

## Advanced Examples

### Class Inheritance
```he3
domain app.inheritance;

interface Drawable {
    function draw(): void;
}

class Shape implements Drawable {
    var x: float;
    var y: float;
    
    constructor(x: float, y: float) {
        this.x = x;
        this.y = y;
    }
    
    function draw(): void {
        print("Drawing shape at (" + x + ", " + y + ")");
    }
}

class Circle extends Shape {
    var radius: float;
    
    constructor(x: float, y: float, radius: float) {
        super(x, y);
        this.radius = radius;
    }
    
    function draw(): void {
        print("Drawing circle at (" + x + ", " + y + ") with radius " + radius);
    }
}
```

### Enums with Associated Data
```he3
domain app.enums;

enum Result<T, E> {
    Ok(T),
    Err(E)
}

enum Color {
    Red,
    Green,
    Blue,
    RGB(integer, integer, integer)
}

class ColorProcessor {
    function processColor(color: Color): Result<string, string> {
        match color {
            Color.Red => Result.Ok("Red color processed"),
            Color.Green => Result.Ok("Green color processed"),
            Color.Blue => Result.Ok("Blue color processed"),
            Color.RGB(r, g, b) => Result.Ok("RGB(" + r + ", " + g + ", " + b + ") processed"),
            _ => Result.Err("Unknown color")
        }
    }
}
```

### Records and Properties
```he3
domain app.records;

record Point {
    var x: float;
    var y: float;
    
    constructor(x: float, y: float) {
        this.x = x;
        this.y = y;
    }
    
    property magnitude: float {
        get {
            return sqrt(x * x + y * y);
        }
    }
    
    function distance(other: Point): float {
        var dx = x - other.x;
        var dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
}
```

## Running Examples

### Quick Testing
```bash
# Test all working examples quickly
./working_tests.sh

# Test core functionality during development
./quick_test.sh
```

### Individual Example Testing
```bash
# Compile and run a standalone example
./he3build examples/standalone/01_minimal/he3project.json
./he3vm build/01_minimal.helium3

# Compile and run a project
./he3build examples/projects/01_minimal_project/he3project.json
./he3vm build/01_minimal_project.helium3

# Show compilation details
./he3 --ast examples/standalone/01_minimal/src/main.he3
./he3 --tokens examples/standalone/01_minimal/src/main.he3
```

### Comprehensive Testing
```bash
# Run full regression test suite
./regression_test.sh

# Run with verbose output for debugging
./regression_test.sh --verbose

# Keep test files for inspection
./regression_test.sh --no-cleanup
```

### Expected Output

**01_minimal.he3**:
```
Method execution completed with result: 0
Execution completed successfully with result: 0
```

**02_arithmetic.he3**:
```
Method execution completed with result: 50
Execution completed successfully with result: 50
```

**03_print_demo.he3**:
```
Hello from He³!
Method execution completed with result: 0
Execution completed successfully with result: 0
```

## Testing Examples

All examples are tested as part of the comprehensive regression test suite:

### **Test Scripts**
- **`working_tests.sh`** - Tests only confirmed working features
- **`quick_test.sh`** - Fast development testing
- **`regression_test.sh`** - Full comprehensive testing

### **Test Coverage**
- ✅ **Basic compilation and execution**
- ✅ **Arithmetic operations** (42 + 8 = 50)
- ✅ **Sys.print() functionality** (Hello from He³!)
- ✅ **Control flow** (if/else, while, for loops)
- ✅ **Project build system**
- ✅ **Object-oriented programming**

### **Running Tests**
```bash
# Quick verification
./working_tests.sh

# Development testing
./quick_test.sh

# Full regression testing
./regression_test.sh

# View detailed test report
cat TEST_REPORT.md
```

## Contributing Examples

When adding new examples:

1. **Follow naming convention**: `NN_description.he3`
2. **Include comprehensive comments** explaining features
3. **Add to test suite** in Makefile
4. **Update this documentation** with new examples
5. **Ensure examples compile** without errors

## Example Categories

- **Basic**: Hello world, simple programs
- **Types**: Type system demonstrations
- **OOP**: Classes, inheritance, interfaces
- **Advanced**: Enums, records, generics
- **Projects**: Multi-file examples
- **Performance**: Optimization examples
- **Error Handling**: Error cases and recovery
