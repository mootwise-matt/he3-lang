# He³ Build System Specification

## Overview

The He³ build system provides a complete compilation and execution pipeline from source code to VM execution. It supports both single-file compilation and multi-file project builds.

**Current Build:** Sirius (v0.1.0-Sirius) • **Build Date:** 2025-10-04

## Build Tools

### 1. Compiler (`he3`)
Compiles individual He³ source files to bytecode.

```bash
./he3 source.he3
```

**Output**: `source.bx` (raw bytecode file)

### 2. Packager (`he3build`)
Packages multi-file projects into executable modules.

```bash
./he3build project.json
```

**Output**: `build/project_name.helium3` (executable module)

### 3. Virtual Machine (`he3vm`)
Executes compiled modules with comprehensive command line options.

```bash
./he3vm [options] module.helium3
```

**Command Line Options:**
- `-h, --help` - Show help message and exit
- `-v, --version` - Show version information and exit
- `-d, --debug` - Enable debug output during execution
- `-s, --stack` - Show stack state before/after execution
- `-m, --memory` - Show memory statistics
- `-r, --regions` - Show memory regions
- `-o, --objects` - Show object system information
- `-c, --classes` - Show loaded classes

**Examples:**
```bash
./he3vm module.helium3                    # Basic execution
./he3vm -d module.helium3                 # Debug mode
./he3vm -s -m module.helium3              # Show stack and memory
./he3vm -h                                # Show help (no file needed)
./he3vm -v                                # Show version (no file needed)
```

## Project Structure

### Single-File Projects
```
project/
├── main.he3          # Source file
└── main.bx           # Compiled bytecode (generated)
```

### Multi-File Projects
```
project/
├── he3project.json   # Project configuration
├── src/              # Source files directory
│   ├── main.he3      # Main source file
│   ├── utils.he3     # Utility classes
│   └── models.he3    # Data models
└── build/            # Build output directory
    └── project.helium3  # Executable module (generated)
```

## Project Configuration

### `he3project.json` Format

```json
{
    "name": "project_name",
    "version": "1.0.0",
    "type": "executable",
    "source_dir": "src",
    "output": "build/project_name.helium3"
}
```

#### Required Fields
- **`name`**: Project identifier (used for output filename)
- **`version`**: Project version string
- **`type`**: Project type (`"executable"` or `"library"`)
- **`source_dir`**: Directory containing source files (default: `"src"`)
- **`output`**: Output file path (default: `"build/{name}.helium3"`)

#### Optional Fields
- **`description`**: Project description
- **`author`**: Project author
- **`license`**: Project license
- **`dependencies`**: Array of dependency names
- **`optimize`**: Enable optimizations (default: `false`)
- **`debug`**: Enable debug information (default: `true`)

### Legacy Format Support

The build system also supports a legacy format for backward compatibility:

```json
{
    "name": "project_name",
    "version": "1.0.0",
    "description": "Project description",
    "entryPoint": "src/main.he3",
    "sourceFiles": ["src/main.he3", "src/utils.he3"],
    "outputPath": "build/project_name.helium3"
}
```

## Source File Discovery

The packager automatically discovers source files in the `src/` directory by looking for common patterns:

### Supported File Patterns
- `main.he3` - Main entry point
- `utils.he3` - Utility functions
- `lib.he3` - Library code
- `app.he3` - Application code

### File Naming Convention
- Use lowercase with underscores: `user_service.he3`
- Main entry point should be `main.he3`
- Utility files should have descriptive names

## Build Process

### 1. Compilation Phase
```bash
# For each .he3 file in src/
./he3 src/file.he3
# Generates: src/file.bx
```

### 2. Packaging Phase
```bash
# Merge all .bx files into .helium3 module
./he3build project.json
# Generates: build/project_name.helium3
```

### 3. Execution Phase
```bash
# Execute the packaged module
./he3vm build/project_name.helium3
```

## Complete Workflow Examples

### Example 1: Simple Single-File Project
```bash
# Create source file
echo 'domain Test { function main(): System.Int64 { return 42; } }' > test.he3

# Compile
./he3 test.he3

# Execute (using .bx file directly)
./he3vm test.bx
```

### Example 2: Multi-File Project
```bash
# Create project structure
mkdir -p myproject/src
cd myproject

# Create project configuration
cat > he3project.json << 'EOF'
{
    "name": "myproject",
    "version": "1.0.0",
    "type": "executable",
    "source_dir": "src",
    "output": "build/myproject.helium3"
}
EOF

# Create source files
cat > src/main.he3 << 'EOF'
domain MyProject {
    function main(): System.Int64 {
        let result: System.Int64 = Calculator.add(10, 5);
        return result;
    }
}
EOF

cat > src/calculator.he3 << 'EOF'
domain MyProject {
    class Calculator {
        static function add(a: System.Int64, b: System.Int64): System.Int64 {
            return a + b;
        }
    }
}
EOF

# Build project
../he3build he3project.json

# Execute
../he3vm build/myproject.helium3
```

## Supported Language Features

### Arithmetic Operations
- Addition (`+`), Subtraction (`-`), Multiplication (`*`), Division (`/`)
- Modulo (`%`), Negation (`-`)
- Mixed-type operations (integer + float)

### Comparison Operations
- Equality (`==`), Inequality (`!=`)
- Less Than (`<`), Less Equal (`<=`)
- Greater Than (`>`), Greater Equal (`>=`)
- String, boolean, and mixed-type comparisons

### Logical Operations
- Logical AND (`&&`), Logical OR (`||`), Logical NOT (`!`)
- Boolean type checking and validation

### Object-Oriented Programming
- Class definitions and instantiation
- Method calls (static and instance)
- Field access and assignment
- Static methods (e.g., `Sys.print()`)

### Type System
- Built-in types: `System.Int64`, `System.Float64`, `System.Boolean`, `System.String`
- Type checking and validation
- Mixed-type operations with automatic coercion

## Error Handling

### Compilation Errors
- Syntax errors with line/column information
- Type checking errors
- Semantic analysis errors

### Runtime Errors
- Division by zero protection
- Stack overflow/underflow detection
- Type validation errors
- Memory management errors

## Debugging

### Debug Mode
Enable debug information in project configuration:

```json
{
    "debug": true
}
```

### Debug Output
- Bytecode disassembly
- String table contents
- Method table information
- Execution trace

## Performance Considerations

### Optimization
Enable optimizations in project configuration:

```json
{
    "optimize": true
}
```

### Memory Management
- Generational garbage collection
- String deduplication
- Module-level memory management
- Reference counting

## Troubleshooting

### Common Issues

1. **"No source files found"**
   - Ensure source files are in the `src/` directory
   - Check file naming convention
   - Verify project configuration

2. **"Invalid project configuration"**
   - Check JSON syntax
   - Verify required fields are present
   - Ensure file paths are correct

3. **Compilation errors**
   - Check syntax in source files
   - Verify type declarations
   - Ensure proper domain/class structure

4. **Runtime errors**
   - Check for division by zero
   - Verify type compatibility
   - Ensure proper method calls

### Debug Commands
```bash
# Compile with debug output
./he3 -d source.he3

# Build with verbose output
./he3build project.json

# Execute with debug information
./he3vm -d module.helium3

# Show help and version (no file required)
./he3vm -h
./he3vm -v

# Show diagnostic information
./he3vm -s -m -o module.helium3  # Stack, memory, objects
```

## Testing Integration

### Test Scripts
The build system integrates with comprehensive testing:

```bash
# Quick development testing
./quick_test.sh

# Test only working features
./working_tests.sh

# Full regression testing
./regression_test.sh

# Verbose testing with debugging
./regression_test.sh --verbose
```

### Test Coverage
- ✅ **Compilation pipeline** - Source to .helium3
- ✅ **VM execution** - Bytecode interpretation
- ✅ **Language features** - All implemented constructs
- ✅ **Project builds** - Single and multi-file projects
- ✅ **Error handling** - Compilation and runtime errors

### Continuous Integration
The build system supports CI/CD workflows:

```bash
# CI/CD pipeline example
make clean
make
./regression_test.sh --verbose
if [ $? -eq 0 ]; then
    echo "Build and tests successful"
    # Deploy or package
else
    echo "Build or tests failed"
    exit 1
fi
```

## Future Enhancements

### Planned Features
- Cross-module imports
- Dependency resolution
- Advanced optimizations
- IDE integration
- Package management
- Enhanced testing framework

### Build System Improvements
- Incremental compilation
- Parallel compilation
- Dependency tracking
- Build caching
- Configuration validation
- Automated testing integration

## Build Names and Version Management

### Build Name System
He³ uses a build name system to identify different builds and branches:

- **Current Build:** Sirius (v0.1.0-Sirius)
- **Build Names:** Used for branch identification and version tracking
- **Version Format:** `v<major>.<minor>.<patch>-<build_name>`
- **Build Info:** Includes build date and time

### Managing Build Names
Use the provided script to update build names:

```bash
# Change build name
./update_build_name.sh <new_name>

# Rebuild with new name
make clean && make

# Verify new build name
./he3 -v
./he3vm -v
./he3build -v
```

### Build Name Examples
- **Sirius** - Current stable build
- **Orion** - Development branch
- **Vega** - Feature branch
- **Alpha** - Alpha release
- **Beta** - Beta release

### Version Information
All He³ tools display comprehensive version information:

```bash
$ ./he3 -v
He³ Compiler v0.1.0-Sirius
A 64-bit, UTF-8, object-oriented programming language
with Pascal roots and Java/C#-style syntax
Build: Sirius (2025-10-04 11:09:24)
```