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

# Test source files
TEST_SOURCES = $(TESTDIR)/lexer_test.c $(TESTDIR)/parser_test.c

# Object files
LEXER_OBJECTS = $(BUILDDIR)/lexer.o
PARSER_OBJECTS = $(BUILDDIR)/parser.o
AST_OBJECTS = $(BUILDDIR)/ast.o
RESOLVER_OBJECTS = $(BUILDDIR)/resolver.o
EMITTER_OBJECTS = $(BUILDDIR)/emitter.o
COMMON_OBJECTS = $(BUILDDIR)/common.o

# Test executables
LEXER_TEST = $(BUILDDIR)/lexer_test
LEXER_REGRESSION_TEST = $(BUILDDIR)/lexer_regression_test
PARSER_TEST = $(BUILDDIR)/parser_test
PARSER_COMPREHENSIVE_TEST = $(BUILDDIR)/parser_comprehensive_test
PARSER_REGRESSION_TEST = $(BUILDDIR)/parser_regression_test

# Main compiler executable
HE3_COMPILER = $(BUILDDIR)/he3

.PHONY: all clean test lexer-test parser-test setup

# Default target
all: setup $(HE3_COMPILER)

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

# Build main compiler
$(HE3_COMPILER): src/compiler/main.c $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(AST_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Test targets
test: lexer-test lexer-regression-test parser-comprehensive-test

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

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)

# Development targets
dev: clean all test
	@echo "Development build complete!"

# Install compiler (copy to root)
install: $(HE3_COMPILER)
	cp $(HE3_COMPILER) ./he3
	@echo "Compiler installed to ./he3"

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
