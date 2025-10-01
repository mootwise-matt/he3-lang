# Phase 1: Minimal Lexer + Parser - TODO List

## 🎯 Goal
Parse simple He³ programs into AST to enable basic .helium3 module generation

## ✅ Completed Tasks

### Project Structure
- [x] Created `docs/compiler/roadmap.md` - Complete development roadmap
- [x] Created `src/include/helium3.h` - .helium3 file format definitions
- [x] Created `src/include/bytecode.h` - Bytecode opcode definitions
- [x] Enhanced `src/include/tokens.h` - Complete token definitions
- [x] Enhanced `src/include/ast.h` - Complete AST definitions
- [x] Created `src/include/types.h` - Type system definitions
- [x] Created `src/compiler/lexer/lexer.h` - Lexer interface
- [x] Created `src/compiler/lexer/lexer.c` - Lexer implementation
- [x] Created `src/compiler/tests/lexer_test.c` - Lexer test suite
- [x] Created `Makefile` - Build system

### Examples and Testing
- [x] Created `examples/standalone/01_hello.he3` - Basic hello world
- [x] Created `examples/standalone/02_calculator.he3` - Calculator with classes
- [x] Created `examples/standalone/03_security_events.he3` - Security and events demo
- [x] Created `examples/projects/01_hello_project/` - Complete project structure
- [x] Created `examples/test_examples.sh` - Test script for examples
- [x] Created `examples/README.md` - Comprehensive examples documentation

## 🔄 In Progress Tasks

### Lexer Implementation
- [x] Basic tokenization for core He³ syntax
- [x] String literal handling
- [x] Number literal handling (int/float)
- [x] Keyword recognition
- [x] Operator recognition
- [x] Comment handling (line and block)
- [x] Test suite with comprehensive coverage

## 📋 Remaining Tasks

### 1. Parser Implementation (Priority: HIGH)
- [ ] Create `src/compiler/parser/parser.h` - Parser interface
- [ ] Create `src/compiler/parser/parser.c` - Parser implementation
- [ ] Parse class declarations
- [ ] Parse method declarations
- [ ] Parse basic expressions (literals, identifiers, binary ops)
- [ ] Parse simple statements (return, variable declaration)
- [ ] Create `src/compiler/tests/parser_test.c` - Parser test suite

### 2. AST Implementation (Priority: HIGH)
- [ ] Create `src/compiler/common/ast_impl.c` - AST implementation
- [ ] Implement `ast_create()` function
- [ ] Implement `ast_destroy()` function
- [ ] Implement `ast_add_child()` function
- [ ] Implement AST printing for debugging
- [ ] Add memory management for AST nodes

### 3. Basic Type System (Priority: MEDIUM)
- [ ] Create `src/compiler/common/types_impl.c` - Type system implementation
- [ ] Implement built-in types (integer, float, boolean, string, void, object)
- [ ] Implement type table management
- [ ] Add basic type resolution
- [ ] Create type test suite

### 4. Simple .helium3 Emitter (Priority: HIGH)
- [ ] Create `src/compiler/emitter/emitter.h` - Emitter interface
- [ ] Create `src/compiler/emitter/emitter.c` - Emitter implementation
- [ ] Implement .helium3 file format writer
- [ ] Generate basic bytecode for simple programs
- [ ] Create emitter test suite

### 5. Integration & Testing (Priority: MEDIUM)
- [ ] Create `src/compiler/common/common.c` - Common utilities
- [ ] Implement error handling and reporting
- [ ] Create integration test for full pipeline
- [ ] Add memory leak detection
- [ ] Performance testing

## 🧪 Test Cases for Phase 1

### Lexer Tests (✅ Complete)
- [x] Basic tokens (identifiers, keywords, literals)
- [x] String literals with proper escaping
- [x] Number literals (integers and floats)
- [x] All He³ keywords
- [x] All operators and delimiters
- [x] Comment handling (line and block)

### Parser Tests (📋 TODO)
- [ ] Parse simple class: `class Program { }`
- [ ] Parse class with method: `class Program { function main(): integer { return 42; } }`
- [ ] Parse class with fields: `class Program { var x: integer; }`
- [ ] Parse expressions: `1 + 2 * 3`
- [ ] Parse variable declarations: `var x: integer := 42;`
- [ ] Parse return statements: `return 42;`

### Emitter Tests (📋 TODO)
- [ ] Generate .helium3 for simple class
- [ ] Generate .helium3 for class with method
- [ ] Verify .helium3 file format correctness
- [ ] Test bytecode generation for simple expressions

## 🎯 Success Criteria

### Phase 1 Complete When:
- [ ] Lexer tokenizes all He³ syntax correctly
- [ ] Parser builds valid AST for simple programs
- [ ] Emitter generates valid .helium3 modules
- [ ] All tests pass (lexer, parser, emitter)
- [ ] Can compile: `class Program { function main(): integer { return 42; } }`
- [ ] Generated .helium3 module can be loaded by VM (future)

## 🚀 Next Steps After Phase 1

1. **Phase 2**: Type resolution and semantic analysis
2. **Phase 3**: Enhanced .helium3 emitter with full bytecode
3. **Phase 4**: Security and event system integration
4. **Phase 5**: VM development in parallel

## 📝 Development Notes

### Current Focus
- Complete parser implementation for basic He³ syntax
- Ensure AST can represent simple class and method declarations
- Get basic .helium3 emitter working

### Key Files to Implement Next
1. `src/compiler/parser/parser.c` - Core parser logic
2. `src/compiler/common/ast_impl.c` - AST memory management
3. `src/compiler/emitter/emitter.c` - .helium3 file generation

### Testing Strategy
- Unit tests for each component
- Integration tests for full pipeline
- Golden tests for AST structure validation
- File format validation for .helium3 output
