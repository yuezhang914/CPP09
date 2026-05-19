# CPP Module 09 - STL

This repository contains my solutions for **CPP Module 09** from the 42 C++ curriculum.

The main goal of this module is to practice the use of the **STL (Standard Template Library)** in C++98.
Each exercise focuses on choosing an appropriate container for a specific problem, processing input safely, handling errors, and applying algorithmic thinking.

## General Rules Followed

- Compiled with `c++`
- Used `-Wall -Wextra -Werror`
- Compatible with `-std=c++98`
- No external libraries
- One Makefile per exercise
- STL containers are used according to the module rules
- Each exercise is organized in its own directory

---

## ex00 - Bitcoin Exchange

### Main Learning Goal

The main purpose of this exercise is to learn how to use an ordered associative container to store and search data efficiently.

This exercise focuses on:

- Reading and parsing files
- Loading data from a CSV database
- Storing key-value pairs
- Searching for the closest valid date
- Validating input format
- Handling recoverable and non-recoverable errors

### What This Exercise Does

The program `btc` receives an input file containing lines in the following format:

```text
date | value
```

It uses the provided `data.csv` file as a Bitcoin exchange rate database.

For each input line, the program:

1. Parses the date and value.
2. Checks whether the date format is valid.
3. Checks whether the value is a valid number between 0 and 1000.
4. Searches for the matching exchange rate.
5. If the exact date does not exist in the database, it uses the closest earlier date.
6. Prints the converted Bitcoin value.

### Key Implementation Idea

The exchange rates are stored in a `std::map<std::string, double>`.

The date is used as the key, and the exchange rate is used as the value.
Because the date format is `YYYY-MM-DD`, string ordering matches chronological ordering.

This makes `std::map` suitable for this exercise, especially when using `lower_bound` to find the closest available date.

### What I Learned

From this exercise, I learned how to use `std::map` for ordered data lookup, how to parse a CSV file, and how to separate program-level errors from line-level errors.

I also learned that validating input is not only about checking the visible format.
For example, a date may look correct but still be invalid, such as an impossible month or day.

---

## ex01 - Reverse Polish Notation

### Main Learning Goal

The main purpose of this exercise is to understand how a stack-based structure can solve expressions that depend on last-in, first-out logic.

This exercise focuses on:

- Understanding Reverse Polish Notation
- Splitting an expression into tokens
- Using a stack to store operands
- Processing operators in the correct order
- Detecting invalid expressions

### What This Exercise Does

The program `RPN` receives one mathematical expression as a command-line argument.

The expression is written in Reverse Polish Notation, also known as postfix notation.

For example:

```text
8 9 * 9 -
```

The program reads each token from left to right.

For each token:

1. If it is a number, it is pushed onto the stack.
2. If it is an operator, the program pops two values from the stack.
3. The operation is applied.
4. The result is pushed back onto the stack.

At the end, the stack must contain exactly one value, which is the final result.

### Key Implementation Idea

The program uses `std::stack<long>`.

A stack is the natural structure for RPN because the most recently read numbers are the first ones used when an operator appears.

Special care is needed for subtraction and division because operand order matters.
The first value popped is the right operand, and the second value popped is the left operand.

### What I Learned

From this exercise, I learned how `std::stack` represents a restricted access pattern and why this restriction is useful.

I also learned how to validate an expression while evaluating it, instead of only checking it before execution.

This exercise made clear that the choice of container should follow the logic of the problem.

---

## ex02 - PmergeMe

### Main Learning Goal

The main purpose of this exercise is to implement a non-trivial sorting algorithm using two different STL containers and compare their behavior.

This exercise focuses on:

- Understanding merge-insert sort
- Implementing the Ford-Johnson algorithm idea
- Working with two different containers
- Comparing `std::vector` and `std::list`
- Measuring execution time
- Handling large input sequences

### What This Exercise Does

The program `PmergeMe` receives a sequence of positive integers as command-line arguments.

It must:

1. Validate all arguments.
2. Store the original sequence.
3. Sort the sequence using the merge-insert sorting approach.
4. Perform the sorting process with two different containers.
5. Display the sequence before sorting.
6. Display the sorted sequence.
7. Display the processing time for each container.

The program is designed to handle at least 3000 different integers.

### Key Implementation Idea

The exercise is based on the Ford-Johnson algorithm, also known as merge-insert sort.

The general idea is:

1. Pair the input values.
2. For each pair, separate the larger value and the smaller value.
3. Recursively sort the larger values to form the main chain.
4. Insert the smaller pending values back into the main chain.
5. Use an insertion order based on the Jacobsthal sequence idea.

The exercise is implemented separately for `std::vector` and `std::list`.

This is important because the two containers behave differently:

- `std::vector` allows fast index-based access.
- `std::list` does not support index access and must be traversed with iterators.

### What I Learned

From this exercise, I learned that implementing the same algorithm with different containers can require different thinking.

With `std::vector`, random access makes binary insertion easier.
With `std::list`, iterator-based traversal becomes more important.

I also learned that measuring performance should include both data management and the sorting process, because the subject asks for the complete processing time.

This exercise showed that STL containers are not interchangeable tools.
Each one has strengths and weaknesses depending on the operations required by the algorithm.

---

## Final Summary

CPP Module 09 helped me understand how to select containers based on the needs of a problem.

Through these exercises, I practiced:

- Using `std::map` for ordered key-value lookup
- Using `std::stack` for last-in, first-out expression evaluation
- Using `std::vector` and `std::list` to compare container behavior
- Parsing and validating input carefully
- Handling errors clearly
- Implementing an algorithm instead of relying on a ready-made sort
- Thinking about data structure choice before writing code

The most important lesson from this module is that STL is not just a set of convenient containers.
It is a way to organize data according to the problem being solved.
