#!/bin/bash

if [ ! -f ./RPN ]; then
    echo "Error: ./RPN not found. Run make first in ex01 directory."
    exit 1
fi

echo "===== Subject examples ====="
echo '$ ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"'
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
echo "expected: 42"
echo

echo '$ ./RPN "7 7 * 7 -"'
./RPN "7 7 * 7 -"
echo "expected: 42"
echo

echo '$ ./RPN "1 2 * 2 / 2 * 2 4 - +"'
./RPN "1 2 * 2 / 2 * 2 4 - +"
echo "expected: 0"
echo

echo '$ ./RPN "(1 + 1)"'
./RPN "(1 + 1)"
echo "expected: Error"
echo

echo "===== Basic valid operations ====="
echo '$ ./RPN "1 2 +"'
./RPN "1 2 +"
echo "expected: 3"
echo

echo '$ ./RPN "5 3 -"'
./RPN "5 3 -"
echo "expected: 2"
echo

echo '$ ./RPN "4 6 *"'
./RPN "4 6 *"
echo "expected: 24"
echo

echo '$ ./RPN "8 2 /"'
./RPN "8 2 /"
echo "expected: 4"
echo

echo "===== Operand order tests ====="
echo '$ ./RPN "3 8 -"'
./RPN "3 8 -"
echo "expected: -5"
echo

echo '$ ./RPN "8 3 -"'
./RPN "8 3 -"
echo "expected: 5"
echo

echo '$ ./RPN "2 8 /"'
./RPN "2 8 /"
echo "expected: 0 if using integer division"
echo

echo "===== Mixed operations ====="
echo '$ ./RPN "2 3 + 4 *"'
./RPN "2 3 + 4 *"
echo "expected: 20"
echo

echo '$ ./RPN "2 3 4 + *"'
./RPN "2 3 4 + *"
echo "expected: 14"
echo

echo '$ ./RPN "9 2 / 3 +"'
./RPN "9 2 / 3 +"
echo "expected: 7 if using integer division"
echo

echo '$ ./RPN "5 1 2 + 4 * + 3 -"'
./RPN "5 1 2 + 4 * + 3 -"
echo "expected: 14"
echo

echo "===== Zero and negative results ====="
echo '$ ./RPN "1 1 -"'
./RPN "1 1 -"
echo "expected: 0"
echo

echo '$ ./RPN "1 2 -"'
./RPN "1 2 -"
echo "expected: -1"
echo

echo '$ ./RPN "2 3 - 4 *"'
./RPN "2 3 - 4 *"
echo "expected: -4"
echo

echo "===== Large result tests ====="
echo '$ ./RPN "9 9 * 9 *"'
./RPN "9 9 * 9 *"
echo "expected: 729"
echo

echo '$ ./RPN "9 9 * 9 * 9 *"'
./RPN "9 9 * 9 * 9 *"
echo "expected: 6561"
echo

echo '$ ./RPN "8 8 * 8 * 8 *"'
./RPN "8 8 * 8 * 8 *"
echo "expected: 4096"
echo

echo "===== Division by zero tests ====="
echo '$ ./RPN "4 0 /"'
./RPN "4 0 /"
echo "expected: Error"
echo

echo '$ ./RPN "5 5 - 3 /"'
./RPN "5 5 - 3 /"
echo "expected: 0"
echo

echo '$ ./RPN "3 5 5 - /"'
./RPN "3 5 5 - /"
echo "expected: Error"
echo

echo "===== Not enough operands ====="
echo '$ ./RPN "+"'
./RPN "+"
echo "expected: Error"
echo

echo '$ ./RPN "1 +"'
./RPN "1 +"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 + +"'
./RPN "1 2 + +"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 * /"'
./RPN "1 2 * /"
echo "expected: Error"
echo

echo "===== Too many operands left ====="
echo '$ ./RPN "1 2"'
./RPN "1 2"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 3 +"'
./RPN "1 2 3 +"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 3"'
./RPN "1 2 3"
echo "expected: Error"
echo

echo "===== Invalid tokens ====="
echo '$ ./RPN "1.5 2 +"'
./RPN "1.5 2 +"
echo "expected: Error"
echo

echo '$ ./RPN "10 2 +"'
./RPN "10 2 +"
echo "expected: Error"
echo

echo '$ ./RPN "1 a +"'
./RPN "1 a +"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 %"'
./RPN "1 2 %"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 ++"'
./RPN "1 2 ++"
echo "expected: Error"
echo

echo '$ ./RPN "1 2 +x"'
./RPN "1 2 +x"
echo "expected: Error"
echo

echo "===== Empty and whitespace expressions ====="
echo '$ ./RPN ""'
./RPN ""
echo "expected: Error"
echo

echo '$ ./RPN " "'
./RPN " "
echo "expected: Error"
echo

echo '$ ./RPN "    "'
./RPN "    "
echo "expected: Error"
echo

echo "===== Whitespace tolerance ====="
echo '$ ./RPN "1    2    +"'
./RPN "1    2    +"
echo "expected: 3"
echo

echo '$ ./RPN "   1 2 +   "'
./RPN "   1 2 +   "
echo "expected: 3"
echo

echo "===== Argument count tests ====="
echo '$ ./RPN'
./RPN
echo "expected: Error"
echo

echo '$ ./RPN "1 2 +" "3 4 +"'
./RPN "1 2 +" "3 4 +"
echo "expected: Error"
echo

echo "===== Done ====="
