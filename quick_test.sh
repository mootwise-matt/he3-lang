#!/bin/bash

# He³ Quick Test Script
# Fast verification of core functionality during development
# Run from project root: ./quick_test.sh

set -e

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "He³ Quick Test"
echo "=============="
echo ""

# Test 1: Basic compilation and execution
echo "1. Testing basic compilation..."
if ./he3 examples/standalone/01_minimal/src/main.he3; then
    echo -e "${GREEN}✓${NC} Compilation successful"
else
    echo -e "${RED}✗${NC} Compilation failed"
    exit 1
fi

# Test 2: VM execution (need to build .helium3 first)
echo "2. Testing VM execution..."
if ./he3build examples/standalone/01_minimal/he3project.json; then
    if ./he3vm build/01_minimal.helium3; then
        echo -e "${GREEN}✓${NC} VM execution successful"
    else
        echo -e "${RED}✗${NC} VM execution failed"
        exit 1
    fi
else
    echo -e "${RED}✗${NC} Project build failed"
    exit 1
fi

# Test 3: Arithmetic operations
echo "3. Testing arithmetic operations..."
if ./he3build examples/standalone/02_arithmetic/he3project.json; then
    ./he3vm build/02_arithmetic.helium3 >/dev/null 2>&1
    result=$?
    if [ "$result" = "50" ]; then
        echo -e "${GREEN}✓${NC} Arithmetic operations working (42 + 8 = 50)"
    else
        echo -e "${RED}✗${NC} Arithmetic execution failed (expected 50, got $result)"
    fi
else
    echo -e "${RED}✗${NC} Arithmetic build failed"
fi

# Test 4: Sys.print functionality
echo "4. Testing Sys.print..."
if ./he3build examples/standalone/03_print_demo/he3project.json; then
    if ./he3vm build/03_print_demo.helium3; then
        echo -e "${GREEN}✓${NC} Sys.print working"
    else
        echo -e "${RED}✗${NC} Sys.print execution failed"
    fi
else
    echo -e "${RED}✗${NC} Sys.print build failed"
fi

# Test 5: Control flow (if/else)
echo "5. Testing control flow..."
mkdir -p /tmp/control_flow_test
cat > /tmp/control_flow_test/main.he3 << 'EOF'
domain ControlFlowTest {
    function main(): System.Int64 {
        let x: System.Int64 = 10;
        if (x > 5) {
            return 1;
        } else {
            return 0;
        }
    }
}
EOF

cat > /tmp/control_flow_test/he3project.json << 'EOF'
{
    "name": "control_flow_test",
    "version": "1.0.0",
    "type": "executable",
    "entryPoint": "main.he3",
    "sourceFiles": ["main.he3"],
    "dependencies": []
}
EOF

if (cd /tmp/control_flow_test && /Users/matthewwhite/Code/he3/he3build he3project.json); then
    if /Users/matthewwhite/Code/he3/he3vm /tmp/control_flow_test/build/output.helium3 >/dev/null 2>&1; then
        result=$?
        if [ "$result" = "1" ]; then
            echo -e "${GREEN}✓${NC} Control flow working (if x > 5 returns 1)"
        else
            echo -e "${RED}✗${NC} Control flow execution failed (expected 1, got $result)"
        fi
    else
        echo -e "${RED}✗${NC} Control flow execution failed"
    fi
else
    echo -e "${RED}✗${NC} Control flow build failed"
fi

# Test 6: While loops
echo "6. Testing while loops..."
mkdir -p /tmp/while_test
cat > /tmp/while_test/main.he3 << 'EOF'
domain WhileTest {
    function main(): System.Int64 {
        let counter: System.Int64 = 0;
        while (counter < 3) {
            counter = counter + 1;
        }
        return counter;
    }
}
EOF

cat > /tmp/while_test/he3project.json << 'EOF'
{
    "name": "while_test",
    "version": "1.0.0",
    "type": "executable",
    "entryPoint": "main.he3",
    "sourceFiles": ["main.he3"],
    "dependencies": []
}
EOF

if (cd /tmp/while_test && /Users/matthewwhite/Code/he3/he3build he3project.json); then
    if /Users/matthewwhite/Code/he3/he3vm /tmp/while_test/build/output.helium3 >/dev/null 2>&1; then
        result=$?
        if [ "$result" = "3" ]; then
            echo -e "${GREEN}✓${NC} While loops working (counter reaches 3)"
        else
            echo -e "${RED}✗${NC} While loop execution failed (expected 3, got $result)"
        fi
    else
        echo -e "${RED}✗${NC} While loop execution failed"
    fi
else
    echo -e "${RED}✗${NC} While loop build failed"
fi

# Test 7: For loops
echo "7. Testing for loops..."
mkdir -p /tmp/for_test
cat > /tmp/for_test/main.he3 << 'EOF'
domain ForTest {
    function main(): System.Int64 {
        let sum: System.Int64 = 0;
        for (let i: System.Int64 = 1; i <= 3; i = i + 1) {
            sum = sum + i;
        }
        return sum;
    }
}
EOF

cat > /tmp/for_test/he3project.json << 'EOF'
{
    "name": "for_test",
    "version": "1.0.0",
    "type": "executable",
    "entryPoint": "main.he3",
    "sourceFiles": ["main.he3"],
    "dependencies": []
}
EOF

if (cd /tmp/for_test && /Users/matthewwhite/Code/he3/he3build he3project.json); then
    if /Users/matthewwhite/Code/he3/he3vm /tmp/for_test/build/output.helium3 >/dev/null 2>&1; then
        result=$?
        if [ "$result" = "6" ]; then
            echo -e "${GREEN}✓${NC} For loops working (sum 1+2+3 = 6)"
        else
            echo -e "${RED}✗${NC} For loop execution failed (expected 6, got $result)"
        fi
    else
        echo -e "${RED}✗${NC} For loop execution failed"
    fi
else
    echo -e "${RED}✗${NC} For loop build failed"
fi

# Test 8: Project build
echo "8. Testing project build..."
if ./he3build examples/projects/01_minimal_project/he3project.json; then
    if ./he3vm build/01_minimal_project.helium3; then
        echo -e "${GREEN}✓${NC} Project build working"
    else
        echo -e "${RED}✗${NC} Project execution failed"
    fi
else
    echo -e "${RED}✗${NC} Project build failed"
fi

# Cleanup
rm -rf /tmp/control_flow_test
rm -rf /tmp/while_test
rm -rf /tmp/for_test

echo ""
echo -e "${GREEN}All quick tests completed!${NC}"
echo "Run './regression_test.sh' for comprehensive testing."
