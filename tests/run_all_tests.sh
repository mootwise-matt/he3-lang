#!/bin/bash

# He³ Comprehensive Test Suite Runner
# Orchestrates all test suites for the He³ compiler-VM pipeline

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

print_section() {
    echo -e "${YELLOW}$1${NC}"
}

print_pass() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
}

print_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
}

# Test suite results
declare -A suite_results
declare -A suite_times

# Run a test suite and capture results
run_test_suite() {
    local suite_name="$1"
    local suite_script="$2"
    
    print_section "Running $suite_name..."
    
    local start_time=$(date +%s%N)
    
    if bash "$suite_script" > /dev/null 2>&1; then
        local end_time=$(date +%s%N)
        local duration=$(( (end_time - start_time) / 1000000 ))
        suite_results["$suite_name"]="PASS"
        suite_times["$suite_name"]="$duration"
        print_pass "$suite_name (${duration}ms)"
        return 0
    else
        local end_time=$(date +%s%N)
        local duration=$(( (end_time - start_time) / 1000000 ))
        suite_results["$suite_name"]="FAIL"
        suite_times["$suite_name"]="$duration"
        print_fail "$suite_name (${duration}ms)"
        return 1
    fi
}

# Main test runner
main() {
    print_header "He³ Comprehensive Test Suite"
    echo "Running all test suites for the He³ compiler-VM pipeline..."
    echo
    
    local total_suites=0
    local passed_suites=0
    local failed_suites=0
    
    # Run all test suites
    if run_test_suite "Main Test Suite" "tests/run_tests.sh"; then
        ((passed_suites++))
    else
        ((failed_suites++))
    fi
    ((total_suites++))
    
    if run_test_suite "Regression Tests" "tests/regression/regression_tests.sh"; then
        ((passed_suites++))
    else
        ((failed_suites++))
    fi
    ((total_suites++))
    
    if run_test_suite "Example Tests" "tests/examples/example_tests.sh"; then
        ((passed_suites++))
    else
        ((failed_suites++))
    fi
    ((total_suites++))
    
    if run_test_suite "Stress Tests" "tests/stress/stress_tests.sh"; then
        ((passed_suites++))
    else
        ((failed_suites++))
    fi
    ((total_suites++))
    
    # Print summary
    echo
    print_header "Test Suite Summary"
    echo "Total test suites: $total_suites"
    echo -e "Passed: ${GREEN}$passed_suites${NC}"
    echo -e "Failed: ${RED}$failed_suites${NC}"
    echo
    
    # Print detailed results
    print_header "Detailed Results"
    for suite in "${!suite_results[@]}"; do
        local result="${suite_results[$suite]}"
        local time="${suite_times[$suite]}"
        if [ "$result" = "PASS" ]; then
            echo -e "${GREEN}✓${NC} $suite: ${GREEN}PASS${NC} (${time}ms)"
        else
            echo -e "${RED}✗${NC} $suite: ${RED}FAIL${NC} (${time}ms)"
        fi
    done
    
    echo
    if [ $failed_suites -gt 0 ]; then
        echo -e "${RED}Some test suites failed!${NC}"
        echo "Check individual test suite outputs for details."
        exit 1
    else
        echo -e "${GREEN}All test suites passed! 🎉${NC}"
        echo "The He³ compiler-VM pipeline is working correctly."
        exit 0
    fi
}

# Run main function
main "$@"
