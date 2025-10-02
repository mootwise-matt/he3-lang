# He³ Test Suite

This directory contains a comprehensive test suite for the He³ compiler-VM pipeline.

## Test Structure

```
tests/
├── run_tests.sh              # Main test suite
├── run_all_tests.sh          # Orchestrates all test suites
├── regression/               # Regression tests
│   └── regression_tests.sh
├── examples/                 # Example program tests
│   └── example_tests.sh
├── stress/                   # Stress and performance tests
│   └── stress_tests.sh
└── unit/                     # Unit test files (auto-generated)
```

## Running Tests

### Quick Start
```bash
# Run all test suites
make test-all

# Run individual test suites
make test-suite      # Main test suite
make test-regression # Regression tests
make test-examples   # Example tests
make test-stress     # Stress tests
```

### Manual Execution
```bash
# Run main test suite
./tests/run_tests.sh

# Run regression tests
./tests/regression/regression_tests.sh

# Run example tests
./tests/examples/example_tests.sh

# Run stress tests
./tests/stress/stress_tests.sh

# Run all test suites
./tests/run_all_tests.sh
```

## Test Categories

### 1. Main Test Suite (`run_tests.sh`)
- **Build Tests**: Compiler and VM compilation
- **Unit Tests**: Lexer, parser, compilation, VM execution
- **Feature Tests**: Arithmetic operations, variable assignments, expressions
- **Error Handling**: Syntax error detection
- **Format Tests**: Bytecode and Helium module format validation
- **Integration Tests**: Example program compatibility
- **Performance Tests**: Compilation speed benchmarks

### 2. Regression Tests (`regression/regression_tests.sh`)
- **Assignment Statements**: Ensures `x = 5` works correctly
- **Binary Expressions**: Ensures `x + y` generates correct bytecode
- **Symbol Table Lookup**: Ensures first variable (index 0) is found
- **Lexer Tokens**: Ensures `=` is tokenized as `TK_ASSIGN`
- **Parser AST**: Ensures assignment creates `AST_ASSIGN` nodes
- **Bytecode Generation**: Ensures correct instruction sequences
- **VM Execution**: Ensures correct runtime results

### 3. Example Tests (`examples/example_tests.sh`)
- **Basic Arithmetic**: `10 + 5 = 15`
- **Variable Assignments**: `x = 100; y = x; z = y + 50`
- **Multiple Operations**: Complex expressions with multiple variables
- **Simple Returns**: Direct value returns
- **Variable Reuse**: Same variable used multiple times
- **Complex Expressions**: Nested operations
- **Different Values**: Various numeric values
- **Zero Handling**: Operations with zero
- **Large Numbers**: Stress testing with large values

### 4. Stress Tests (`stress/stress_tests.sh`)
- **Many Variables**: Tests with 50+ variables
- **Deep Expressions**: Nested operations
- **Compilation Speed**: Large file compilation timing
- **Memory Usage**: Large symbol table handling
- **Error Recovery**: Malformed input handling
- **Concurrent Compilation**: Multiple files simultaneously
- **Bytecode Integrity**: File consistency and repeatability

## Test Results

Each test suite provides:
- **Pass/Fail Status**: Clear indication of test results
- **Execution Time**: Performance metrics
- **Error Messages**: Detailed failure information
- **Summary Statistics**: Total tests, passed, failed counts

## Adding New Tests

### Unit Tests
Add new test cases to `run_tests.sh`:
```bash
test_new_feature() {
    print_test "New feature test"
    # Test implementation
    if condition; then
        print_pass "New feature test"
    else
        print_fail "New feature test" "Error message"
    fi
}
```

### Regression Tests
Add to `regression/regression_tests.sh`:
```bash
test_new_regression() {
    print_test "New regression test"
    # Test implementation
}
```

### Example Tests
Add to `examples/example_tests.sh`:
```bash
test_example "path/to/example.he3" "expected_result"
```

### Stress Tests
Add to `stress/stress_tests.sh`:
```bash
test_new_stress() {
    print_test "New stress test"
    # Stress test implementation
}
```

## Continuous Integration

The test suite is designed to be CI-friendly:
- **Exit Codes**: Proper exit codes for CI systems
- **Clean Output**: Structured output for parsing
- **Isolated Tests**: Each test is independent
- **Cleanup**: Automatic cleanup of test files

## Troubleshooting

### Common Issues
1. **Permission Denied**: Ensure test scripts are executable
   ```bash
   chmod +x tests/*.sh tests/*/*.sh
   ```

2. **Build Failures**: Ensure compiler and VM are built
   ```bash
   make clean && make
   ```

3. **Test Failures**: Check individual test output for details
   ```bash
   ./tests/run_tests.sh  # Shows detailed output
   ```

### Debug Mode
Run individual test functions for debugging:
```bash
# Source the test script and run specific function
source tests/run_tests.sh
test_compiler_build
```

## Performance Benchmarks

The test suite includes performance benchmarks:
- **Compilation Speed**: < 5 seconds for standard tests
- **Memory Usage**: Handles 100+ variables
- **Concurrent Compilation**: Multiple files simultaneously
- **Bytecode Size**: Reasonable file sizes

## Contributing

When adding new features to He³:
1. Add corresponding tests to the appropriate test suite
2. Ensure all existing tests still pass
3. Add regression tests for bug fixes
4. Update this README if adding new test categories
