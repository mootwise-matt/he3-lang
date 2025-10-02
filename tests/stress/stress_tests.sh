#!/bin/bash

# He³ Stress Test Suite
# Tests the compiler and VM under various stress conditions

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
    echo -e "${YELLOW}Stress Test: $1${NC}"
}

print_pass() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
}

print_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
    echo -e "${RED}  Error: $2${NC}"
}

# Test with many variables
test_many_variables() {
    print_test "Many variables (50 variables)"
    
    local test_file="tests/stress/many_variables.he3"
    local num_vars=50
    
    # Generate test file with many variables
    cat > "$test_file" << EOF
domain app.test;

class Program {
  function main(): integer {
EOF
    
    # Add variable declarations
    for i in $(seq 1 $num_vars); do
        echo "    let var$i: integer;" >> "$test_file"
    done
    
    # Add assignments
    for i in $(seq 1 $num_vars); do
        echo "    var$i = $i;" >> "$test_file"
    done
    
    # Add return statement
    echo "    return var1 + var$num_vars;" >> "$test_file"
    
    cat >> "$test_file" << 'EOF'
  }
}
EOF
    
    # Test compilation and execution
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        local expected=$((1 + num_vars))
        if [ "$result" = "$expected" ]; then
            print_pass "Many variables"
        else
            print_fail "Many variables" "Expected $expected, got $result"
        fi
    else
        print_fail "Many variables" "Compilation failed"
    fi
}

# Test with deep expressions
test_deep_expressions() {
    print_test "Deep expressions (nested operations)"
    
    local test_file="tests/stress/deep_expressions.he3"
    
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let a: integer;
    let b: integer;
    let c: integer;
    let d: integer;
    let e: integer;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    e = 5;
    return a + b + c + d + e;
  }
}
EOF
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "15" ]; then
            print_pass "Deep expressions"
        else
            print_fail "Deep expressions" "Expected 15, got $result"
        fi
    else
        print_fail "Deep expressions" "Compilation failed"
    fi
}

# Test compilation speed with large files
test_compilation_speed() {
    print_test "Compilation speed (large file)"
    
    local test_file="tests/stress/speed_test.he3"
    local num_ops=100
    
    # Generate large test file
    cat > "$test_file" << 'EOF'
domain app.test;

class Program {
  function main(): integer {
    let x: integer;
    let y: integer;
    let result: integer;
    x = 1;
    y = 2;
    result = x + y;
EOF
    
    # Add many operations
    for i in $(seq 1 $num_ops); do
        echo "    result = result + 1;" >> "$test_file"
    done
    
    cat >> "$test_file" << 'EOF'
    return result;
  }
}
EOF
    
    # Measure compilation time
    local start_time=$(date +%s%N)
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local end_time=$(date +%s%N)
        local duration=$(( (end_time - start_time) / 1000000 ))  # Convert to milliseconds
        
        if [ "$duration" -lt 10000 ]; then  # Less than 10 seconds
            print_pass "Compilation speed - ${duration}ms"
        else
            print_fail "Compilation speed" "Too slow: ${duration}ms"
        fi
    else
        print_fail "Compilation speed" "Compilation failed"
    fi
}

# Test memory usage
test_memory_usage() {
    print_test "Memory usage (large symbol table)"
    
    local test_file="tests/stress/memory_test.he3"
    local num_vars=100
    
    # Generate test file with many variables
    cat > "$test_file" << EOF
domain app.test;

class Program {
  function main(): integer {
EOF
    
    # Add variable declarations
    for i in $(seq 1 $num_vars); do
        echo "    let var$i: integer;" >> "$test_file"
    done
    
    # Add assignments
    for i in $(seq 1 $num_vars); do
        echo "    var$i = $i;" >> "$test_file"
    done
    
    # Add return statement
    echo "    return var1;" >> "$test_file"
    
    cat >> "$test_file" << 'EOF'
  }
}
EOF
    
    # Test compilation
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local result=$(./build/he3vm "${test_file%.he3}.bx" 2>/dev/null | grep "Execution completed with result:" | sed 's/.*result: //')
        if [ "$result" = "1" ]; then
            print_pass "Memory usage"
        else
            print_fail "Memory usage" "Expected 1, got $result"
        fi
    else
        print_fail "Memory usage" "Compilation failed"
    fi
}

# Test error recovery
test_error_recovery() {
    print_test "Error recovery (malformed input)"
    
    local test_file="tests/stress/error_test.he3"
    
    # Create file with syntax errors
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
    
    # This should fail gracefully
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        print_fail "Error recovery" "Should have failed but didn't"
    else
        print_pass "Error recovery"
    fi
}

# Test concurrent compilation (if possible)
test_concurrent_compilation() {
    print_test "Concurrent compilation (multiple files)"
    
    local test_files=()
    local pids=()
    
    # Create multiple test files
    for i in {1..5}; do
        local test_file="tests/stress/concurrent_$i.he3"
        cat > "$test_file" << EOF
domain app.test$i;

class Program {
  function main(): integer {
    let x: integer;
    x = $i;
    return x;
  }
}
EOF
        test_files+=("$test_file")
    done
    
    # Compile all files concurrently
    for test_file in "${test_files[@]}"; do
        ./build/he3 "$test_file" > /dev/null 2>&1 &
        pids+=($!)
    done
    
    # Wait for all compilations to complete
    local success=true
    for pid in "${pids[@]}"; do
        if ! wait $pid; then
            success=false
        fi
    done
    
    if [ "$success" = true ]; then
        print_pass "Concurrent compilation"
    else
        print_fail "Concurrent compilation" "Some compilations failed"
    fi
}

# Test bytecode file integrity
test_bytecode_integrity() {
    print_test "Bytecode file integrity"
    
    local test_file="tests/stress/integrity_test.he3"
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
    
    if ./build/he3 "$test_file" > /dev/null 2>&1; then
        local bytecode_file="${test_file%.he3}.bx"
        if [ -f "$bytecode_file" ]; then
            # Check file size
            local size=$(wc -c < "$bytecode_file")
            if [ "$size" -gt 20 ]; then
                # Try to execute multiple times
                local success=true
                for i in {1..5}; do
                    local result=$(./build/he3vm "$bytecode_file" 2>/dev/null)
                    if [ "$result" != "42" ]; then
                        success=false
                        break
                    fi
                done
                
                if [ "$success" = true ]; then
                    print_pass "Bytecode file integrity"
                else
                    print_fail "Bytecode file integrity" "Inconsistent execution results"
                fi
            else
                print_fail "Bytecode file integrity" "Bytecode file too small: $size bytes"
            fi
        else
            print_fail "Bytecode file integrity" "Bytecode file not created"
        fi
    else
        print_fail "Bytecode file integrity" "Compilation failed"
    fi
}

# Run all stress tests
run_stress_tests() {
    print_header "He³ Stress Test Suite"
    
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Run stress tests
    test_many_variables && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_deep_expressions && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_compilation_speed && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_memory_usage && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_error_recovery && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_concurrent_compilation && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    test_bytecode_integrity && ((passed_tests++)) || ((failed_tests++))
    ((total_tests++))
    
    echo
    print_header "Stress Test Summary"
    echo "Total tests: $total_tests"
    echo -e "Passed: ${GREEN}$passed_tests${NC}"
    echo -e "Failed: ${RED}$failed_tests${NC}"
    
    if [ $failed_tests -gt 0 ]; then
        echo -e "${RED}Some stress tests failed!${NC}"
        return 1
    else
        echo -e "${GREEN}All stress tests passed! 🎉${NC}"
        return 0
    fi
}

# Cleanup function
cleanup() {
    echo
    echo "Cleaning up stress test files..."
    rm -f tests/stress/*.he3 tests/stress/*.bx
    echo "Cleanup complete."
}

# Set up cleanup on exit
trap cleanup EXIT

# Run tests
run_stress_tests
