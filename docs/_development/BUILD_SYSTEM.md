# He³ Build System Guide

## Overview

The He³ programming language uses a **three-stage build process**:

1. **Compiler**: `.he3` → `.bx` (raw bytecode)
2. **Packager**: `.bx` + manifest → `.helium3` (packaged module with headers, Sys proxy)
3. **VM**: `.helium3` → execution

## Build Tools

### 1. `he3` - Single-File Compiler
- **Purpose**: Compile individual He³ source files to raw bytecode
- **Input**: `.he3` source files
- **Output**: `.bx` bytecode files (intermediate format)
- **Usage**: `./he3 [options] <source_file>`

### 2. `he3build` - Multi-File Packager
- **Purpose**: Package multiple `.bx` files into `.helium3` modules
- **Input**: `he3project.json` configuration + `.bx` files
- **Output**: `.helium3` module files (executable format)
- **Usage**: `./he3build [options] <project_file>`

### 3. `he3vm` - Virtual Machine
- **Purpose**: Execute packaged `.helium3` modules
- **Input**: `.helium3` module files
- **Output**: Program execution results
- **Usage**: `./he3vm [options] <module_file>`

## File Formats

### `.he3` - Source Files
- **Format**: He³ source code
- **Purpose**: Human-readable source code
- **Features**: Classes, methods, domains, imports

### `.bx` - Raw Bytecode
- **Format**: Raw bytecode with basic headers
- **Purpose**: Intermediate compilation output
- **Features**: 
  - Basic bytecode instructions
  - String and constant tables
  - Type and method definitions
  - **Note**: Not directly executable by VM

### `.helium3` - Packaged Modules
- **Format**: Complete module with manifest and headers
- **Purpose**: Executable modules for VM
- **Features**:
  - Complete module metadata
  - Sys proxy class integration
  - Security boundaries
  - Cross-module references
  - **Note**: This is the correct format for VM execution

## Current Status

### ✅ **Working Components**
- **Compiler**: `.he3` → `.bx` conversion works correctly
- **VM .bx loader**: Can load `.bx` files directly (for testing)
- **Basic language features**: Classes, methods, variables, arithmetic

### ❌ **Broken Components**
- **Packager**: Parser issues prevent `.bx` → `.helium3` conversion
- **VM .helium3 loader**: Segfaults when loading `.helium3` files
- **Sys proxy integration**: Not implemented in packaged modules

## Working Examples

### Current Working Examples (Using .bx directly)
```bash
# These work because they bypass the broken packager
./he3 examples/standalone/01_simple_return.he3
./he3vm examples/standalone/01_simple_return.bx

./he3 examples/standalone/02_basic_variables.he3
./he3vm examples/standalone/02_basic_variables.bx
```

### Intended Workflow (Currently Broken)
```bash
# This is the correct workflow, but currently broken
./he3 examples/standalone/01_simple_return.he3
./he3build examples/projects/01_simple_project/he3project.json
./he3vm examples/projects/01_simple_project/build/simple-project.helium3
```

## Build Process Details

### Stage 1: Compilation (.he3 → .bx)
1. **Lexical Analysis**: Tokenize source code
2. **Parsing**: Build Abstract Syntax Tree (AST)
3. **IR Generation**: Convert AST to Intermediate Representation
4. **Bytecode Generation**: Convert IR to raw bytecode
5. **File Output**: Save as `.bx` file with basic headers

### Stage 2: Packaging (.bx → .helium3)
1. **Project Loading**: Parse `he3project.json` configuration
2. **Module Discovery**: Find all `.bx` files in project
3. **Dependency Resolution**: Resolve cross-module references
4. **Sys Integration**: Add Sys proxy class and built-in functions
5. **Module Creation**: Package into `.helium3` format with complete headers

### Stage 3: Execution (.helium3 → VM)
1. **Module Loading**: Load `.helium3` file with complete metadata
2. **Sys Initialization**: Initialize built-in functions and classes
3. **Method Resolution**: Resolve entry point and dependencies
4. **Execution**: Run the packaged module

## Project Structure

### Single-File Projects
```
project.he3          # Source file
    ↓ (he3)
project.bx           # Raw bytecode (intermediate)
    ↓ (he3build)
project.helium3      # Packaged module (executable)
    ↓ (he3vm)
Execution
```

### Multi-File Projects
```
project/
├── he3project.json          # Project configuration
├── src/                     # Source files
│   ├── main.he3            # Entry point
│   ├── utils.he3           # Utility classes
│   └── models.he3          # Data models
└── build/                   # Build output
    ├── main.bx             # Compiled bytecode
    ├── utils.bx            # Compiled bytecode
    ├── models.bx           # Compiled bytecode
    └── project.helium3     # Packaged module (executable)
```

## Configuration Files

### `he3project.json`
```json
{
  "name": "my-project",
  "version": "1.0.0",
  "type": "executable",
  "entryPoint": "src/main.he3",
  "domains": ["app.main", "app.utils"],
  "dependencies": [],
  "security": {
    "keys": {}
  },
  "build": {
    "output": "build/my-project.helium3",
    "optimize": false,
    "debug": true
  }
}
```

## Troubleshooting

### Common Issues

1. **Packager Parser Errors**: Complex OO syntax causes parser issues
2. **VM .helium3 Segfaults**: Module loader has memory management issues
3. **Missing Sys Proxy**: Built-in functions not available in modules
4. **Import Resolution**: Cross-module imports don't work

### Debug Mode

```bash
# Compiler debug
./he3 -debug source.he3

# Packager debug
./he3build -d project/he3project.json

# VM debug
./he3vm -d module.helium3
```

## Next Steps

### Priority 1: Fix Packager
- Resolve parser issues with complex OO syntax
- Implement proper `.bx` to `.helium3` conversion
- Add Sys proxy class integration

### Priority 2: Fix VM .helium3 Loader
- Debug segfaults in module loading
- Implement proper memory management
- Test complete module execution

### Priority 3: Test Complete Workflow
- Verify `.he3` → `.bx` → `.helium3` → VM execution
- Create working multi-file project examples
- Document proper usage patterns

## Architecture Benefits

The three-stage build process provides:

1. **Separation of Concerns**: Compilation, packaging, and execution are separate
2. **Modularity**: Multiple source files can be combined into single modules
3. **Optimization**: Each stage can be optimized independently
4. **Debugging**: Issues can be isolated to specific stages
5. **Flexibility**: Different packaging strategies for different use cases

## Conclusion

The He³ build system has a **solid foundation** with working compilation and basic VM execution. The main issues are in the **packaging stage** (`.bx` → `.helium3`) and **module loading** (`.helium3` → VM execution). Once these are fixed, the complete build workflow will function as designed.

The current working examples demonstrate that the core language features and compilation pipeline are functional, providing a strong foundation for completing the build system.