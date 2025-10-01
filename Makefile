# He³ Compiler Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Isrc/include
SRCDIR = src
BUILDDIR = build
TESTDIR = $(SRCDIR)/compiler/tests

# Compiler source files
LEXER_SOURCES = $(SRCDIR)/compiler/lexer/lexer.c
PARSER_SOURCES = $(SRCDIR)/compiler/parser/parser.c
AST_SOURCES = $(SRCDIR)/compiler/ast/ast.c
RESOLVER_SOURCES = $(SRCDIR)/compiler/resolver/resolver.c
EMITTER_SOURCES = $(SRCDIR)/compiler/emitter/emitter.c
COMMON_SOURCES = $(SRCDIR)/compiler/common/common.c

# IR and bytecode source files
IR_SOURCES = $(SRCDIR)/compiler/ir/ir.c
BYTECODE_SOURCES = $(SRCDIR)/vm/bytecode/bytecode.c
IR_TO_BYTECODE_SOURCES = $(SRCDIR)/compiler/emitter/ir_to_bytecode.c
AST_TO_IR_SOURCES = $(SRCDIR)/compiler/emitter/ast_to_ir.c

# VM source files
VM_SOURCES = $(SRCDIR)/vm/vm.c
VM_LOADER_SOURCES = $(SRCDIR)/vm/loader/bytecode_loader.c
VM_EXECUTION_SOURCES = $(SRCDIR)/vm/execution/stack.c $(SRCDIR)/vm/execution/interpreter.c
VM_MEMORY_SOURCES = $(SRCDIR)/vm/memory/heap.c
VM_OBJECT_SOURCES = $(SRCDIR)/vm/objects/object.c
VM_MAIN_SOURCES = $(SRCDIR)/vm/main.c

# Test source files
TEST_SOURCES = $(TESTDIR)/lexer_test.c $(TESTDIR)/parser_test.c

# Object files
LEXER_OBJECTS = $(BUILDDIR)/lexer.o
PARSER_OBJECTS = $(BUILDDIR)/parser.o
AST_OBJECTS = $(BUILDDIR)/ast.o
RESOLVER_OBJECTS = $(BUILDDIR)/resolver.o
EMITTER_OBJECTS = $(BUILDDIR)/emitter.o
COMMON_OBJECTS = $(BUILDDIR)/common.o

# IR and bytecode object files
IR_OBJECTS = $(BUILDDIR)/ir.o
BYTECODE_OBJECTS = $(BUILDDIR)/bytecode.o
IR_TO_BYTECODE_OBJECTS = $(BUILDDIR)/ir_to_bytecode.o
AST_TO_IR_OBJECTS = $(BUILDDIR)/ast_to_ir.o

# VM object files
VM_OBJECTS = $(BUILDDIR)/vm.o
VM_LOADER_OBJECTS = $(BUILDDIR)/bytecode_loader.o
VM_EXECUTION_OBJECTS = $(BUILDDIR)/stack.o $(BUILDDIR)/interpreter.o
VM_MEMORY_OBJECTS = $(BUILDDIR)/heap.o
VM_OBJECT_OBJECTS = $(BUILDDIR)/object.o

# Test executables
LEXER_TEST = $(BUILDDIR)/lexer_test
LEXER_REGRESSION_TEST = $(BUILDDIR)/lexer_regression_test
PARSER_TEST = $(BUILDDIR)/parser_test
PARSER_COMPREHENSIVE_TEST = $(BUILDDIR)/parser_comprehensive_test
PARSER_REGRESSION_TEST = $(BUILDDIR)/parser_regression_test
IR_BYTECODE_TEST = $(BUILDDIR)/ir_bytecode_test
AST_TO_IR_TEST = $(BUILDDIR)/ast_to_ir_test
STATEMENT_TRANSLATION_TEST = $(BUILDDIR)/statement_translation_test
METHOD_TRANSLATION_TEST = $(BUILDDIR)/method_translation_test
ERROR_RECOVERY_TEST = $(BUILDDIR)/error_recovery_test
MEMORY_TEST = $(BUILDDIR)/memory_test
OBJECT_TEST = $(BUILDDIR)/object_test

# Main executables
HE3_COMPILER = $(BUILDDIR)/he3
HE3VM = $(BUILDDIR)/he3vm

.PHONY: all clean test lexer-test parser-test ir-bytecode-test ast-to-ir-test statement-translation-test method-translation-test error-recovery-test memory-test object-test setup

# Default target
all: setup $(HE3_COMPILER) $(HE3VM)

# Setup build directory
setup:
	@mkdir -p $(BUILDDIR)

# Build lexer
$(BUILDDIR)/lexer.o: $(LEXER_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build parser
# Build AST
$(BUILDDIR)/ast.o: $(AST_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/parser.o: $(PARSER_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build resolver
$(BUILDDIR)/resolver.o: $(RESOLVER_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build emitter
$(BUILDDIR)/emitter.o: $(EMITTER_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build common utilities
$(BUILDDIR)/common.o: $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build IR
$(BUILDDIR)/ir.o: $(IR_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build bytecode
$(BUILDDIR)/bytecode.o: $(BYTECODE_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build IR to bytecode translator
$(BUILDDIR)/ir_to_bytecode.o: $(IR_TO_BYTECODE_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build AST to IR translator
$(BUILDDIR)/ast_to_ir.o: $(AST_TO_IR_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build lexer test
$(LEXER_TEST): $(TESTDIR)/lexer_test.c $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LEXER_OBJECTS) -o $@

# Build lexer regression test
$(LEXER_REGRESSION_TEST): $(TESTDIR)/lexer_regression_test.c $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LEXER_OBJECTS) -o $@

# Build parser test
# Build AST
$(BUILDDIR)/ast.o: $(AST_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(PARSER_TEST): $(TESTDIR)/parser_test.c $(PARSER_OBJECTS) $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(PARSER_OBJECTS) $(LEXER_OBJECTS) -o $@

# Build parser comprehensive test
# Build AST
$(BUILDDIR)/ast.o: $(AST_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(PARSER_COMPREHENSIVE_TEST): $(TESTDIR)/parser_comprehensive_test.c $(PARSER_OBJECTS) $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(PARSER_OBJECTS) $(LEXER_OBJECTS) -o $@

# Build parser regression test
# Build AST
$(BUILDDIR)/ast.o: $(AST_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(PARSER_REGRESSION_TEST): $(TESTDIR)/parser_regression_test.c $(PARSER_OBJECTS) $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(PARSER_OBJECTS) $(LEXER_OBJECTS) -o $@

# Build IR and bytecode test
$(IR_BYTECODE_TEST): $(TESTDIR)/ir_bytecode_test.c $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) -o $@

# Build AST to IR test
$(AST_TO_IR_TEST): $(TESTDIR)/ast_to_ir_test.c $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) -o $@

# Build statement translation test
$(STATEMENT_TRANSLATION_TEST): $(TESTDIR)/statement_translation_test.c $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) -o $@

# Build method translation test
$(METHOD_TRANSLATION_TEST): $(TESTDIR)/method_translation_test.c $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) -o $@

# Build error recovery test
$(ERROR_RECOVERY_TEST): $(TESTDIR)/error_recovery_test.c $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS) -o $@

# Build main compiler
$(HE3_COMPILER): src/compiler/main.c $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Clean target
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILDDIR)
	@echo "Clean complete"

# Test targets
test: lexer-test lexer-regression-test parser-comprehensive-test ir-bytecode-test ast-to-ir-test statement-translation-test method-translation-test error-recovery-test

lexer-test: $(LEXER_TEST)
	@echo "Running lexer tests..."
	@$(LEXER_TEST)

lexer-regression-test: $(LEXER_REGRESSION_TEST)
	@echo "Running lexer regression tests..."
	@$(LEXER_REGRESSION_TEST)

parser-test: $(PARSER_TEST)
	@echo "Running parser tests..."
	@$(PARSER_TEST)

parser-comprehensive-test: $(PARSER_COMPREHENSIVE_TEST)
	@echo "Running parser comprehensive tests..."
	@$(PARSER_COMPREHENSIVE_TEST)

parser-regression-test: $(PARSER_REGRESSION_TEST)
	@echo "Running parser regression tests..."
	@$(PARSER_REGRESSION_TEST)

ir-bytecode-test: $(IR_BYTECODE_TEST)
	@echo "Running IR and bytecode tests..."
	@$(IR_BYTECODE_TEST)

ast-to-ir-test: $(AST_TO_IR_TEST)
	@echo "Running AST to IR tests..."
	@$(AST_TO_IR_TEST)

statement-translation-test: $(STATEMENT_TRANSLATION_TEST)
	@echo "Running statement translation tests..."
	@$(STATEMENT_TRANSLATION_TEST)

method-translation-test: $(METHOD_TRANSLATION_TEST)
	@echo "Running method translation tests..."
	@$(METHOD_TRANSLATION_TEST)

error-recovery-test: $(ERROR_RECOVERY_TEST)
	@echo "Running error recovery tests..."
	@$(ERROR_RECOVERY_TEST)

memory-test: $(MEMORY_TEST)
	@echo "Running memory management tests..."
	@$(MEMORY_TEST)

object-test: $(OBJECT_TEST)
	@echo "Running object system tests..."
	@$(OBJECT_TEST)

# VM build rules
$(BUILDDIR)/vm.o: $(VM_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/bytecode_loader.o: $(VM_LOADER_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/stack.o: $(SRCDIR)/vm/execution/stack.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/interpreter.o: $(SRCDIR)/vm/execution/interpreter.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/heap.o: $(VM_MEMORY_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/object.o: $(VM_OBJECT_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Memory test executable
$(MEMORY_TEST): $(SRCDIR)/vm/memory/memory_test.c $(VM_MEMORY_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Object test executable
$(OBJECT_TEST): tmp/object_test.c $(VM_OBJECT_OBJECTS) $(VM_MEMORY_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# VM executable
$(HE3VM): $(VM_MAIN_SOURCES) $(VM_OBJECTS) $(VM_LOADER_OBJECTS) $(VM_EXECUTION_OBJECTS) $(VM_MEMORY_OBJECTS) $(VM_OBJECT_OBJECTS) $(BYTECODE_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Test targets
test: test-examples
	@echo "Running all tests..."

test-examples:
	@echo "Testing examples..."
	@for example in examples/standalone/*.he3; do \
		echo "Testing $$example..."; \
		./he3 --ast "$$example" > /dev/null || exit 1; \
	done
	@echo "All examples passed!"


# Development targets
dev: clean all test
	@echo "Development build complete!"

# Install executables (copy to root)
install: $(HE3_COMPILER) $(HE3VM)
	cp $(HE3_COMPILER) ./he3
	cp $(HE3VM) ./he3vm
	@echo "Compiler installed to ./he3"
	@echo "VM installed to ./he3vm"

# Help target
help:
	@echo "He³ Compiler Build System"
	@echo "========================="
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the compiler"
	@echo "  test         - Run all tests"
	@echo "  lexer-test   - Run lexer tests only"
	@echo "  parser-test  - Run parser tests only"
	@echo "  clean        - Remove build artifacts"
	@echo "  dev          - Clean, build, and test"
	@echo "  install      - Install compiler to root"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Build directory: $(BUILDDIR)"
	@echo "Compiler output: $(HE3_COMPILER)"
