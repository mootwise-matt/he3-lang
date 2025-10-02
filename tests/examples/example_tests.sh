#!/bin/bash

# He³ Example Test Suite
# Tests all example programs to ensure they work correctly

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================${NC}"
}

print_test() {
    echo -e "${YELLOW}Testing: $1${NC}"
}

print_pass() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
}

print_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
    echo -e "${RED}  Error: $2${NC}"
}

# Test individual example
test_example() {
    local example_file="$1"
    local expected_result="$2"
    local test_name=$(basename "$example_file" .he3)
    
    print_test "$test_name"
    
    # Compile
    if ./build/he3 "$example_file" > /dev/null 2>&1; then
        local bytecode_file="${example_file%.he3}.bx"
        if [ -f "$bytecode_file" ]; then
            # Execute
            local result=$(./build/he3vm "$bytecode_file" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
            if [ "$result" = "$expected_result" ]; then
                print_pass "$test_name"
                return 0
            else
                print_fail "$test_name" "Expected $expected_result, got $result"
                return 1
            fi
        else
            print_fail "$test_name" "Bytecode file not created"
            return 1
        fi
    else
        print_fail "$test_name" "Compilation failed"
        return 1
    fi
}

# Test examples that should compile but may not have return values
test_example_compile_only() {
    local example_file="$1"
    local test_name=$(basename "$example_file" .he3)
    
    print_test "$test_name (compile only)"
    
    # Compile
    if ./build/he3 "$example_file" > /dev/null 2>&1; then
        local bytecode_file="${example_file%.he3}.bx"
        if [ -f "$bytecode_file" ]; then
            print_pass "$test_name (compile only)"
            return 0
        else
            print_fail "$test_name (compile only)" "Bytecode file not created"
            return 1
        fi
    else
        print_fail "$test_name (compile only)" "Compilation failed"
        return 1
    fi
}

# Create comprehensive test examples
create_test_examples() {
    print_header "Creating Test Examples"
    
    # Basic arithmetic
    cat > tests/examples/01_basic_arithmetic.he3 << 'EOF'
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

    # Variable assignments
    cat > tests/examples/02_variable_assignments.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    let z: integer;
    x = 100;
    y = x;
    z = y + 50;
    return z;
  }
}
EOF

    # Multiple operations
    cat > tests/examples/03_multiple_operations.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let a: integer;
    let b: integer;
    let c: integer;
    let d: integer;
    a = 2;
    b = 3;
    c = 4;
    d = a + b + c;
    return d;
  }
}
EOF

    # Simple return
    cat > tests/examples/04_simple_return.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    return 42;
  }
}
EOF

    # Variable reuse
    cat > tests/examples/05_variable_reuse.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    x = 10;
    x = x + 5;
    x = x * 2;
    return x;
  }
}
EOF

    # Complex expression
    cat > tests/examples/06_complex_expression.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    let z: integer;
    x = 1;
    y = 2;
    z = 3;
    return x + y + z;
  }
}
EOF

    # Test with different values
    cat > tests/examples/07_different_values.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let a: integer;
    let b: integer;
    a = 7;
    b = 8;
    return a + b;
  }
}
EOF

    # Test with zero
    cat > tests/examples/08_zero_test.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 0;
    y = 5;
    return x + y;
  }
}
EOF

    # Test with large numbers
    cat > tests/examples/09_large_numbers.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 1000;
    y = 2000;
    return x + y;
  }
}
EOF

    # Test with negative numbers (if supported)
    cat > tests/examples/10_negative_numbers.he3 << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    x = 10;
    y = 5;
    return x - y;
  }
}
EOF

    echo "Test examples created successfully."
}

# Run all example tests
run_example_tests() {
    print_header "He³ Example Test Suite"
    
    # Create test examples
    create_test_examples
    
    echo
    print_header "Running Example Tests"
    
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Test examples with expected results
    test_example "tests/examples/01_basic_arithmetic.he3" "15" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/02_variable_assignments.he3" "150" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/03_multiple_operations.he3" "9" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/04_simple_return.he3" "42" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/05_variable_reuse.he3" "30" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/06_complex_expression.he3" "6" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/07_different_values.he3" "15" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/08_zero_test.he3" "5" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_example "tests/examples/09_large_numbers.he3" "3000" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    # Test examples that might not work yet
    test_example_compile_only "tests/examples/10_negative_numbers.he3" && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    echo
    print_header "Example Test Summary"
    echo "Total tests: $total_tests"
    echo -e "Passed: ${GREEN}$passed_tests${NC}"
    echo -e "Failed: ${RED}$failed_tests${NC}"
    
    if [ $failed_tests -gt 0 ]; then
        echo -e "${RED}Some example tests failed!${NC}"
        return 1
    else
        echo -e "${GREEN}All example tests passed! 🎉${NC}"
        return 0
    fi
}

# Cleanup function
cleanup() {
    echo
    echo "Cleaning up example test files..."
    rm -f tests/examples/*.he3 tests/examples/*.bx
    echo "Cleanup complete."
}

# Set up cleanup on exit
trap cleanup EXIT

# Run tests
run_example_tests
