#!/bin/bash

# He³ Example Test Suite
# Tests all example programs using the new helium3/ directory structure

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

# Test individual example using he3build
test_example() {
    local example_dir="$1"
    local expected_result="$2"
    local test_name=$(basename "$example_dir")
    
    print_test "$test_name"
    
    # Build the example
    if ./he3build "$example_dir/he3project.json" > /dev/null 2>&1; then
        local output_file="helium3/standalone/${test_name}.helium3"
        if [ -f "$output_file" ]; then
            # Execute
            ./he3vm "$output_file" >/dev/null 2>&1
            local result=$?
            if [ "$result" = "$expected_result" ]; then
                print_pass "$test_name"
                return 0
            else
                print_fail "$test_name" "Expected exit code $expected_result, got $result"
                return 1
            fi
        else
            print_fail "$test_name" "Output file not created: $output_file"
            return 1
        fi
    else
        print_fail "$test_name" "Build failed"
        return 1
    fi
}

# Test Option/Result examples
test_option_examples() {
    print_header "Testing Option/Result Examples"
    
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Test basic Option
    if [ -f "tmp/test_simple_option.he3" ]; then
        print_test "test_simple_option"
        if ./he3 -m tmp/test_simple_option.he3 > /dev/null 2>&1; then
            ./he3vm tmp/test_simple_option.helium3 >/dev/null 2>&1
            local result=$?
            if [ "$result" = "42" ]; then
                print_pass "test_simple_option"
                ((passed_tests++))
            else
                print_fail "test_simple_option" "Expected exit code 42, got $result"
                ((failed_tests++))
            fi
        else
            print_fail "test_simple_option" "Compilation failed"
            ((failed_tests++))
        fi
        ((total_tests++))
    fi
    
    # Test match statement
    if [ -f "tmp/test_match_simple.he3" ]; then
        print_test "test_match_simple"
        if ./he3 -m tmp/test_match_simple.he3 > /dev/null 2>&1; then
            ./he3vm tmp/test_match_simple.helium3 >/dev/null 2>&1
            local result=$?
            if [ "$result" = "42" ]; then
                print_pass "test_match_simple"
                ((passed_tests++))
            else
                print_fail "test_match_simple" "Expected exit code 42, got $result"
                ((failed_tests++))
            fi
        else
            print_fail "test_match_simple" "Compilation failed"
            ((failed_tests++))
        fi
        ((total_tests++))
    fi
    
    # Test if statement with Option
    if [ -f "tmp/test_simple_match.he3" ]; then
        print_test "test_simple_match"
        if ./he3 -m tmp/test_simple_match.he3 > /dev/null 2>&1; then
            ./he3vm tmp/test_simple_match.helium3 >/dev/null 2>&1
            local result=$?
            if [ "$result" = "42" ]; then
                print_pass "test_simple_match"
                ((passed_tests++))
            else
                print_fail "test_simple_match" "Expected exit code 42, got $result"
                ((failed_tests++))
            fi
        else
            print_fail "test_simple_match" "Compilation failed"
            ((failed_tests++))
        fi
        ((total_tests++))
    fi
    
    echo
    echo "Option/Result tests: $passed_tests/$total_tests passed"
    return $failed_tests
}

# Run all example tests
run_example_tests() {
    print_header "He³ Example Test Suite"
    
    # Create helium3 directory if it doesn't exist
    mkdir -p helium3/standalone
    
    echo
    print_header "Testing Standalone Examples"
    
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Test all standalone examples
    for dir in examples/standalone/*/; do
        if [ -d "$dir" ]; then
            name=$(basename "$dir")
            # Most examples return 0, but some have specific expected results
            expected_result="0"
            case "$name" in
                "02_arithmetic") expected_result="50" ;;
                "04_print_test") expected_result="42" ;;
            esac
            
            if test_example "$dir" "$expected_result"; then
                ((passed_tests++))
            else
                ((failed_tests++))
            fi
            ((total_tests++))
        fi
    done
    
    echo
    print_header "Testing Option/Result Examples"
    
    # Test Option/Result examples
    local option_failed=0
    if test_option_examples; then
        echo "All Option/Result tests passed"
    else
        option_failed=$?
        echo "Some Option/Result tests failed"
    fi
    
    echo
    print_header "Example Test Summary"
    echo "Standalone examples: $passed_tests/$total_tests passed"
    echo "Option/Result examples: $((3 - option_failed))/3 passed"
    echo "Total tests: $((total_tests + 3))"
    echo -e "Passed: ${GREEN}$((passed_tests + 3 - option_failed))${NC}"
    echo -e "Failed: ${RED}$((failed_tests + option_failed))${NC}"
    
    if [ $((failed_tests + option_failed)) -gt 0 ]; then
        echo -e "${RED}Some example tests failed!${NC}"
        return 1
    else
        echo -e "${GREEN}All example tests passed! 🎉${NC}"
        return 0
    fi
}

# Run tests
run_example_tests