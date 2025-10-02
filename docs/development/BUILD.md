# Building and Developing He³

This guide covers building the He³ compiler, running tests, and contributing to the project.

## Prerequisites

### Required Tools
- **C Compiler**: GCC 4.9+, Clang 3.5+, or MSVC 2015+
- **Make**: GNU Make 3.81+ or compatible
- **Git**: For version control

### Platform Support
- **Linux**: Tested on Ubuntu 18.04+, CentOS 7+
- **macOS**: Tested on macOS 10.14+
- **Windows**: Tested with MinGW-w64 and MSVC

## Building the Compiler

### Quick Build
```bash
git clone https://github.com/your-username/he3-lang.git
cd he3-lang
make
```

### Build Options
```bash
# Debug build with symbols
make debug

# Release build with optimizations
make release

# Clean build artifacts
make clean

# Install compiler to system
make install PREFIX=/usr/local
```

### Build Targets
- `he3` - Main compiler executable
- `he3vm` - Virtual machine executable
- `libhe3` - Compiler library (planned)

## Project Structure

```
he3/
├── src/
│   ├── shared/            # Shared components between compiler and VM
│   │   ├── ast/           # Unified AST definitions
│   │   ├── bytecode/      # Shared bytecode format and opcodes
│   │   ├── types/         # Common type definitions
│   │   └── tokens.h       # Common token definitions
│   ├── compiler/          # Compiler implementation
│   │   ├── lexer/         # Lexical analysis
│   │   ├── parser/        # Syntax analysis
│   │   ├── ir/            # Intermediate representation
│   │   ├── emitter/       # Code generation
│   │   └── main.c         # Compiler entry point
│   └── vm/                # Virtual Machine implementation
│       ├── execution/     # VM execution engine
│       ├── memory/        # Memory management
│       ├── objects/       # Object system
│       └── main.c         # VM entry point
├── examples/              # Example programs
├── docs/                  # Documentation
├── build/                 # Build artifacts
├── Makefile              # Build configuration
└── README.md             # Project overview
```

## Development Workflow

### 1. Setting Up Development Environment

```bash
# Clone repository
git clone https://github.com/your-username/he3-lang.git
cd he3-lang

# Create development branch
git checkout -b feature/your-feature

# Build compiler
make
```

### 2. Making Changes

#### Adding New Language Features
1. **Update lexer** (`src/compiler/lexer/`) for new tokens
2. **Update parser** (`src/compiler/parser/`) for new syntax
3. **Update AST** (`src/compiler/ast/`) for new node types
4. **Add tests** in `src/compiler/tests/`
5. **Update documentation** in `docs/`

#### Example: Adding a New Keyword
```c
// 1. Add token to src/include/tokens.h
typedef enum {
    // ... existing tokens
    TK_NEW_KEYWORD,
} TokenKind;

// 2. Add lexer support in src/compiler/lexer/lexer.c
if (strncmp(start, "newkeyword", 10) == 0) kind = TK_NEW_KEYWORD;

// 3. Add parser support in src/compiler/parser/parser.c
if (parser_match(parser, TK_NEW_KEYWORD)) {
    // Parse new keyword construct
}
```

### 3. Testing Changes

```bash
# Run all tests
make test

# Run specific test category
make test-lexer
make test-parser
make test-ast

# Test with examples
make test-examples

# Test specific example
./he3 --ast examples/standalone/01_hello.he3
```

### 4. Code Quality

#### Compiler Warnings
The build system enables strict warnings:
```bash
# Build with all warnings
make CFLAGS="-Wall -Wextra -Werror"
```

#### Code Style
- Use C99 standard
- Follow existing naming conventions
- Add comprehensive comments
- Include error handling

#### Memory Management
- Use `malloc`/`free` consistently
- Check for allocation failures
- Avoid memory leaks
- Use valgrind for debugging

## Testing Strategy

### Unit Tests
```bash
# Run lexer tests
make test-lexer

# Run parser tests  
make test-parser

# Run AST tests
make test-ast
```

### Integration Tests
```bash
# Test complete compilation pipeline
make test-integration

# Test with real examples
make test-examples
```

### Regression Tests
```bash
# Test all examples for regressions
make test-regression

# Update golden files
make update-golden
```

## Debugging

### Compiler Debugging
```bash
# Build with debug symbols
make debug

# Run with debugger
gdb ./build/he3
(gdb) run --ast examples/standalone/01_hello.he3

# Use valgrind for memory issues
valgrind --leak-check=full ./build/he3 examples/standalone/01_hello.he3
```

### Parser Debugging
```bash
# Show tokens
./build/he3 --tokens examples/standalone/01_hello.he3

# Show AST
./build/he3 --ast examples/standalone/01_hello.he3

# Verbose parsing
./build/he3 --verbose examples/standalone/01_hello.he3
```

### Memory Debugging
```bash
# Use AddressSanitizer
make CFLAGS="-fsanitize=address -g" LDFLAGS="-fsanitize=address"

# Use Valgrind
valgrind --tool=memcheck --leak-check=full ./build/he3
```

## Performance Profiling

### Compilation Performance
```bash
# Profile compiler performance
make profile

# Benchmark compilation
make benchmark
```

### Memory Usage
```bash
# Monitor memory usage
make memory-check

# Profile memory allocation
make memory-profile
```

## Contributing

### Pull Request Process
1. **Fork** the repository
2. **Create** a feature branch
3. **Make** your changes
4. **Add** tests for new features
5. **Update** documentation
6. **Submit** a pull request

### Code Review Checklist
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] New features have tests
- [ ] Documentation is updated
- [ ] Code follows style guidelines
- [ ] No memory leaks
- [ ] Performance impact considered

### Commit Message Format
```
type(scope): brief description

Detailed description of changes.

Fixes #issue-number
```

**Types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

## Release Process

### Version Numbering
- **Major**: Breaking changes
- **Minor**: New features, backward compatible
- **Patch**: Bug fixes, backward compatible

### Release Steps
1. **Update** version numbers
2. **Update** CHANGELOG.md
3. **Run** full test suite
4. **Create** release tag
5. **Build** release artifacts
6. **Publish** to GitHub

## Troubleshooting

### Common Build Issues

#### Missing Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"

# macOS
xcode-select --install
```

#### Compiler Errors
```bash
# Check C99 compliance
make CFLAGS="-std=c99 -pedantic"

# Check for undefined behavior
make CFLAGS="-fsanitize=undefined"
```

#### Memory Issues
```bash
# Use AddressSanitizer
make CFLAGS="-fsanitize=address" LDFLAGS="-fsanitize=address"

# Use Valgrind
valgrind --tool=memcheck ./build/he3
```

### Getting Help
- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Documentation**: `docs/` directory
- **Examples**: `examples/` directory

---

This guide should help you get started with He³ development. For more specific questions, please refer to the documentation or open an issue.
