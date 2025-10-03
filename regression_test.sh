#!/bin/bash

# He³ Comprehensive Regression Test Script
# This script provides stable, reproducible testing of all He³ language features
# Run from the project root directory: ./regression_test.sh

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
TEST_DIR="/tmp/he3_regression_test_$(date +%s)"
EXPECTED_OUTPUT_DIR="tests/expected_outputs"
VERBOSE=false
CLEANUP=true

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --no-cleanup)
            CLEANUP=false
            shift
            ;;
        -h|--help)
            echo "He³ Regression Test Script"
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose     Show detailed output"
            echo "  --no-cleanup      Keep test files after completion"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

# Test results array
declare -a TEST_RESULTS=()

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

# Test result tracking
record_test() {
    local test_name="$1"
    local status="$2"
    local message="$3"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    case $status in
        "PASS")
            PASSED_TESTS=$((PASSED_TESTS + 1))
            log_success "$test_name: $message"
            ;;
        "FAIL")
            FAILED_TESTS=$((FAILED_TESTS + 1))
            log_error "$test_name: $message"
            ;;
        "SKIP")
            SKIPPED_TESTS=$((SKIPPED_TESTS + 1))
            log_warning "$test_name: $message"
            ;;
    esac
    
    TEST_RESULTS+=("$status|$test_name|$message")
}

# Check if tools are available
check_prerequisites() {
    log_info "Checking prerequisites..."
    
    if [ ! -f "./he3" ]; then
        log_error "He³ compiler not found. Please run 'make' first."
        exit 1
    fi
    
    if [ ! -f "./he3vm" ]; then
        log_error "He³ VM not found. Please run 'make' first."
        exit 1
    fi
    
    if [ ! -f "./he3build" ]; then
        log_error "He³ build system not found. Please run 'make' first."
        exit 1
    fi
    
    log_success "All prerequisites found"
}

# Create test directory
setup_test_environment() {
    log_info "Setting up test environment in $TEST_DIR"
    mkdir -p "$TEST_DIR"
    mkdir -p "$EXPECTED_OUTPUT_DIR"
}

# Clean up test environment
cleanup() {
    if [ "$CLEANUP" = true ]; then
        log_info "Cleaning up test environment..."
        rm -rf "$TEST_DIR"
    else
        log_info "Test files preserved in $TEST_DIR"
    fi
}

# Test a single .he3 file
test_he3_file() {
    local file_path="$1"
    local test_name="$2"
    local expected_exit_code="${3:-0}"
    local expected_output="${4:-}"
    
    if [ ! -f "$file_path" ]; then
        record_test "$test_name" "SKIP" "File not found: $file_path"
        return
    fi
    
    local base_name=$(basename "$file_path" .he3)
    local output_file="$TEST_DIR/${base_name}.bx"
    local helium_file="$TEST_DIR/${base_name}.helium3"
    
    # Test compilation
    if [ "$VERBOSE" = true ]; then
        log_info "Compiling $file_path..."
    fi
    
    if ./he3 "$file_path" -o "$output_file" 2>"$TEST_DIR/${base_name}_compile.log"; then
        if [ -f "$output_file" ]; then
            record_test "${test_name}_compile" "PASS" "Compilation successful"
            
            # Test VM execution if bytecode was generated
            if [ "$VERBOSE" = true ]; then
                log_info "Executing $output_file..."
            fi
            
            local vm_output
            local vm_exit_code
            vm_output=$(./he3vm "$output_file" 2>"$TEST_DIR/${base_name}_vm.log"; vm_exit_code=$?)
            
            if [ $vm_exit_code -eq $expected_exit_code ]; then
                record_test "${test_name}_execute" "PASS" "Execution successful (exit code: $vm_exit_code)"
                
                # Check expected output if provided
                if [ -n "$expected_output" ]; then
                    if echo "$vm_output" | grep -q "$expected_output"; then
                        record_test "${test_name}_output" "PASS" "Output contains expected text: $expected_output"
                    else
                        record_test "${test_name}_output" "FAIL" "Output does not contain expected text: $expected_output"
                        if [ "$VERBOSE" = true ]; then
                            echo "Actual output: $vm_output"
                        fi
                    fi
                fi
            else
                record_test "${test_name}_execute" "FAIL" "Execution failed (exit code: $vm_exit_code, expected: $expected_exit_code)"
                if [ "$VERBOSE" = true ]; then
                    echo "VM output: $vm_output"
                fi
            fi
        else
            record_test "${test_name}_compile" "FAIL" "No bytecode file generated"
        fi
    else
        record_test "${test_name}_compile" "FAIL" "Compilation failed"
        if [ "$VERBOSE" = true ]; then
            echo "Compilation error:"
            cat "$TEST_DIR/${base_name}_compile.log"
        fi
    fi
}

# Test a project using he3build
test_project() {
    local project_file="$1"
    local test_name="$2"
    local expected_exit_code="${3:-0}"
    
    if [ ! -f "$project_file" ]; then
        record_test "$test_name" "SKIP" "Project file not found: $project_file"
        return
    fi
    
    local project_dir=$(dirname "$project_file")
    local project_name=$(basename "$project_dir")
    local output_file="$TEST_DIR/${project_name}.helium3"
    
    if [ "$VERBOSE" = true ]; then
        log_info "Building project $project_file..."
    fi
    
    if ./he3build "$project_file" -o "$output_file" 2>"$TEST_DIR/${project_name}_build.log"; then
        if [ -f "$output_file" ]; then
            record_test "${test_name}_build" "PASS" "Project build successful"
            
            # Test VM execution
            if [ "$VERBOSE" = true ]; then
                log_info "Executing project $output_file..."
            fi
            
            local vm_output
            local vm_exit_code
            vm_output=$(./he3vm "$output_file" 2>"$TEST_DIR/${project_name}_vm.log"; vm_exit_code=$?)
            
            if [ $vm_exit_code -eq $expected_exit_code ]; then
                record_test "${test_name}_execute" "PASS" "Project execution successful (exit code: $vm_exit_code)"
            else
                record_test "${test_name}_execute" "FAIL" "Project execution failed (exit code: $vm_exit_code, expected: $expected_exit_code)"
                if [ "$VERBOSE" = true ]; then
                    echo "VM output: $vm_output"
                fi
            fi
        else
            record_test "${test_name}_build" "FAIL" "No helium3 file generated"
        fi
    else
        record_test "${test_name}_build" "FAIL" "Project build failed"
        if [ "$VERBOSE" = true ]; then
            echo "Build error:"
            cat "$TEST_DIR/${project_name}_build.log"
        fi
    fi
}

# Test control flow features
test_control_flow() {
    log_info "Testing control flow features..."
    
    # Create test files for control flow
    local test_dir="$TEST_DIR/control_flow"
    mkdir -p "$test_dir"
    
    # Test if/else statements
    cat > "$test_dir/if_test.he3" << 'EOF'
domain IfTest {
    function main(): System.Int64 {
        let x: System.Int64 = 10;
        let y: System.Int64 = 5;
        
        if (x > y) {
            return 1;  // Should return 1
        } else {
            return 0;
        }
    }
}
EOF
    
    test_he3_file "$test_dir/if_test.he3" "if_else" 0
    
    # Test while loops
    cat > "$test_dir/while_test.he3" << 'EOF'
domain WhileTest {
    function main(): System.Int64 {
        let counter: System.Int64 = 0;
        while (counter < 3) {
            counter = counter + 1;
        }
        return counter;  // Should return 3
    }
}
EOF
    
    test_he3_file "$test_dir/while_test.he3" "while_loop" 0
    
    # Test for loops
    cat > "$test_dir/for_test.he3" << 'EOF'
domain ForTest {
    function main(): System.Int64 {
        let sum: System.Int64 = 0;
        for (let i: System.Int64 = 1; i <= 3; i = i + 1) {
            sum = sum + i;
        }
        return sum;  // Should return 6 (1+2+3)
    }
}
EOF
    
    test_he3_file "$test_dir/for_test.he3" "for_loop" 0
    
    # Test arithmetic operations
    cat > "$test_dir/arithmetic_test.he3" << 'EOF'
domain ArithmeticTest {
    function main(): System.Int64 {
        let a: System.Int64 = 10;
        let b: System.Int64 = 3;
        
        let sum: System.Int64 = a + b;      // 13
        let diff: System.Int64 = a - b;     // 7
        let prod: System.Int64 = a * b;     // 30
        let quot: System.Int64 = a / b;     // 3
        let mod: System.Int64 = a % b;      // 1
        let neg: System.Int64 = -a;         // -10
        
        return sum + diff + prod + quot + mod + neg;  // 13+7+30+3+1-10 = 44
    }
}
EOF
    
    test_he3_file "$test_dir/arithmetic_test.he3" "arithmetic" 0
    
    # Test comparison operations
    cat > "$test_dir/comparison_test.he3" << 'EOF'
domain ComparisonTest {
    function main(): System.Int64 {
        let x: System.Int64 = 10;
        let y: System.Int64 = 5;
        
        let eq: System.Boolean = (x == y);     // false
        let ne: System.Boolean = (x != y);     // true
        let lt: System.Boolean = (x < y);      // false
        let le: System.Boolean = (x <= y);     // false
        let gt: System.Boolean = (x > y);      // true
        let ge: System.Boolean = (x >= y);     // true
        
        // Convert boolean to int64 for return
        let result: System.Int64 = 0;
        if (ne && gt && ge) {
            result = 1;  // Should return 1
        }
        return result;
    }
}
EOF
    
    test_he3_file "$test_dir/comparison_test.he3" "comparison" 0
    
    # Test logical operations
    cat > "$test_dir/logical_test.he3" << 'EOF'
domain LogicalTest {
    function main(): System.Int64 {
        let p: System.Boolean = true;
        let q: System.Boolean = false;
        
        let and_result: System.Boolean = p && q;     // false
        let or_result: System.Boolean = p || q;      // true
        let not_result: System.Boolean = !p;         // false
        
        // Convert boolean to int64 for return
        let result: System.Int64 = 0;
        if (or_result && !and_result && !not_result) {
            result = 1;  // Should return 1
        }
        return result;
    }
}
EOF
    
    test_he3_file "$test_dir/logical_test.he3" "logical" 0
}

# Test Sys.print functionality
test_sys_print() {
    log_info "Testing Sys.print functionality..."
    
    local test_dir="$TEST_DIR/sys_print"
    mkdir -p "$test_dir"
    
    # Test Sys.print
    cat > "$test_dir/sys_print_test.he3" << 'EOF'
domain SysPrintTest {
    function main(): System.Int64 {
        Sys.print("Hello");
        Sys.print(" ");
        Sys.print("World");
        return 0;
    }
}
EOF
    
    test_he3_file "$test_dir/sys_print_test.he3" "sys_print" 0 "Hello World"
    
    # Test Sys.println
    cat > "$test_dir/sys_println_test.he3" << 'EOF'
domain SysPrintlnTest {
    function main(): System.Int64 {
        Sys.println("Hello World");
        return 0;
    }
}
EOF
    
    test_he3_file "$test_dir/sys_println_test.he3" "sys_println" 0 "Hello World"
}

# Run all tests
run_all_tests() {
    log_info "Starting He³ Regression Test Suite"
    echo "======================================"
    echo ""
    
    # Test existing examples
    log_info "Testing existing examples..."
    
    # Standalone examples
    test_he3_file "examples/standalone/01_minimal/src/main.he3" "minimal_example" 0
    test_he3_file "examples/standalone/02_arithmetic/src/main.he3" "arithmetic_example" 0
    test_he3_file "examples/standalone/03_print_demo/src/main.he3" "print_demo" 0 "Hello from He³!"
    
    # Test projects
    test_project "examples/projects/01_minimal_project/he3project.json" "minimal_project" 0
    
    # Test control flow features
    test_control_flow
    
    # Test Sys.print functionality
    test_sys_print
}

# Generate test report
generate_report() {
    echo ""
    echo "======================================"
    echo "He³ Regression Test Report"
    echo "======================================"
    echo ""
    echo "Test Summary:"
    echo "  Total Tests: $TOTAL_TESTS"
    echo "  Passed: $PASSED_TESTS"
    echo "  Failed: $FAILED_TESTS"
    echo "  Skipped: $SKIPPED_TESTS"
    echo ""
    
    if [ $FAILED_TESTS -gt 0 ]; then
        echo "Failed Tests:"
        echo "============="
        for result in "${TEST_RESULTS[@]}"; do
            IFS='|' read -r status test_name message <<< "$result"
            if [ "$status" = "FAIL" ]; then
                echo "  ✗ $test_name: $message"
            fi
        done
        echo ""
    fi
    
    if [ $SKIPPED_TESTS -gt 0 ]; then
        echo "Skipped Tests:"
        echo "=============="
        for result in "${TEST_RESULTS[@]}"; do
            IFS='|' read -r status test_name message <<< "$result"
            if [ "$status" = "SKIP" ]; then
                echo "  - $test_name: $message"
            fi
        done
        echo ""
    fi
    
    # Overall result
    if [ $FAILED_TESTS -eq 0 ]; then
        log_success "All tests passed! He³ is working correctly."
        exit 0
    else
        log_error "$FAILED_TESTS test(s) failed. Please check the output above."
        exit 1
    fi
}

# Main execution
main() {
    # Set up signal handlers for cleanup
    trap cleanup EXIT
    
    check_prerequisites
    setup_test_environment
    run_all_tests
    generate_report
}

# Run main function
main "$@"
