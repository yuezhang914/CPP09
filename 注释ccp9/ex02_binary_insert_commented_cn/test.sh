#!/bin/bash

if [ ! -f ./PmergeMe ]; then
    echo "Error: ./PmergeMe not found. Run make first in ex02 directory."
    exit 1
fi


pass_count=0
fail_count=0

run_valid()
{
    label="$1"
    expected="$2"
    shift 2

    output=$(./PmergeMe "$@" 2>&1)
    after=$(printf "%s\n" "$output" | sed -n 's/^After:[[:space:]]*//p')

    if [ "$after" = "$expected" ]; then
        echo "[PASS] $label"
        pass_count=$((pass_count + 1))
    else
        echo "[FAIL] $label"
        echo "command: ./PmergeMe $*"
        echo "expected After: $expected"
        echo "actual output:"
        printf "%s\n" "$output"
        fail_count=$((fail_count + 1))
    fi
}

run_error()
{
    label="$1"
    shift

    output=$(./PmergeMe "$@" 2>&1)

    if printf "%s\n" "$output" | grep -q "Error"; then
        echo "[PASS] $label"
        pass_count=$((pass_count + 1))
    else
        echo "[FAIL] $label"
        echo "command: ./PmergeMe $*"
        echo "expected: Error"
        echo "actual output:"
        printf "%s\n" "$output"
        fail_count=$((fail_count + 1))
    fi
}

run_no_arg_error()
{
    output=$(./PmergeMe 2>&1)

    if printf "%s\n" "$output" | grep -q "Error"; then
        echo "[PASS] no argument"
        pass_count=$((pass_count + 1))
    else
        echo "[FAIL] no argument"
        echo "expected: Error"
        echo "actual output:"
        printf "%s\n" "$output"
        fail_count=$((fail_count + 1))
    fi
}

run_big_reverse()
{
    args=$(seq 3000 -1 1)
    output=$(./PmergeMe $args 2>&1)
    after=$(printf "%s\n" "$output" | sed -n 's/^After:[[:space:]]*//p')
    expected=$(seq 1 3000 | tr '\n' ' ')
    expected=${expected% }

    if [ "$after" = "$expected" ]; then
        echo "[PASS] 3000 reversed integers"
        pass_count=$((pass_count + 1))
    else
        echo "[FAIL] 3000 reversed integers"
        echo "expected sorted sequence from 1 to 3000"
        echo "actual After line starts with:"
        printf "%s\n" "$after" | cut -c 1-200
        fail_count=$((fail_count + 1))
    fi
}

echo "===== Valid basic tests ====="
run_valid "subject example" "3 4 5 7 9" 3 5 9 7 4
run_valid "single value" "42" 42
run_valid "two values already sorted" "1 2" 1 2
run_valid "two values reversed" "1 2" 2 1
run_valid "odd count" "1 2 3 4 5" 5 1 4 2 3
run_valid "even count" "1 2 3 4 5 6" 6 2 5 1 4 3
run_valid "already sorted" "1 2 3 4 5 6 7 8 9" 1 2 3 4 5 6 7 8 9
run_valid "reverse sorted" "1 2 3 4 5 6 7 8 9" 9 8 7 6 5 4 3 2 1
run_valid "mixed small values" "1 2 3 4 5 7 8 9" 8 3 7 1 9 2 5 4
run_valid "large values" "1 42 999 1000 65535 2147483647" 2147483647 42 1000 999 1 65535

echo
echo "===== Duplicate tests ====="
run_valid "duplicate pair high case" "1 2 9 9" 9 1 9 2
run_valid "many duplicates" "1 1 2 2 3 3 5 5" 5 1 3 3 2 5 1 2
run_valid "all same values" "7 7 7 7 7" 7 7 7 7 7

echo
echo "===== Jacobsthal / insertion order stress tests ====="
run_valid "11 values mixed" "1 2 3 4 5 6 7 8 9 10 11" 11 3 8 1 10 2 7 4 9 5 6
run_valid "21 values mixed" "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21" 21 1 20 2 19 3 18 4 17 5 16 6 15 7 14 8 13 9 12 10 11
run_valid "stray with large middle" "1 2 3 4 5 6 7 8 100" 100 1 8 2 7 3 6 4 5

echo
echo "===== Error tests ====="
run_no_arg_error
run_error "negative number" -1 2
run_error "zero is not positive" 0 1
run_error "decimal number" 1.5 2
run_error "alphabetic token" 1 abc 3
run_error "mixed alnum token" 12a 3
run_error "plus sign token" +3 4
run_error "minus sign with digits" -3 4
run_error "empty string argument" ""
run_error "space string as one argument" "1 2 3"
run_error "int overflow" 2147483648 1
run_error "very large number" 999999999999999999999 1

echo
echo "===== Large scale test ====="
run_big_reverse

echo
echo "===== Summary ====="
echo "PASS: $pass_count"
echo "FAIL: $fail_count"

if [ "$fail_count" -ne 0 ]; then
    exit 1
fi
exit 0