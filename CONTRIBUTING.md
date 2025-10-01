# Contributing to He³

Thank you for your interest in contributing to the He³ programming language! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Process](#development-process)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)
- [Issue Reporting](#issue-reporting)

## Code of Conduct

This project follows the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you agree to uphold this code.

## Getting Started

### Prerequisites
- C compiler (GCC 4.9+, Clang 3.5+, or MSVC 2015+)
- Make (GNU Make 3.81+ or compatible)
- Git
- Basic understanding of compiler design

### Setting Up Development Environment

1. **Fork the repository**
   ```bash
   # Go to https://github.com/your-username/he3-lang
   # Click "Fork" button
   ```

2. **Clone your fork**
   ```bash
   git clone https://github.com/your-username/he3-lang.git
   cd he3-lang
   ```

3. **Add upstream remote**
   ```bash
   git remote add upstream https://github.com/original-owner/he3-lang.git
   ```

4. **Build the project**
   ```bash
   make
   ```

5. **Run tests**
   ```bash
   make test
   ```

## Development Process

### Branch Naming
- `feature/description` - New features
- `fix/description` - Bug fixes
- `docs/description` - Documentation updates
- `refactor/description` - Code refactoring
- `test/description` - Test improvements

### Workflow
1. **Create a branch** from `main`
2. **Make your changes**
3. **Add tests** for new functionality
4. **Update documentation** as needed
5. **Run tests** to ensure everything works
6. **Submit a pull request**

## Coding Standards

### C Code Style
- **Standard**: C99 with modern features
- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 100 characters maximum
- **Naming**: snake_case for functions and variables, UPPER_CASE for constants

### Example
```c
// Good
static TokenKind parse_keyword(const char* start, size_t length) {
    if (length == 3 && strncmp(start, "var", 3) == 0) {
        return TK_VAR;
    }
    return TK_IDENTIFIER;
}

// Bad
static TokenKind parseKeyword(const char* start, size_t length) {
    if (length == 3 && strncmp(start, "var", 3) == 0) {
        return TK_VAR;
    }
    return TK_IDENTIFIER;
}
```

### Documentation Style
- Use **Markdown** for documentation
- Follow the existing documentation structure
- Include code examples where appropriate
- Keep documentation up-to-date with code changes

### Commit Messages
Follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
type(scope): brief description

Detailed description of changes.

Fixes #issue-number
```

**Types**:
- `feat`: New features
- `fix`: Bug fixes
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test additions/changes
- `chore`: Maintenance tasks

## Testing

### Test Categories
- **Unit Tests**: Individual component testing
- **Integration Tests**: End-to-end testing
- **Regression Tests**: Prevent breaking changes
- **Performance Tests**: Benchmarking

### Running Tests
```bash
# Run all tests
make test

# Run specific test category
make test-lexer
make test-parser
make test-ast

# Run with verbose output
make test VERBOSE=1
```

### Adding Tests
1. **Create test file** in appropriate directory
2. **Follow naming convention**: `test_*.c`
3. **Include test cases** for all scenarios
4. **Add to Makefile** test targets
5. **Document test purpose** in comments

### Example Test
```c
#include <assert.h>
#include "lexer.h"

void test_lexer_keywords() {
    Lexer lexer;
    lexer_create(&lexer, "class function");
    
    Token token1 = lexer_next_token(&lexer);
    assert(token1.kind == TK_CLASS);
    
    Token token2 = lexer_next_token(&lexer);
    assert(token2.kind == TK_FUNCTION);
    
    lexer_destroy(&lexer);
}
```

## Documentation

### Documentation Structure
```
docs/
├── language/          # Language specification
├── compiler/          # Compiler documentation
├── bytecode/          # VM and bytecode docs
├── examples/          # Example documentation
└── development/       # Development guides
```

### Writing Documentation
- **Be clear and concise**
- **Use examples** to illustrate concepts
- **Keep it up-to-date** with code changes
- **Follow existing style** and structure
- **Include diagrams** for complex concepts

### Documentation Types
- **API Documentation**: Function and type documentation
- **User Guides**: How to use the language
- **Developer Guides**: How to contribute
- **Architecture Docs**: System design and structure

## Pull Request Process

### Before Submitting
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] New features have tests
- [ ] Documentation is updated
- [ ] Code follows style guidelines
- [ ] No memory leaks
- [ ] Performance impact considered

### Pull Request Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Manual testing completed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No breaking changes (or documented)

## Related Issues
Fixes #issue-number
```

### Review Process
1. **Automated checks** must pass
2. **Code review** by maintainers
3. **Testing** on multiple platforms
4. **Documentation review**
5. **Approval** and merge

## Issue Reporting

### Bug Reports
When reporting bugs, include:
- **Description** of the problem
- **Steps to reproduce**
- **Expected behavior**
- **Actual behavior**
- **Environment** (OS, compiler, version)
- **Minimal example** if possible

### Feature Requests
When requesting features, include:
- **Description** of the feature
- **Use case** and motivation
- **Proposed implementation** (if known)
- **Alternatives** considered
- **Additional context**

### Issue Labels
- `bug`: Something isn't working
- `enhancement`: New feature or request
- `documentation`: Documentation improvements
- `good first issue`: Good for newcomers
- `help wanted`: Extra attention needed
- `question`: Further information requested

## Development Areas

### High Priority
- **Virtual Machine**: Bytecode execution engine
- **Type Resolution**: Semantic analysis
- **IR Generation**: Intermediate representation
- **Bytecode Emission**: Object file generation

### Medium Priority
- **Pattern Matching**: `match` statements
- **Async/Await**: Concurrency support
- **Error Handling**: Option/Result types
- **Performance**: Optimization passes

### Low Priority
- **IDE Support**: Language server protocol
- **Debugging**: Debug information
- **Profiling**: Performance analysis tools
- **Packaging**: Distribution packages

## Getting Help

### Resources
- **Documentation**: `docs/` directory
- **Examples**: `examples/` directory
- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions

### Community
- **GitHub Discussions**: General questions
- **Issues**: Bug reports and feature requests
- **Pull Requests**: Code contributions

## Recognition

Contributors will be recognized in:
- **CONTRIBUTORS.md**: List of all contributors
- **Release Notes**: Major contributions
- **Documentation**: Code comments and credits

## License

By contributing to He³, you agree that your contributions will be licensed under the same license as the project (Apache License 2.0).

---

Thank you for contributing to He³! Your contributions help make this project better for everyone.
