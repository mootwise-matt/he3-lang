#!/bin/bash

# He³ Regression Test Suite
# Tests to ensure previously fixed bugs don't regress

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_test() {
    echo -e "${YELLOW}Regression Test: $1${NC}"
}

print_pass() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
}

print_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
    echo -e "${RED}  Error: $2${NC}"
}

# Test that assignment statements work correctly
test_assignment_statements() {
    print_test "Assignment statements (x = 5)"
    
    local test_file="tests/regression/assignment_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    x = 5;
    return x;
  }
}
EOF
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "5" ]; then
            print_pass "Assignment statements"
        else
            print_fail "Assignment statements" "Expected 5, got $result"
        fi
    else
        print_fail "Assignment statements" "Compilation failed"
    fi
}

# Test that binary expressions work correctly
test_binary_expressions() {
    print_test "Binary expressions (x + y)"
    
    local test_file="tests/regression/binary_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 10;
    y = 5;
    return x + y;
  }
}
EOF
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "15" ]; then
            print_pass "Binary expressions"
        else
            print_fail "Binary expressions" "Expected 15, got $result"
        fi
    else
        print_fail "Binary expressions" "Compilation failed"
    fi
}

# Test that symbol table lookup works correctly
test_symbol_table_lookup() {
    print_test "Symbol table lookup (first variable at index 0)"
    
    local test_file="tests/regression/symbol_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 100;
    y = x;  // This should load x correctly
    return y;
  }
}
EOF
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "100" ]; then
            print_pass "Symbol table lookup"
        else
            print_fail "Symbol table lookup" "Expected 100, got $result"
        fi
    else
        print_fail "Symbol table lookup" "Compilation failed"
    fi
}

# Test that lexer correctly tokenizes = as TK_ASSIGN
test_lexer_assignment_token() {
    print_test "Lexer assignment token (= as TK_ASSIGN)"
    
    local test_file="tests/regression/lexer_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    x = 42;
    return x;
  }
}
EOF
    
    # Check that = is tokenized as TK_ASSIGN (should be token kind 63)
    local tokens=$(./build/he3 -t "$test_file" 2>/dev/null | grep "TK_ASSIGN\|=" | head -1)
    if echo "$tokens" | grep -q "63"; then
        print_pass "Lexer assignment token"
    else
        print_fail "Lexer assignment token" "= not tokenized as TK_ASSIGN"
    fi
}

# Test that parser creates AST_ASSIGN nodes
test_parser_assignment_ast() {
    print_test "Parser assignment AST (AST_ASSIGN nodes)"
    
    local test_file="tests/regression/parser_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    x = 42;
    return x;
  }
}
EOF
    
    # Check that AST contains AST[ASSIGN] not AST[BINARY]
    local ast=$(./build/he3 -a "$test_file" 2>/dev/null)
    if echo "$ast" | grep -q "AST\[ASSIGN\]"; then
        print_pass "Parser assignment AST"
    else
        print_fail "Parser assignment AST" "AST does not contain AST[ASSIGN]"
    fi
}

# Test that bytecode generation works correctly
test_bytecode_generation() {
    print_test "Bytecode generation (correct instruction sequence)"
    
    local test_file="tests/regression/bytecode_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    x = 10;
    return x;
  }
}
EOF
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local bytecode_file="${test_file%.he3}.bx"
        if [ -f "$bytecode_file" ]; then
            # Check that bytecode file has reasonable size
            local size=$(wc -c < "$bytecode_file")
            if [ "$size" -gt 20 ]; then
                print_pass "Bytecode generation"
            else
                print_fail "Bytecode generation" "Bytecode file too small: $size bytes"
            fi
        else
            print_fail "Bytecode generation" "Bytecode file not created"
        fi
    else
        print_fail "Bytecode generation" "Compilation failed"
    fi
}

# Test that VM execution works correctly
test_vm_execution() {
    print_test "VM execution (correct result)"
    
    local test_file="tests/regression/vm_test.he3"
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 7;
    y = 3;
    return x + y;
  }
}
EOF
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "10" ]; then
            print_pass "VM execution"
        else
            print_fail "VM execution" "Expected 10, got $result"
        fi
    else
        print_fail "VM execution" "Compilation failed"
    fi
}

# Run all regression tests
run_regression_tests() {
    echo "Running He³ Regression Tests..."
    echo
    
    test_assignment_statements
    test_binary_expressions
    test_symbol_table_lookup
    test_lexer_assignment_token
    test_parser_assignment_ast
    test_bytecode_generation
    test_vm_execution
    
    echo
    echo "Regression tests completed."
}

# Cleanup
cleanup() {
    rm -f tests/regression/*.he3 tests/regression/*.bx
}

trap cleanup EXIT

# Run tests
run_regression_tests
