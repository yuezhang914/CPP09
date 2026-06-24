#include "PmergeMe.hpp"

/*
包含库说明：
climits：使用 INT_MAX 判断输入数字是否超过 int 上限。
cstdlib：使用 EXIT_SUCCESS、EXIT_FAILURE 表示程序退出状态，并使用 std::strtol 做字符串到 long 的转换。
exception：使用 std::exception 捕获输入检查或排序流程中的异常。
iostream：使用 std::cerr 输出错误信息。
stdexcept：使用 std::runtime_error 抛出题目要求的 Error。
*/
#include <climits>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>

/*
函数用途：判断一个字符是不是数字字符。
参数说明：c 是调用者传入的单个字符，来源于命令行参数字符串中的某一位。
变量说明：本函数没有局部变量。
实现逻辑：判断字符是否位于 '0' 到 '9' 之间，是则返回 true，否则返回 false。
*/
static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

/*
函数用途：判断一个命令行参数是否是合法的正整数。
参数说明：text 是调用者传入的 C 字符串，来源于 argv 中的某个数字参数。
变量说明：value 保存 strtol 转换后的 long；end 指向解析结束的位置；i 是遍历 text 的下标。
实现逻辑：先排除空字符串，再确认每个字符都是数字；然后用 strtol 转换，并检查结果必须大于 0 且不超过 INT_MAX。
*/
static bool isPositiveInteger(char const *text)
{
    long value;
    char *end;
    int i;

    if (text == NULL || text[0] == '\0')
        return false;
    i = 0;
    while (text[i] != '\0')
    {
        if (isDigit(text[i]) == false)
            return false;
        i++;
    }
    value = std::strtol(text, &end, 10);
    if (*end != '\0')
        return false;
    return value > 0 && value <= INT_MAX;
}

/*
函数用途：检查所有命令行数字参数是否都是合法正整数。
参数说明：values 是 main 传入的字符串数组，来源于 argv + 1；count 是数字个数，来源于 argc - 1。
变量说明：i 是遍历 values 的下标。
实现逻辑：逐个调用 isPositiveInteger；只要有一个不合法，就抛出 Error 终止程序。
*/
static void validateInput(char **values, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        if (isPositiveInteger(values[i]) == false)
            throw std::runtime_error("Error");
        i++;
    }
}

/*
函数用途：ex02 程序入口，检查输入并启动 PmergeMe 排序。
参数说明：argc 是命令行参数数量，由系统传入；argv 是命令行参数数组，由系统传入，argv[1] 到 argv[argc - 1] 应该是正整数序列。
变量说明：sorter 是 PmergeMe 对象，用来执行两种容器的排序和计时。
实现逻辑：要求至少有一个数字；先验证所有参数；再调用 sorter.sort 输出 Before、After 和耗时；任何异常都输出 Error 并返回失败状态。
*/
int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw std::runtime_error("Error");
        validateInput(argv + 1, argc - 1);
        PmergeMe sorter;
        sorter.sort(argv + 1, argc - 1);
    }
    catch (std::exception const &error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
