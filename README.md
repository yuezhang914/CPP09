# CPP Module 09 - STL Containers and Algorithms

This repository contains my solutions for **CPP Module 09** from the 42 C++ curriculum.

The goal of this module is to practice the **STL (Standard Template Library)** in C++98 by choosing the right container for each problem, validating input carefully, handling errors clearly, and implementing algorithms without relying on ready-made shortcuts.

## General Rules

- Language: C++
- Standard: C++98
- Compilation flags: `-Wall -Wextra -Werror`
- One directory per exercise
- One `Makefile` per exercise
- No external libraries
- STL containers are used according to the exercise requirements
- The implementations focus on both correctness and the intent of the subject

## Repository Structure

```text
cpp09/
├── ex00/
│   └── Bitcoin Exchange
├── ex01/
│   └── Reverse Polish Notation
└── ex02/
    └── PmergeMe
```

---

# ex00 - Bitcoin Exchange

## Main Learning Goal

The main purpose of `ex00` is to learn how to use an **ordered associative container** to store data and perform efficient lookups.

This exercise is mainly about:

- Reading and parsing files
- Loading a CSV database
- Storing key-value data
- Searching by key
- Finding the closest earlier date when an exact date does not exist
- Validating input format and values
- Separating fatal errors from line-level errors

## What the Program Does

The program reads a Bitcoin exchange rate database from `data.csv`.

It then receives an input file containing lines in this format:

```text
date | value
```

For each valid line, the program:

1. Parses the date.
2. Parses the value.
3. Checks that the date format is valid.
4. Checks that the value is valid and within the required range.
5. Looks for the corresponding exchange rate.
6. If the exact date is missing, uses the closest earlier available date.
7. Prints the converted result.

Example:

```text
2011-01-03 | 3
```

The program outputs the date, the original value, and the value converted using the correct Bitcoin exchange rate.

## Why `std::map` Was Chosen

I used:

```cpp
std::map<std::string, double>
```

The date is the key, and the exchange rate is the value.

This container fits the exercise because:

- It stores data as `date -> rate`.
- It keeps keys automatically sorted.
- It allows searching by date.
- It supports `lower_bound`, which is useful for finding the closest earlier date.

The date format is `YYYY-MM-DD`, so lexicographical string order matches chronological date order. This makes `std::map<std::string, double>` a natural choice for this exercise.

## What I Learned

From this exercise, I learned that container choice should follow the problem.

Here, the important operation is not just storing rates, but searching dates in sorted order. `std::map` solves that naturally.

I also learned to validate data carefully. A string can look like a date but still be invalid, for example an impossible month or day.

---

# ex01 - Reverse Polish Notation

## Main Learning Goal

The main purpose of `ex01` is to understand how a **stack-based structure** solves a problem that follows last-in, first-out logic.

This exercise is mainly about:

- Understanding Reverse Polish Notation
- Processing tokens from left to right
- Using a stack to store operands
- Applying operators in the correct order
- Detecting invalid expressions
- Handling division and operand order correctly

## What the Program Does

The program receives one expression as a command-line argument.

The expression is written in Reverse Polish Notation, also called postfix notation.

Example:

```text
8 9 * 9 -
```

The program reads each token from left to right:

1. If the token is a number, it pushes it onto the stack.
2. If the token is an operator, it pops the two most recent numbers.
3. It applies the operator.
4. It pushes the result back onto the stack.
5. At the end, the stack must contain exactly one value.

## Why `std::stack` Was Chosen

I used `std::stack` because RPN works exactly like a stack.

When an operator appears, it must use the two most recently stored numbers.

That is the stack rule:

```text
Last In, First Out
```

For example:

```text
8 9 *
```

The program pushes `8`, then pushes `9`. When it sees `*`, it pops `9` and `8`, then calculates `8 * 9`.

`std::stack` provides exactly the operations needed:

- `push` to add a number
- `top` to read the most recent number
- `pop` to remove it
- `size` to check whether the expression is valid

The exercise does not need random access, sorting, or key-value lookup. It only needs access to the most recent operands, so `std::stack` is the most natural container.

## What I Learned

From this exercise, I learned that a restricted container can make a problem easier.

`std::stack` does not allow random access, but that restriction matches the RPN logic perfectly.

I also learned that operand order matters. For subtraction and division, the first popped value is the right operand, and the second popped value is the left operand.

---

# ex02 - PmergeMe

## Main Learning Goal

The main purpose of `ex02` is to implement a non-trivial sorting algorithm using **two different STL containers** and compare their processing time.

This exercise is mainly about:

- Understanding merge-insert sort
- Implementing the Ford-Johnson algorithm idea
- Pairing elements into larger and smaller values
- Recursively sorting the main chain
- Inserting pending elements in a Jacobsthal-based order
- Comparing `std::vector` and `std::list`
- Measuring processing time
- Handling at least 3000 integers
- Staying close to the subject's intended algorithm, not only producing a sorted result

## What the Program Does

The program receives a sequence of positive integers as command-line arguments.

It must:

1. Validate the input.
2. Print the original sequence.
3. Sort the sequence with a merge-insert / Ford-Johnson approach.
4. Run the algorithm with `std::vector`.
5. Run the algorithm with `std::list`.
6. Print the sorted result.
7. Print the processing time for both containers.

Example:

```bash
./PmergeMe 3 5 9 7 4
```

Output:

```text
Before: 3 5 9 7 4
After:  3 4 5 7 9
Time to process a range of 5 elements with std::vector<int>: ... us
Time to process a range of 5 elements with std::list<int>: ... us
```

## Why `std::vector` and `std::list` Were Chosen

The subject requires two different containers.

I used:

```cpp
std::vector<int>
std::list<int>
```

They are useful for comparison because their internal behavior is very different.

`std::vector` stores elements in contiguous memory. It allows fast index-based access, which makes binary insertion easier.

`std::list` stores elements as linked nodes. It does not support index access, so it must be traversed with iterators. However, once an insertion position is found, inserting a node is straightforward.

This makes the comparison meaningful:

```text
vector: fast access, possible element shifting during insertion
list: slower traversal, easier node insertion once the position is found
```

## General Ford-Johnson Idea

The algorithm follows this general structure:

1. Pair the input values.
2. For each pair, identify the larger value and the smaller value.
3. Recursively sort the larger values to form the main chain.
4. Keep the smaller values as pending elements.
5. Insert the pending elements into the main chain.
6. Use a Jacobsthal-based order to reduce the worst-case number of insertion comparisons.

The key idea is not only to sort correctly, but also to control the insertion search range.

## Previous Implementation Logic

My previous version followed this simpler structure:

```text
Pair values into high and low
Store all high values separately
Recursively sort the high values
After sorting, search back through the pair table to find the matching low
Build the pending sequence
Insert pending values in Jacobsthal order
```

This version was correct as a sorting program.

It produced the right output, handled duplicate values, handled odd input counts, and passed the test cases.

However, it was not the closest version to the Ford-Johnson subject intent.

The main weakness was this step:

```text
After sorting high values, search back to find the matching low value.
```

That means the high value and low value were temporarily separated, and the implementation later had to recover the relationship.

This is easier to understand, but less faithful to the Ford-Johnson idea, where the relationship between paired values should be preserved through the recursive process.

## Revised Subject-Closest Implementation Logic

I revised the implementation to keep the pair relationship more explicit.

Each input value is wrapped in a `SortItem`:

```cpp
struct SortItem
{
    int value;
    std::size_t id;
};
```

The `value` is the actual number.

The `id` is a unique identifier that keeps track of the original item, even when duplicate numbers exist.

Pairs are stored as:

```cpp
struct ItemPair
{
    SortItem high;
    SortItem low;
};
```

Pending elements are stored as:

```cpp
struct PendingItem
{
    SortItem item;
    std::size_t highId;
    bool hasHigh;
};
```

The revised algorithm works like this:

```text
Wrap each input number as SortItem(value, id)
Pair SortItems into high and low
Recursively sort the high SortItems
Use high.id to keep the high-low relationship reliable
Build pending items with their highId boundary
Insert pending items in Jacobsthal order
Use highId to limit the search range before the matching high
Extract the final integer values for output
```

This is closer to the subject because the implementation no longer depends on plain integer values to recover pair relationships.

The unique `id` also makes duplicate values safe. For example, two values can both be `9`, but they still have different IDs, so their matching lows are not confused.

## Why I Changed the ex02 Implementation

I changed the implementation because the subject expects more than just a correct final sorted sequence.

The goal of `ex02` is to practice merge-insert sort, so the structure of the algorithm matters.

The earlier version was easier to read, but it separated `high` and `low` too much and then searched back by value.

The revised version is better because:

- It preserves the relationship between each `high` and its `low`.
- It avoids confusing duplicate values.
- It uses unique IDs to track actual items instead of relying only on integer values.
- It keeps the insertion boundary linked to the original paired high.
- It is closer to the Ford-Johnson / merge-insert sorting idea required by the subject.

In short:

```text
Previous version: sort high values, then recover low values.
Revised version: sort tracked items and keep the high-low binding reliable.
```

The revised version is still written in C++98, does not use `std::sort`, and keeps separate vector and list implementations.

## What I Learned

From this exercise, I learned that passing tests is not always enough.

For algorithmic subjects, the implementation should also match the intended algorithm as closely as possible.

I also learned that when values can be duplicated, using only the value itself to identify an element can be unsafe. Adding an ID makes the relationship between paired elements explicit and reliable.

This exercise also showed the practical difference between `std::vector` and `std::list`:

- `std::vector` makes index-based access and binary insertion easier.
- `std::list` requires iterator-based traversal and extra helper logic.

## ex02 Test Result

The subject-closest version was tested with a dedicated script covering:

- Basic valid inputs
- Single value input
- Odd and even input counts
- Already sorted input
- Reverse sorted input
- Duplicate values
- Jacobsthal insertion boundary cases
- Invalid input cases
- 3000 reversed integers

Result:

```text
PASS: 29
FAIL: 0
```

---

# How to Build and Run

Go into the exercise directory and run:

```bash
make
```

Then run the executable with valid input.

Examples:

```bash
./btc input.txt
./RPN "8 9 * 9 -"
./PmergeMe 3 5 9 7 4
```

Clean object files:

```bash
make clean
```

Remove object files and executable:

```bash
make fclean
```

Rebuild:

```bash
make re
```

---

# Final Summary

CPP Module 09 helped me understand how to choose STL containers based on the real needs of a problem.

Through this module, I practiced:

- `std::map` for ordered key-value lookup
- `std::stack` for last-in, first-out expression evaluation
- `std::vector` and `std::list` for implementing and comparing the same sorting algorithm
- File parsing and input validation
- Error handling
- Recursive algorithm design
- Merge-insert sorting
- Jacobsthal-based insertion order
- Writing code that is not only correct, but also close to the subject's expected idea

The most important lesson from this project is that STL containers are not interchangeable tools.

A good implementation starts by understanding the problem, then choosing the data structure that matches the operations the problem really needs.
