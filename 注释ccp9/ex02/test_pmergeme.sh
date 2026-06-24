#!/bin/bash

# 检查当前目录下是否存在 PmergeMe 可执行文件。
# 如果没有，说明还没有 make，或者脚本没有放在 ex02 目录下。
if [ ! -f ./PmergeMe ]; then
    echo "Error: ./PmergeMe not found. Run make first in ex02 directory."
    exit 1
fi

# 记录通过的测试数量。
pass_count=0

# 记录失败的测试数量。
fail_count=0

# 函数用途：运行一个“应该成功排序”的测试。
# 参数说明：
#   $1 是测试名称，方便看是哪一项。
#   $2 是期望的 After 行，也就是排序后的数字序列。
#   shift 2 后，剩下的参数就是传给 ./PmergeMe 的测试数字。
run_valid()
{
    label="$1"
    expected="$2"
    shift 2

    # 执行程序，把标准输出和标准错误都收集到 output。
    output=$(./PmergeMe "$@" 2>&1)

    # 从程序输出中提取 After: 后面的排序结果。
    after=$(printf "%s\n" "$output" | sed -n 's/^After:[[:space:]]*//p')

    # 比较实际排序结果和期望排序结果。
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

# 函数用途：运行一个“应该报 Error”的测试。
# 参数说明：
#   $1 是测试名称。
#   shift 后，剩下的参数就是传给 ./PmergeMe 的非法输入。
run_error()
{
    label="$1"
    shift

    # 执行程序，把标准输出和标准错误都收集到 output。
    output=$(./PmergeMe "$@" 2>&1)

    # 只要输出中包含 Error，就认为这个错误测试通过。
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

# 函数用途：专门测试没有任何数字参数的情况。
# 因为没有参数时不能简单用 run_error "$@" 表示，所以单独写一个函数。
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

# 函数用途：测试 3000 个逆序整数，确认程序能处理题目要求的大规模输入。
# 实现逻辑：
#   seq 3000 -1 1 生成 3000 到 1 的逆序输入。
#   程序排序后应该输出 1 到 3000 的升序序列。
run_big_reverse()
{
    # 生成逆序参数：3000 2999 2998 ... 1。
    args=$(seq 3000 -1 1)

    # 执行程序。这里故意不加引号，让 shell 把 args 按空格拆成很多参数。
    output=$(./PmergeMe $args 2>&1)

    # 提取 After 行。
    after=$(printf "%s\n" "$output" | sed -n 's/^After:[[:space:]]*//p')

    # 生成期望的升序结果：1 2 3 ... 3000。
    expected=$(seq 1 3000 | tr '\n' ' ')

    # 删除末尾多余空格。
    expected=${expected% }

    # 比较实际结果和期望结果。
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

# 题目示例：普通乱序输入。
run_valid "subject example" "3 4 5 7 9" 3 5 9 7 4

# 只有一个数字，应该直接保持不变。
run_valid "single value" "42" 42

# 两个数字本来有序。
run_valid "two values already sorted" "1 2" 1 2

# 两个数字逆序，测试最小交换情况。
run_valid "two values reversed" "1 2" 2 1

# 奇数个输入，测试落单元素 stray 的处理。
run_valid "odd count" "1 2 3 4 5" 5 1 4 2 3

# 偶数个输入，测试完整配对。
run_valid "even count" "1 2 3 4 5 6" 6 2 5 1 4 3

# 已经升序，确认不会破坏原本顺序。
run_valid "already sorted" "1 2 3 4 5 6 7 8 9" 1 2 3 4 5 6 7 8 9

# 完全逆序，测试最常见压力输入之一。
run_valid "reverse sorted" "1 2 3 4 5 6 7 8 9" 9 8 7 6 5 4 3 2 1

# 小规模混合乱序。
run_valid "mixed small values" "1 2 3 4 5 7 8 9" 8 3 7 1 9 2 5 4

# 包含大整数和 int 最大值。
run_valid "large values" "1 42 999 1000 65535 2147483647" 2147483647 42 1000 999 1 65535

echo
echo "===== Duplicate tests ====="

# 重复 high 情况，用来确认 takeVectorPair / takeListPair 删除已用 pair 的逻辑正确。
run_valid "duplicate pair high case" "1 2 9 9" 9 1 9 2

# 多个重复数字，题目说重复处理可自行决定；如果你的实现允许重复，这个应通过。
run_valid "many duplicates" "1 1 2 2 3 3 5 5" 5 1 3 3 2 5 1 2

# 全部数字相同。
run_valid "all same values" "7 7 7 7 7" 7 7 7 7 7

echo
echo "===== Jacobsthal / insertion order stress tests ====="

# 11 个数字，覆盖 Jacobsthal 分组边界 1、3、5、11。
run_valid "11 values mixed" "1 2 3 4 5 6 7 8 9 10 11" 11 3 8 1 10 2 7 4 9 5 6

# 21 个数字，覆盖更大的 Jacobsthal 分组边界。
run_valid "21 values mixed" "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21" 21 1 20 2 19 3 18 4 17 5 16 6 15 7 14 8 13 9 12 10 11

# 带落单元素且数值分布明显，方便检查 stray 插入。
run_valid "stray with large middle" "1 2 3 4 5 6 7 8 100" 100 1 8 2 7 3 6 4 5

echo
echo "===== Error tests ====="

# 没有参数，应该报 Error。
run_no_arg_error

# 负数不属于 positive integer。
run_error "negative number" -1 2

# 0 不是 positive integer。
run_error "zero is not positive" 0 1

# 小数不是整数。
run_error "decimal number" 1.5 2

# 字母不是数字。
run_error "alphabetic token" 1 abc 3

# 数字和字母混合，也应报错。
run_error "mixed alnum token" 12a 3

# 如果你的实现只接受纯数字，+3 应该报错。
run_error "plus sign token" +3 4

# -3 应该报错。
run_error "minus sign with digits" -3 4

# 空字符串参数应该报错。
run_error "empty string argument" ""

# 引号包住的一整串会被 shell 当成一个参数，不是多个数字参数，所以应报错。
run_error "space string as one argument" "1 2 3"

# 超过 int 最大值，应该报错。
run_error "int overflow" 2147483648 1

# 极大数字，应该报错。
run_error "very large number" 999999999999999999999 1

echo
echo "===== Large scale test ====="

# 题目要求至少能处理 3000 个不同整数。
run_big_reverse

echo
echo "===== Summary ====="

# 输出通过和失败数量。
echo "PASS: $pass_count"
echo "FAIL: $fail_count"

# 如果有失败，脚本返回 1，方便在终端或 CI 里看出测试失败。
if [ "$fail_count" -ne 0 ]; then
    exit 1
fi

# 全部通过则返回 0。
exit 0