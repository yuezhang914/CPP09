#include "RPN.hpp"

/*
包含库说明：
cstdlib：使用 EXIT_SUCCESS 和 EXIT_FAILURE 表示程序退出状态。
exception：使用 std::exception 捕获 RPN 计算中抛出的异常。
iostream：使用 std::cout 输出结果，使用 std::cerr 输出错误。
*/
#include <cstdlib>
#include <exception>
#include <iostream>

/*
函数用途：ex01 程序入口，检查参数并输出 RPN 表达式计算结果。
参数说明：argc 是命令行参数数量，由系统传入；argv 是命令行参数数组，由系统传入，其中 argv[1] 应该是一整条 RPN 表达式。
变量说明：rpn 是 RPN 计算器对象，用来调用 calculate。
实现逻辑：要求只传入一个表达式；调用 calculate 并输出结果；任何异常都输出 Error 并返回失败状态。
*/
int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw std::runtime_error("Error");
        RPN rpn;
        std::cout << rpn.calculate(argv[1]) << std::endl;
    }
    catch (std::exception const &error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
