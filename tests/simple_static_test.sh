#!/bin/bash

# Simple Static Methods Test
# Tests basic static method functionality

set -e

echo "🧪 Running Simple Static Methods Test"
echo "===================================="

# Test basic functionality
echo "Testing basic static method calls..."

# Test 1: Basic print
echo -n "Test 1: Basic Sys.print()... "
if ./he3build examples/standalone/03_print_demo/he3project.json > /dev/null 2>&1; then
    if ./he3vm build/03_print_demo.helium3 > /dev/null 2>&1; then
        echo "✅ PASS"
    else
        echo "❌ FAIL (execution failed)"
        exit 1
    fi
else
    echo "❌ FAIL (compilation failed)"
    exit 1
fi

# Test 2: Multiple static calls
echo -n "Test 2: Multiple static calls... "
if ./he3build examples/standalone/04_static_methods_test/he3project.json > /dev/null 2>&1; then
    if ./he3vm build/04_static_methods_test.helium3 > /dev/null 2>&1; then
        echo "✅ PASS"
    else
        echo "❌ FAIL (execution failed)"
        exit 1
    fi
else
    echo "❌ FAIL (compilation failed)"
    exit 1
fi

# Test 3: Different argument types
echo -n "Test 3: Different argument types... "
if ./he3build examples/standalone/05_static_args_test/he3project.json > /dev/null 2>&1; then
    if ./he3vm build/05_static_args_test.helium3 > /dev/null 2>&1; then
        echo "✅ PASS"
    else
        echo "❌ FAIL (execution failed)"
        exit 1
    fi
else
    echo "❌ FAIL (compilation failed)"
    exit 1
fi

echo ""
echo "🎉 All static method tests passed!"
echo "✅ Static method infrastructure is working correctly"
