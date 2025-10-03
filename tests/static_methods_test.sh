#!/bin/bash

# Static Methods Test Suite
# Tests the static method functionality in He³

set -e

echo "🧪 Running Static Methods Test Suite"
echo "===================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_dir="$2"
    local expected_output="$3"
    
    echo -n "Testing $test_name... "
    
    # Compile the test
    if ./he3build "$test_dir/he3project.json" > /dev/null 2>&1; then
        # Run the test and capture output
        local actual_output=$(./he3vm build/output.helium3 2>&1)
        local exit_code=$?
        
        if [ $exit_code -eq 0 ]; then
            if [ -n "$expected_output" ]; then
                if echo "$actual_output" | grep -q "$expected_output"; then
                    echo -e "${GREEN}PASS${NC}"
                    ((TESTS_PASSED++))
                else
                    echo -e "${RED}FAIL${NC} (output mismatch)"
                    echo "Expected: $expected_output"
                    echo "Actual: $actual_output"
                    ((TESTS_FAILED++))
                fi
            else
                echo -e "${GREEN}PASS${NC}"
                ((TESTS_PASSED++))
            fi
        else
            echo -e "${RED}FAIL${NC} (exit code: $exit_code)"
            echo "Output: $actual_output"
            ((TESTS_FAILED++))
        fi
    else
        echo -e "${RED}FAIL${NC} (compilation failed)"
        ((TESTS_FAILED++))
    fi
}

# Test 1: Basic static method calls
echo ""
echo "Test 1: Basic Static Method Calls"
run_test "Sys.print() and Sys.println()" \
         "examples/standalone/03_print_demo" \
         "Hello from He³!"

# Test 2: Comprehensive static method test
echo ""
echo "Test 2: Comprehensive Static Method Test"
run_test "Multiple static method calls" \
         "examples/standalone/04_static_methods_test" \
         "Test 1:"

# Test 3: Static method arguments
echo ""
echo "Test 3: Static Method Arguments"
run_test "Different argument types" \
         "examples/standalone/05_static_args_test" \
         "Test 1: Empty string:"

# Summary
echo ""
echo "===================================="
echo "Test Results:"
echo "✅ Passed: $TESTS_PASSED"
echo "❌ Failed: $TESTS_FAILED"
echo "Total: $((TESTS_PASSED + TESTS_FAILED))"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}💥 Some tests failed!${NC}"
    exit 1
fi
