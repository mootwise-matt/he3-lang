# He³ Compiler Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Isrc/shared -Isrc/compiler -Isrc/vm
SRCDIR = src
BUILDDIR = build
TESTDIR = $(SRCDIR)/compiler/tests

# Shared source files
SHARED_SOURCES = $(SRCDIR)/shared/bytecode/constant_table.c $(SRCDIR)/shared/ast/ast.c $(SRCDIR)/shared/stdlib/sys.c

# Compiler source files
LEXER_SOURCES = $(SRCDIR)/compiler/lexer/lexer.c
PARSER_SOURCES = $(SRCDIR)/compiler/parser/parser.c
AST_SOURCES = $(SRCDIR)/shared/ast/ast.c
MAIN_SOURCES = $(SRCDIR)/compiler/main.c
PACKAGER_SOURCES = $(SRCDIR)/compiler/packager/packager.c
BUILD_SOURCES = $(SRCDIR)/compiler/he3build.c

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
VM_MODULE_SOURCES = $(SRCDIR)/vm/modules/module_registry.c
VM_BYTECODE_FILE_SOURCES = $(SRCDIR)/vm/bytecode/bytecode_file.c
VM_OPCODE_UTILS_SOURCES = $(SRCDIR)/vm/bytecode/opcode_utils.c
VM_HELIUM_MODULE_SOURCES = $(SRCDIR)/vm/bytecode/helium_module.c
VM_MAIN_SOURCES = $(SRCDIR)/vm/main.c

# Test source files
TEST_SOURCES = $(TESTDIR)/lexer_test.c $(TESTDIR)/parser_test.c

# Object files
SHARED_OBJECTS = $(BUILDDIR)/constant_table.o $(BUILDDIR)/ast.o $(BUILDDIR)/sys.o
LEXER_OBJECTS = $(BUILDDIR)/lexer.o
PARSER_OBJECTS = $(BUILDDIR)/parser.o
# AST_OBJECTS = $(BUILDDIR)/ast.o  # Now using shared AST
MAIN_OBJECTS = $(BUILDDIR)/main.o
PACKAGER_OBJECTS = $(BUILDDIR)/packager.o
BUILD_OBJECTS = $(BUILDDIR)/he3build.o

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
VM_MODULE_OBJECTS = $(BUILDDIR)/module_registry.o
VM_BYTECODE_FILE_OBJECTS = $(BUILDDIR)/bytecode_file.o
VM_OPCODE_UTILS_OBJECTS = $(BUILDDIR)/opcode_utils.o
VM_HELIUM_MODULE_OBJECTS = $(BUILDDIR)/helium_module.o
VM_MAIN_OBJECTS = $(BUILDDIR)/vm_main.o

# Test object files
TEST_OBJECTS = $(BUILDDIR)/lexer_test.o $(BUILDDIR)/parser_test.o

# All source files
ALL_SOURCES = $(SHARED_SOURCES) $(LEXER_SOURCES) $(PARSER_SOURCES) $(AST_SOURCES) $(MAIN_SOURCES) $(IR_SOURCES) $(BYTECODE_SOURCES) $(IR_TO_BYTECODE_SOURCES) $(AST_TO_IR_SOURCES) $(BYTECODE_FILE_SOURCES) $(HELIUM_MODULE_SOURCES) $(VM_SOURCES) $(VM_LOADER_SOURCES) $(VM_EXECUTION_SOURCES) $(VM_MEMORY_SOURCES) $(VM_OBJECT_SOURCES) $(VM_MODULE_SOURCES) $(VM_BYTECODE_FILE_SOURCES) $(VM_OPCODE_UTILS_SOURCES) $(VM_HELIUM_MODULE_SOURCES) $(VM_MAIN_SOURCES)

# All object files
ALL_OBJECTS = $(SHARED_OBJECTS) $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(MAIN_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) $(BYTECODE_FILE_OBJECTS) $(HELIUM_MODULE_OBJECTS) $(VM_OBJECTS) $(VM_LOADER_OBJECTS) $(VM_EXECUTION_SOURCES) $(VM_MEMORY_SOURCES) $(VM_OBJECT_OBJECTS) $(VM_MODULE_OBJECTS) $(VM_BYTECODE_FILE_OBJECTS) $(VM_OPCODE_UTILS_OBJECTS) $(VM_HELIUM_MODULE_OBJECTS) $(VM_MAIN_OBJECTS)

# Main targets
all: he3 he3vm he3build

# Compiler executable
he3: $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(MAIN_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) $(BYTECODE_FILE_OBJECTS) $(HELIUM_MODULE_OBJECTS) $(SHARED_OBJECTS)
	@echo "Building He³ compiler..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "Compiler built successfully!"

# VM executable
he3vm: $(VM_OBJECTS) $(VM_LOADER_OBJECTS) $(VM_EXECUTION_OBJECTS) $(VM_MEMORY_OBJECTS) $(VM_OBJECT_OBJECTS) $(VM_MODULE_OBJECTS) $(VM_BYTECODE_FILE_OBJECTS) $(VM_OPCODE_UTILS_OBJECTS) $(VM_HELIUM_MODULE_OBJECTS) $(VM_MAIN_OBJECTS) $(SHARED_OBJECTS)
	@echo "Building He³ VM..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "VM built successfully!"

# Build system executable
he3build: $(BUILD_OBJECTS) $(PACKAGER_OBJECTS) $(LEXER_OBJECTS) $(PARSER_OBJECTS) $(IR_OBJECTS) $(BYTECODE_OBJECTS) $(IR_TO_BYTECODE_OBJECTS) $(AST_TO_IR_OBJECTS) $(BYTECODE_FILE_OBJECTS) $(HELIUM_MODULE_OBJECTS) $(SHARED_OBJECTS)
	@echo "Building He³ Build System..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "Build system built successfully!"

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

$(BUILDDIR)/%.o: $(SRCDIR)/shared/stdlib/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/lexer/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/parser/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/ir/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/emitter/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/packager/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/bytecode/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/execution/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/loader/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/memory/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/object/%.c
$(BUILDDIR)/%.o: $(SRCDIR)/vm/objects/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/modules/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/vm/bytecode/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/compiler/tests/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILDDIR)
	rm -f he3 he3vm test_lexer test_parser
	@echo "Clean complete!"

# Test targets
test: test_lexer test_parser
	@echo "Running tests..."
	./test_lexer
	./test_parser
	@echo "Tests completed!"

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build compiler and VM"
	@echo "  he3     - Build compiler only"
	@echo "  he3vm   - Build VM only"
	@echo "  test    - Run tests"
	@echo "  clean   - Clean build files"
	@echo "  help    - Show this help"

.PHONY: all he3 he3vm test clean help
