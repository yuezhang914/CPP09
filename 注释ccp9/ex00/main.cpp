#include "BitcoinExchange.hpp"

/*
包含库说明：
cstdlib：使用 EXIT_SUCCESS 和 EXIT_FAILURE 表示程序退出状态。
exception：使用 std::exception 捕获构造、读文件和转换过程中抛出的异常。
iostream：使用 std::cerr 输出致命错误信息。
*/
#include <cstdlib>
#include <exception>
#include <iostream>

/*
函数用途：ex00 程序入口，检查参数并启动 BitcoinExchange 转换流程。
参数说明：argc 是命令行参数数量，由系统传入；argv 是命令行参数数组，由系统传入，其中 argv[1] 应该是输入文件名。
变量说明：exchange 是 BitcoinExchange 对象，会在构造时读取 data.csv。
实现逻辑：要求只传入一个输入文件；创建 exchange；调用 convert 处理文件；任何致命异常都输出到标准错误并返回失败状态。
*/
int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw std::runtime_error("Error: could not open file.");
        BitcoinExchange exchange;
        exchange.convert(argv[1]);
    }
    catch (std::exception const &error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
