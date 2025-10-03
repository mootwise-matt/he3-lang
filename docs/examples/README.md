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
- String literals with proper memory management
- Static method calls (Sys.print)
- String constant handling

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

### Compile and Run
```bash
# Compile a standalone example
./he3 examples/standalone/01_hello.he3

# Show tokens
./he3 --tokens examples/standalone/01_hello.he3

# Show AST
./he3 --ast examples/standalone/01_hello.he3

# Compile a project
./he3 examples/projects/01_hello_project/
```

### Expected Output

**01_hello.he3 AST**:
```
AST[COMPUNIT]: 
  AST[DOMAIN]: ('app.hello')
  AST[CLASS]: ('Program')
    AST[METHOD]: ('main')
      AST[ARGUMENTS]: 
      AST[LITERAL]: 
      AST[BLOCK]: 
        AST[FUNCTION_CALL]: ('print')
          AST[ARGUMENTS]: 
            AST[LITERAL]: ('"Hello, He³ World!"')
        AST[RETURN]: 
          AST[LITERAL]: (0)
```

## Testing Examples

All examples are tested as part of the compiler's regression test suite:

```bash
# Run all example tests
make test-examples

# Test specific example
make test-example EXAMPLE=01_hello.he3
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
