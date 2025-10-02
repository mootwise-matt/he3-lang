# He³ Compiler Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Isrc/shared -Isrc/compiler -Isrc/vm
SRCDIR = src
BUILDDIR = build
TESTDIR = $(SRCDIR)/compiler/tests

# Shared source files
SHARED_SOURCES = $(SRCDIR)/shared/bytecode/constant_table.c $(SRCDIR)/shared/ast/ast.c

# Compiler source files
LEXER_SOURCES = $(SRCDIR)/compiler/lexer/lexer.c
PARSER_SOURCES = $(SRCDIR)/compiler/parser/parser.c
AST_SOURCES = $(SRCDIR)/shared/ast/ast.c
MAIN_SOURCES = $(SRCDIR)/compiler/main.c

# IR and bytecode source files
IR_SOURCES = $(SRCDIR)/compiler/ir/ir.c
BYTECODE_SOURCES = $(SRCDIR)/vm/bytecode/bytecode.c
IR_TO_BYTECODE_SOURCES = $(SRCDIR)/compiler/emitter/ir_to_bytecode.c
AST_TO_IR_SOURCES = $(SRCDIR)/compiler/emitter/ast_to_ir.c
BYTECODE_FILE_SOURCES = $(SRCDIR)/vm/bytecode/bytecode_file.c
HELIUM_MODULE_SOURCES = $(SRCDIR)/vm/bytecode/helium_module.c

# VM source files
VM_SOURCES = $(SRCDIR)/vm/vm.c
VM_LOADER_SOURCES = $(SRCDIR)/vm/loader/bytecode_loader.c
VM_EXECUTION_SOURCES = $(SRCDIR)/vm/execution/stack.c $(SRCDIR)/vm/execution/interpreter.c $(SRCDIR)/vm/execution/context.c
VM_MEMORY_SOURCES = $(SRCDIR)/vm/memory/heap.c
VM_OBJECT_SOURCES = $(SRCDIR)/vm/objects/object.c
VM_BYTECODE_FILE_SOURCES = $(SRCDIR)/vm/bytecode/bytecode_file.c
VM_OPCODE_UTILS_SOURCES = $(SRCDIR)/vm/bytecode/opcode_utils.c
VM_HELIUM_MODULE_SOURCES = $(SRCDIR)/vm/bytecode/helium_module.c
VM_MAIN_SOURCES = $(SRCDIR)/vm/main.c

# Test source files
TEST_SOURCES = $(TESTDIR)/lexer_test.c $(TESTDIR)/parser_test.c

# Object files
SHARED_OBJECTS = $(BUILDDIR)/constant_table.o $(BUILDDIR)/ast.o
LEXER_OBJECTS = $(BUILDDIR)/lexer.o
PARSER_OBJECTS = $(BUILDDIR)/parser.o
# AST_OBJECTS = $(BUILDDIR)/ast.o  # Now using shared AST
MAIN_OBJECTS = $(BUILDDIR)/main.o

# IR and bytecode object files
IR_OBJECTS = $(BUILDDIR)/ir.o
BYTECODE_OBJECTS = $(BUILDDIR)/bytecode.o
IR_TO_BYTECODE_OBJECTS = $(BUILDDIR)/ir_to_bytecode.o
AST_TO_IR_OBJECTS = $(BUILDDIR)/ast_to_ir.o
BYTECODE_FILE_OBJECTS = $(BUILDDIR)/bytecode_file.o
HELIUM_MODULE_OBJECTS = $(BUILDDIR)/helium_module.o

# VM object files
VM_OBJECTS = $(BUILDDIR)/vm.o
VM_LOADER_OBJECTS = $(BUILDDIR)/bytecode_loader.o
VM_EXECUTION_OBJECTS = $(BUILDDIR)/stack.o $(BUILDDIR)/interpreter.o $(BUILDDIR)/context.o
VM_MEMORY_OBJECTS = $(BUILDDIR)/heap.o
VM_OBJECT_OBJECTS = $(BUILDDIR)/object.o
VM_BYTECODE_FILE_OBJECTS = $(BUILDDIR)/bytecode_file.o
VM_OPCODE_UTILS_OBJECTS = $(BUILDDIR)/opcode_utils.o
VM_HELIUM_MODULE_OBJECTS = $(BUILDDIR)/helium_module.o
VM_MAIN_OBJECTS = $(BUILDDIR)/vm_main.o

# Test object files
TEST_OBJECTS = $(BUILDDIR)/lexer_test.o $(BUILDDIR)/parser_test.o

# All source files
ALL_SOURCES = $(SHARED_SOURCES) $(LEXER_SOURCES) $(PARSER_SOURCES) $(AST_SOURCES) $(MAIN_SOURCES) $(IR_SOURCES) $(BYTECODE_SOURCES) $(IR_TO_BYTECODE_SOURCES) $(AST_TO_IR_SOURCES) $(BYTECODE_FILE_SOURCES) $(HELIUM_MODULE_SOURCES) $(VM_SOURCES) $(VM_LOADER_SOURCES) $(VM_EXECUTION_SOURCES) $(VM_MEMORY_SOURCES) $(VM_OBJECT_SOURCES) $(VM_BYTECODE_FILE_SOURCES) $(VM_OPCODE_UTILS_SOURCES) $(VM_HELIUM_MODULE_SOURCES) $(VM_MAIN_SOURCES)

# All object files
ALL_OBJECTS = $(SHARED_OBJECTS) $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(MAIN_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) $(BYTECODE_FILE_OBJECTS) $(HELIUM_MODULE_OBJECTS) $(VM_OBJECTS) $(VM_LOADER_OBJECTS) $(VM_EXECUTION_OBJECTS) $(VM_MEMORY_OBJECTS) $(VM_OBJECT_OBJECTS) $(VM_BYTECODE_FILE_OBJECTS) $(VM_OPCODE_UTILS_OBJECTS) $(VM_HELIUM_MODULE_OBJECTS) $(VM_MAIN_OBJECTS)

# Main targets
all: he3 he3vm

# Compiler executable
he3: $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(MAIN_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) $(BYTECODE_FILE_OBJECTS) $(HELIUM_MODULE_OBJECTS) $(SHARED_OBJECTS)
	@echo "Building He³ compiler..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "Compiler built successfully!"

# VM executable
he3vm: $(VM_OBJECTS) $(VM_LOADER_OBJECTS) $(VM_EXECUTION_OBJECTS) $(VM_MEMORY_OBJECTS) $(VM_OBJECT_OBJECTS) $(VM_BYTECODE_FILE_OBJECTS) $(VM_OPCODE_UTILS_OBJECTS) $(VM_HELIUM_MODULE_OBJECTS) $(VM_MAIN_OBJECTS) $(SHARED_OBJECTS)
	@echo "Building He³ VM..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "VM built successfully!"

# Test executables
test_lexer: $(LEXER_OBJECTS) $(BUILDDIR)/lexer_test.o
	@echo "Building lexer test..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "Lexer test built successfully!"

test_parser: $(PARSER_OBJECTS) $(AST_OBJECTS) $(BUILDDIR)/parser_test.o
	@echo "Building parser test..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "Parser test built successfully!"

# Object file rules
$(BUILDDIR)/%.o: $(SRCDIR)/shared/bytecode/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/shared/ast/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/lexer/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/parser/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/ast/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/emitter/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/ir/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/bytecode/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/loader/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/execution/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/memory/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/objects/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean targets
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILDDIR)
	rm -f he3 he3vm test_lexer test_parser
	@echo "Clean complete!"

clean_all: clean
	@echo "Cleaning all generated files..."
	rm -f *.ohe3 *.bx *.helium3
	@echo "Clean all complete!"

# Test targets
test: test_lexer test_parser
	@echo "Running tests..."
	@echo "Running lexer tests..."
	./test_lexer
	@echo "Running parser tests..."
	./test_parser
	@echo "All tests completed!"

# Example compilation targets
examples: he3 he3vm
	@echo "Compiling examples..."
	@echo "Compiling 01_hello.he3..."
	./he3 examples/standalone/01_hello.he3
	@echo "Compiling 02_arithmetic.he3..."
	./he3 examples/standalone/02_arithmetic.he3
	@echo "Compiling 03_simple.he3..."
	./he3 examples/standalone/03_simple.he3
	@echo "Compiling 04_locals.he3..."
	./he3 examples/standalone/04_locals.he3
	@echo "Compiling 05_binary.he3..."
	./he3 examples/standalone/05_binary.he3
	@echo "Compiling 06_return.he3..."
	./he3 examples/standalone/06_return.he3
	@echo "Compiling 07_control.he3..."
	./he3 examples/standalone/07_control.he3
	@echo "Compiling 08_functions.he3..."
	./he3 examples/standalone/08_functions.he3
	@echo "Compiling 09_classes.he3..."
	./he3 examples/standalone/09_classes.he3
	@echo "Compiling 10_async.he3..."
	./he3 examples/standalone/10_async.he3
	@echo "All examples compiled!"

# Example execution targets
run_examples: examples
	@echo "Running examples..."
	@echo "Running 01_hello.bx..."
	./he3vm examples/standalone/01_hello.bx
	@echo "Running 02_arithmetic.bx..."
	./he3vm examples/standalone/02_arithmetic.bx
	@echo "Running 03_simple.bx..."
	./he3vm examples/standalone/03_simple.bx
	@echo "Running 04_locals.bx..."
	./he3vm examples/standalone/04_locals.bx
	@echo "Running 05_binary.bx..."
	./he3vm examples/standalone/05_binary.bx
	@echo "Running 06_return.bx..."
	./he3vm examples/standalone/06_return.bx
	@echo "Running 07_control.bx..."
	./he3vm examples/standalone/07_control.bx
	@echo "Running 08_functions.bx..."
	./he3vm examples/standalone/08_functions.bx
	@echo "Running 09_classes.bx..."
	./he3vm examples/standalone/09_classes.bx
	@echo "Running 10_async.bx..."
	./he3vm examples/standalone/10_async.he3
	@echo "All examples executed!"

# Debug targets
debug: CFLAGS += -DDEBUG -g3
debug: he3 he3vm

# Release targets
release: CFLAGS += -O3 -DNDEBUG
release: clean he3 he3vm

# Install targets
install: he3 he3vm
	@echo "Installing He³ compiler and VM..."
	cp he3 /usr/local/bin/
	cp he3vm /usr/local/bin/
	@echo "Installation complete!"

# Uninstall targets
uninstall:
	@echo "Uninstalling He³ compiler and VM..."
	rm -f /usr/local/bin/he3
	rm -f /usr/local/bin/he3vm
	@echo "Uninstallation complete!"

# Help target
help:
	@echo "He³ Compiler Makefile"
	@echo "Available targets:"
	@echo "  all          - Build both compiler and VM (default)"
	@echo "  he3          - Build compiler only"
	@echo "  he3vm        - Build VM only"
	@echo "  test_lexer   - Build lexer test"
	@echo "  test_parser  - Build parser test"
	@echo "  test         - Run all tests"
	@echo "  examples     - Compile all examples"
	@echo "  run_examples - Compile and run all examples"
	@echo "  debug        - Build with debug flags"
	@echo "  release      - Build with release flags"
	@echo "  clean        - Clean build files"
	@echo "  clean_all    - Clean all generated files"
	@echo "  install      - Install to /usr/local/bin"
	@echo "  uninstall    - Remove from /usr/local/bin"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all he3 he3vm test_lexer test_parser test examples run_examples debug release clean clean_all install uninstall help