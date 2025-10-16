#!/bin/bash

# Simple test runner for wish shell
# Tests are numbered 1-22 in the tests/ directory

TESTS_DIR="tests"
WISH="./wish"
PASSED=0
FAILED=0
TOTAL=0

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================"
echo "Running WISH Shell Tests"
echo "========================================"
echo ""

# Check if wish executable exists
if [ ! -x "$WISH" ]; then
    echo -e "${RED}Error: wish executable not found or not executable${NC}"
    exit 1
fi

# Run tests 1-22
for i in {1..22}; do
    if [ ! -f "$TESTS_DIR/${i}.in" ]; then
        continue
    fi
    
    TOTAL=$((TOTAL + 1))
    
    # Read test description
    DESC=""
    if [ -f "$TESTS_DIR/${i}.desc" ]; then
        DESC=$(cat "$TESTS_DIR/${i}.desc")
    fi
    
    echo -e "${YELLOW}Test $i:${NC} $DESC"
    
    # Run the test
    ACTUAL_OUT=$(mktemp)
    ACTUAL_ERR=$(mktemp)
    
    # Check if there's a .run file (custom command)
    if [ -f "$TESTS_DIR/${i}.run" ]; then
        # Execute the command from .run file
        TEST_CMD=$(cat "$TESTS_DIR/${i}.run")
        # Replace INPUT_DIR placeholder if it exists
        TEST_CMD="${TEST_CMD//INPUT_DIR/$TESTS_DIR}"
        eval "$TEST_CMD" > "$ACTUAL_OUT" 2> "$ACTUAL_ERR"
        ACTUAL_RC=$?
    else
        # Default: Execute wish with the input file
        $WISH "$TESTS_DIR/${i}.in" > "$ACTUAL_OUT" 2> "$ACTUAL_ERR"
        ACTUAL_RC=$?
    fi
    
    # Compare outputs
    EXPECTED_OUT="$TESTS_DIR/${i}.out"
    EXPECTED_ERR="$TESTS_DIR/${i}.err"
    EXPECTED_RC="$TESTS_DIR/${i}.rc"
    
    OUT_MATCH=1
    ERR_MATCH=1
    RC_MATCH=1
    
    # Check stdout
    if [ -f "$EXPECTED_OUT" ]; then
        if ! diff -q "$EXPECTED_OUT" "$ACTUAL_OUT" > /dev/null 2>&1; then
            OUT_MATCH=0
        fi
    fi
    
    # Check stderr
    if [ -f "$EXPECTED_ERR" ]; then
        if ! diff -q "$EXPECTED_ERR" "$ACTUAL_ERR" > /dev/null 2>&1; then
            ERR_MATCH=0
        fi
    fi
    
    # Check return code
    if [ -f "$EXPECTED_RC" ]; then
        EXPECTED_RC_VAL=$(cat "$EXPECTED_RC")
        if [ "$ACTUAL_RC" != "$EXPECTED_RC_VAL" ]; then
            RC_MATCH=0
        fi
    fi
    
    # Report results
    if [ $OUT_MATCH -eq 1 ] && [ $ERR_MATCH -eq 1 ] && [ $RC_MATCH -eq 1 ]; then
        echo -e "${GREEN}✓ PASSED${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAILED${NC}"
        FAILED=$((FAILED + 1))
        
        if [ $OUT_MATCH -eq 0 ]; then
            echo "  - stdout mismatch"
            echo "  Expected:"
            cat "$EXPECTED_OUT" | head -5
            echo "  Actual:"
            cat "$ACTUAL_OUT" | head -5
        fi
        
        if [ $ERR_MATCH -eq 0 ]; then
            echo "  - stderr mismatch"
            echo "  Expected:"
            cat "$EXPECTED_ERR" | head -5
            echo "  Actual:"
            cat "$ACTUAL_ERR" | head -5
        fi
        
        if [ $RC_MATCH -eq 0 ]; then
            echo "  - return code mismatch (expected: $EXPECTED_RC_VAL, got: $ACTUAL_RC)"
        fi
    fi
    
    # Cleanup
    rm -f "$ACTUAL_OUT" "$ACTUAL_ERR"
    echo ""
done

# Summary
echo "========================================"
echo "Test Results Summary"
echo "========================================"
echo -e "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo "========================================"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! 🎉${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
