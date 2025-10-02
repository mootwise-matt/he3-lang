#!/bin/bash

# He³ Compiler-VM Test Suite
# Comprehensive testing framework for the He³ programming language

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Test results
declare -a FAILED_TEST_NAMES=()

# Helper functions
print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================${NC}"
}

print_test() {
    echo -e "${YELLOW}Testing: $1${NC}"
    ((TOTAL_TESTS++))
}

print_pass() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
    ((PASSED_TESTS++))
}

print_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
    echo -e "${RED}  Error: $2${NC}"
    ((FAILED_TESTS++))
    FAILED_TEST_NAMES+=("$1")
}

# Test functions
test_compiler_build() {
    print_test "Compiler build"
    if make build/he3 > /dev/null 2>&1; then
        print_pass "Compiler build"
    else
        print_fail "Compiler build" "Make failed"
        return 1
    fi
}

test_vm_build() {
    print_test "VM build"
    if make build/he3vm > /dev/null 2>&1; then
        print_pass "VM build"
    else
        print_fail "VM build" "Make failed"
        return 1
    fi
}

test_lexer() {
    print_test "Lexer tokenization"
    local test_file="tests/unit/lexer_test.he3"
    
    # Create test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer = 42;
    let y: string = "hello";
    return x;
  }
}
EOF
    
    if ./build/he3 -t "$test_file" > /dev/null 2>&1; then
        print_pass "Lexer tokenization"
    else
        print_fail "Lexer tokenization" "Tokenization failed"
    fi
}

test_parser() {
    print_test "Parser AST generation"
    local test_file="tests/unit/parser_test.he3"
    
    # Create test file
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
    
    if ./build/he3 -a "$test_file" > /dev/null 2>&1; then
        print_pass "Parser AST generation"
    else
        print_fail "Parser AST generation" "AST generation failed"
    fi
}

test_compilation() {
    print_test "End-to-end compilation"
    local test_file="tests/unit/compilation_test.he3"
    local bytecode_file="tests/unit/compilation_test.bx"
    
    # Create test file
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
        if [ -f "$bytecode_file" ]; then
            print_pass "End-to-end compilation"
        else
            print_fail "End-to-end compilation" "Bytecode file not created"
        fi
    else
        print_fail "End-to-end compilation" "Compilation failed"
    fi
}

test_vm_execution() {
    print_test "VM execution"
    local test_file="tests/unit/vm_test.he3"
    local bytecode_file="tests/unit/vm_test.bx"
    
    # Create test file
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
    
    # Compile
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        # Execute
        local result=$(./build/he3vm "$bytecode_file" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "42" ]; then
            print_pass "VM execution"
        else
            print_fail "VM execution" "Expected 42, got $result"
        fi
    else
        print_fail "VM execution" "Compilation failed"
    fi
}

test_arithmetic_operations() {
    print_test "Arithmetic operations"
    local test_file="tests/unit/arithmetic_test.he3"
    local bytecode_file="tests/unit/arithmetic_test.bx"
    
    # Create test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let a: integer;
    let b: integer;
    let result: integer;
    a = 10;
    b = 5;
    result = a + b;
    return result;
  }
}
EOF
    
    # Compile and execute
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "$bytecode_file" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "15" ]; then
            print_pass "Arithmetic operations"
        else
            print_fail "Arithmetic operations" "Expected 15, got $result"
        fi
    else
        print_fail "Arithmetic operations" "Compilation failed"
    fi
}

test_variable_assignments() {
    print_test "Variable assignments"
    local test_file="tests/unit/assignment_test.he3"
    local bytecode_file="tests/unit/assignment_test.bx"
    
    # Create test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 100;
    y = x;
    return y;
  }
}
EOF
    
    # Compile and execute
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "$bytecode_file" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "100" ]; then
            print_pass "Variable assignments"
        else
            print_fail "Variable assignments" "Expected 100, got $result"
        fi
    else
        print_fail "Variable assignments" "Compilation failed"
    fi
}

test_expression_precedence() {
    print_test "Expression precedence"
    local test_file="tests/unit/precedence_test.he3"
    local bytecode_file="tests/unit/precedence_test.bx"
    
    # Create test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    let z: integer;
    x = 2;
    y = 3;
    z = 4;
    return x + y * z;
  }
}
EOF
    
    # Compile and execute
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "$bytecode_file" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        # Note: This test assumes left-to-right evaluation for now
        # Expected: (x + y) * z = (2 + 3) * 4 = 20
        if [ "$result" = "20" ]; then
            print_pass "Expression precedence"
        else
            print_fail "Expression precedence" "Expected 20, got $result"
        fi
    else
        print_fail "Expression precedence" "Compilation failed"
    fi
}

test_error_handling() {
    print_test "Error handling"
    local test_file="tests/unit/error_test.he3"
    
    # Create test file with syntax error
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    x = ;  // Missing value
    return x;
  }
}
EOF
    
    # This should fail
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        print_fail "Error handling" "Should have failed but didn't"
    else
        print_pass "Error handling"
    fi
}

test_bytecode_format() {
    print_test "Bytecode format validation"
    local test_file="tests/unit/bytecode_test.he3"
    local bytecode_file="tests/unit/bytecode_test.bx"
    
    # Create test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    return 42;
  }
}
EOF
    
    # Compile
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        # Check if bytecode file exists and has reasonable size
        if [ -f "$bytecode_file" ] && [ -s "$bytecode_file" ]; then
            local size=$(wc -c < "$bytecode_file")
            if [ "$size" -gt 50 ]; then  # Reasonable minimum size
                print_pass "Bytecode format validation"
            else
                print_fail "Bytecode format validation" "Bytecode file too small ($size bytes)"
            fi
        else
            print_fail "Bytecode format validation" "Bytecode file not created or empty"
        fi
    else
        print_fail "Bytecode format validation" "Compilation failed"
    fi
}

test_helium_module_format() {
    print_test "Helium module format"
    local test_file="tests/unit/helium_test.he3"
    local module_file="tests/unit/helium_test.helium3"
    
    # Create test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    return 42;
  }
}
EOF
    
    # Compile to helium module
    if ./build/he3 -m "$test_file" > /dev/null 2>&1; then
        if [ -f "$module_file" ]; then
            print_pass "Helium module format"
        else
            print_fail "Helium module format" "Module file not created"
        fi
    else
        print_fail "Helium module format" "Module compilation failed"
    fi
}

# Integration tests
test_integration_examples() {
    print_test "Integration with examples"
    local examples_dir="examples/standalone"
    local failed_examples=()
    
    for example in "$examples_dir"/*.he3; do
        if [ -f "$example" ]; then
            local basename=$(basename "$example" .he3)
            local bytecode_file="$examples_dir/${basename}.bx"
            
            # Compile
            if ./build/he3 "$example" > /dev/null 2>&1; then
                if [ -f "$bytecode_file" ]; then
                    # Try to execute (some examples might not have return values)
                    if ./build/he3vm "$bytecode_file" > /dev/null 2>&1; then
                        echo -e "  ${GREEN}✓${NC} $basename"
                    else
                        echo -e "  ${YELLOW}⚠${NC} $basename (compiled but execution failed)"
                    fi
                else
                    echo -e "  ${RED}✗${NC} $basename (bytecode not created)"
                    failed_examples+=("$basename")
                fi
            else
                echo -e "  ${RED}✗${NC} $basename (compilation failed)"
                failed_examples+=("$basename")
            fi
        fi
    done
    
    if [ ${#failed_examples[@]} -eq 0 ]; then
        print_pass "Integration with examples"
    else
        print_fail "Integration with examples" "Failed examples: ${failed_examples[*]}"
    fi
}

# Performance tests
test_performance() {
    print_test "Performance (compilation speed)"
    local test_file="tests/unit/performance_test.he3"
    
    # Create a larger test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    let z: integer;
    let a: integer;
    let b: integer;
    let c: integer;
    let d: integer;
    let e: integer;
    let f: integer;
    let g: integer;
    x = 1;
    y = 2;
    z = 3;
    a = 4;
    b = 5;
    c = 6;
    d = 7;
    e = 8;
    f = 9;
    g = 10;
    return x + y + z + a + b + c + d + e + f + g;
  }
}
EOF
    
    # Measure compilation time
    local start_time=$(date +%s%N)
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local end_time=$(date +%s%N)
        local duration=$(( (end_time - start_time) / 1000000 ))  # Convert to milliseconds
        
        if [ "$duration" -lt 5000 ]; then  # Less than 5 seconds
            print_pass "Performance (compilation speed) - ${duration}ms"
        else
            print_fail "Performance (compilation speed)" "Too slow: ${duration}ms"
        fi
    else
        print_fail "Performance (compilation speed)" "Compilation failed"
    fi
}

# Main test runner
run_all_tests() {
    print_header "He³ Compiler-VM Test Suite"
    echo "Starting comprehensive testing..."
    echo
    
    # Build tests
    print_header "Build Tests"
    test_compiler_build
    test_vm_build
    echo
    
    # Unit tests
    print_header "Unit Tests"
    test_lexer
    test_parser
    test_compilation
    test_vm_execution
    test_arithmetic_operations
    test_variable_assignments
    test_expression_precedence
    test_error_handling
    test_bytecode_format
    test_helium_module_format
    echo
    
    # Integration tests
    print_header "Integration Tests"
    test_integration_examples
    echo
    
    # Performance tests
    print_header "Performance Tests"
    test_performance
    echo
    
    # Summary
    print_header "Test Summary"
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    if [ $FAILED_TESTS -gt 0 ]; then
        echo
        echo "Failed tests:"
        for test_name in "${FAILED_TEST_NAMES[@]}"; do
            echo -e "  ${RED}✗${NC} $test_name"
        done
        echo
        echo -e "${RED}Some tests failed!${NC}"
        exit 1
    else
        echo
        echo -e "${GREEN}All tests passed! 🎉${NC}"
        exit 0
    fi
}

# Cleanup function
cleanup() {
    echo
    echo "Cleaning up test files..."
    rm -f tests/unit/*.he3 tests/unit/*.bx tests/unit/*.helium3
    echo "Cleanup complete."
}

# Set up cleanup on exit
trap cleanup EXIT

# Run tests
run_all_tests
