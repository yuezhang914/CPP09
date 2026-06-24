#include "BitcoinExchange.hpp"

/*
包含库说明：
cstdlib：使用 std::strtod 把字符串转换成 double。
fstream：使用 std::ifstream 读取 data.csv 和用户传入的输入文件。
iostream：使用 std::cout 输出结果，使用 std::cerr 输出单行输入错误。
stdexcept：使用 std::runtime_error 抛出可捕获的错误信息。
*/
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

/*
函数用途：判断一个字符是不是数字字符。
参数说明：c 是调用者传入的单个字符，通常来自日期字符串或数字字符串。
变量说明：本函数没有局部变量。
实现逻辑：判断字符是否位于 '0' 到 '9' 之间，是则返回 true，否则返回 false。
*/
static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

/*
函数用途：把只包含数字的字符串转换成 int。
参数说明：text 是调用者传入的数字字符串，来源通常是日期中的 year、month、day 子串。
变量说明：value 是累积计算出的整数；i 是遍历 text 的下标。
实现逻辑：从左到右读取每个数字字符，每次把旧值乘 10 再加上当前数字。
*/
static int toInt(std::string const &text)
{
    int value;
    std::size_t i;

    value = 0;
    i = 0;
    while (i < text.size())
    {
        value = value * 10 + text[i] - '0';
        i++;
    }
    return value;
}

/*
函数用途：去掉字符串开头和结尾的空白字符。
参数说明：text 是调用者传入的原始字符串，可能来自 data.csv 的字段，也可能来自用户输入文件的字段。
变量说明：start 是第一个非空白字符的位置；end 是最后一个非空白字符后面的位置。
实现逻辑：先从左往右跳过空格、tab、回车、换行等空白字符，再从右往左跳过空白字符，最后返回中间有效内容。
*/
static std::string trim(std::string const &text)
{
    std::size_t start;
    std::size_t end;

    start = 0;
    while (start < text.size() && (text[start] == ' ' || text[start] == '\t' || text[start] == '\r' || text[start] == '\n'))
        start++;
    end = text.size();
    while (end > start && (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r' || text[end - 1] == '\n'))
        end--;
    return text.substr(start, end - start);
}

/*
函数用途：判断某一年是否为闰年。
参数说明：year 是调用者传入的年份，来源通常是日期字符串前 4 位转换后的结果。
变量说明：本函数没有局部变量。
实现逻辑：按公历规则判断，能被 400 整除是闰年；能被 100 整除不是闰年；否则能被 4 整除是闰年。
*/
static bool isLeapYear(int year)
{
    if (year % 400 == 0)
        return true;
    if (year % 100 == 0)
        return false;
    return year % 4 == 0;
}

/*
函数用途：检查日期字符串是否符合 YYYY-MM-DD 格式且日期真实存在。
参数说明：date 是调用者传入的日期字符串，可能来自 data.csv，也可能来自用户输入文件。
变量说明：year、month、day 是从 date 切出的年月日；maxDay 是当前月份允许的最大天数；i 是遍历 date 的下标。
实现逻辑：先检查长度和 '-' 的位置，再检查其余位置都是数字，最后根据月份和闰年判断 day 是否合法。
*/
static bool isValidDate(std::string const &date)
{
    int year;
    int month;
    int day;
    int maxDay;
    std::size_t i;

    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;
    i = 0;
    while (i < date.size())
    {
        if (i != 4 && i != 7 && isDigit(date[i]) == false)
            return false;
        i++;
    }
    year = toInt(date.substr(0, 4));
    month = toInt(date.substr(5, 2));
    day = toInt(date.substr(8, 2));
    if (month < 1 || month > 12 || day < 1)
        return false;
    maxDay = 31;
    if (month == 2)
        maxDay = isLeapYear(year) == true ? 29 : 28;
    else if (month == 4 || month == 6 || month == 9 || month == 11)
        maxDay = 30;
    return day <= maxDay;
}

/*
函数用途：把字符串解析为 double，并检查它是不是允许的数字格式。
参数说明：text 是待解析的数字字符串；signAllowed 是调用者传入的开关，表示是否允许正负号；value 是输出参数，解析成功后保存转换结果。
变量说明：i 是遍历 text 的下标；hasDigit 记录是否至少出现过一个数字；hasPoint 记录是否已经出现过小数点；end 是 strtod 返回的解析停止位置。
实现逻辑：手动检查符号、数字和小数点格式，再用 std::strtod 转换，最后确认整个字符串都被成功解析。
*/
static bool parseNumber(std::string const &text, bool signAllowed, double &value)
{
    std::size_t i;
    bool hasDigit;
    bool hasPoint;
    char *end;

    if (text.empty() == true)
        return false;
    i = 0;
    if ((text[i] == '-' || text[i] == '+') && signAllowed == true)
        i++;
    hasDigit = false;
    hasPoint = false;
    // 每个字符都必须是：
    // 1. 数字
    // 或者
    // 2. 第一次出现的小数点
    // 否则直接 false
    while (i < text.size())
    {
        if (isDigit(text[i]) == true)
            hasDigit = true;
        else if (text[i] == '.' && hasPoint == false)
            hasPoint = true;
        else
            return false;
        i++;
    }
    if (hasDigit == false)
        return false;
    value = std::strtod(text.c_str(), &end);
    return *end == '\0';
}

/*
函数用途：解析 data.csv 的一行，并把日期和汇率存入 rates。
参数说明：line 是从 data.csv 读出的一行；rates 是调用者传入的 map 引用，也就是 BitcoinExchange::_rates。
变量说明：comma 是逗号位置；date 保存逗号前清理后的日期；number 保存逗号后清理后的汇率字符串；value 保存解析后的汇率。
实现逻辑：检查 data.csv 行中必须有且只有一个逗号，清理字段两边空白，验证日期和汇率，再插入 map；如果格式或重复日期不合法就抛出异常。
*/
static void readDatabaseLine(std::string const &line, std::map<std::string, double> &rates)
{
    std::string date;
    std::string number;
    double value;
    std::size_t comma;

    comma = line.find(',');
    if (comma == std::string::npos || line.find(',', comma + 1) != std::string::npos)
        throw std::runtime_error("Error: invalid data file.");
    date = trim(line.substr(0, comma));
    number = trim(line.substr(comma + 1));
    if (isValidDate(date) == false || parseNumber(number, false, value) == false)
        throw std::runtime_error("Error: invalid data file.");
    if (rates.insert(std::make_pair(date, value)).second == false)
        throw std::runtime_error("Error: invalid data file.");
}

/*
函数用途：读取汇率数据库文件并填充 _rates。
参数说明：file 是调用者传入的文件名，本项目中默认来自构造函数的 "data.csv"。
变量说明：input 是文件输入流；line 保存当前读取到的一行。
实现逻辑：打开 data.csv
检查文件能不能打开
读取第一行表头
检查表头 trim 后是不是 date,exchange_rate
逐行读取数据库内容
跳过空行
调用 readDatabaseLine 解析每一行
把结果存进 _rates
最后检查 _rates 不能是空的
*/
void BitcoinExchange::loadDatabase(std::string const &file)
{
    std::ifstream input;
    std::string line;

    input.open(file.c_str());
    if (input.is_open() == false)
        throw std::runtime_error("Error: could not open data file.");
    if (!std::getline(input, line) || trim(line) != "date,exchange_rate")
        throw std::runtime_error("Error: invalid data file.");
    while (std::getline(input, line))
    {
        if (trim(line).empty() == true)
            continue;
        readDatabaseLine(line, _rates);
    }
    if (_rates.empty() == true)
        throw std::runtime_error("Error: empty data file.");
}

/*
函数用途：默认构造函数，创建 BitcoinExchange 对象并加载题目给定的 data.csv。
参数说明：本函数没有参数。
变量说明：本函数没有局部变量；_rates 是对象成员变量，会被 loadDatabase 填充。
实现逻辑：调用 loadDatabase("data.csv")，构造对象时就准备好可查询的汇率表。
*/
BitcoinExchange::BitcoinExchange(void)
{
    loadDatabase("data.csv");
}

/*
函数用途：拷贝构造函数，用另一个 BitcoinExchange 初始化当前对象。
参数说明：other 是传入的已有对象引用，数据来源是调用拷贝构造时的实参。
变量说明：本函数没有局部变量；_rates 是当前对象成员变量。
实现逻辑：把 other._rates 复制给当前对象，保证新对象拥有同样的汇率表。
*/
BitcoinExchange::BitcoinExchange(BitcoinExchange const &other)
{
    _rates = other._rates;
}

/*
函数用途：赋值运算符，让一个已存在的 BitcoinExchange 对象复制另一个对象的数据。
参数说明：other 是等号右边传入的对象引用。
变量说明：本函数没有局部变量；_rates 是当前对象成员变量。
实现逻辑：先避免自己给自己赋值，再复制 other._rates，最后返回当前对象引用以支持连续赋值。
*/
BitcoinExchange &BitcoinExchange::operator=(BitcoinExchange const &other)
{
    if (this != &other)
        _rates = other._rates;
    return *this;
}

/*
函数用途：析构函数，负责对象生命周期结束时的清理入口。
参数说明：本函数没有参数。
变量说明：本函数没有局部变量。
实现逻辑：类中没有手动 new 出来的资源，map 会自动释放，所以函数体为空。
*/
BitcoinExchange::~BitcoinExchange(void)
{
}

/*
函数用途：解析用户输入文件中的一行“date | value”。
参数说明：line 是从用户输入文件读出的一行；date 是输出参数，保存解析出的日期；value 是输出参数，保存解析出的比特币数量。
变量说明：pipe 是竖线分隔符的位置；number 保存竖线后面清理空白后的数量字符串。
实现逻辑：先确认本行有且只有一个竖线分隔符，再把竖线左右两边分别 trim；随后验证日期和数字格式，再检查 value 必须在 0 到 1000 之间；不合法时抛出题目要求的错误。
*/
static void parseInputLine(std::string const &line, std::string &date, double &value)
{
    std::string number;
    std::size_t pipe;

    pipe = line.find('|');
    if (pipe == std::string::npos || line.find('|', pipe + 1) != std::string::npos)
        throw std::runtime_error("Error: bad input => " + line);
    date = trim(line.substr(0, pipe));
    number = trim(line.substr(pipe + 1));
    if (isValidDate(date) == false || parseNumber(number, true, value) == false)
        throw std::runtime_error("Error: bad input => " + line);
    if (value < 0)
        throw std::runtime_error("Error: not a positive number.");
    if (value > 1000)
        throw std::runtime_error("Error: too large a number.");
}

/*
函数用途：根据输入日期找到可使用的汇率。
参数说明：date 是调用者传入的日期，来源于用户输入文件解析结果。
变量说明：it 是 map 迭代器，用来定位第一个不小于 date 的数据库日期。
实现逻辑：1. 先找第一个 >= date 的位置。
2. 如果正好等于 date，直接用。
3. 如果不是精确匹配，又已经在 begin，说明输入日期早于数据库第一天，不能往前退，报错。
4. 否则往前退一步，使用更早的最近日期。
*/
double BitcoinExchange::getRate(std::string const &date) const
{
    std::map<std::string, double>::const_iterator it;

    it = _rates.lower_bound(date);
    if (it != _rates.end() && it->first == date)
        return it->second;
    if (it == _rates.begin())
        throw std::runtime_error("Error: bad input => " + date);
    it--;
    return it->second;
}

/*
函数用途：读取用户传入的输入文件，并逐行输出比特币数量乘以对应汇率后的结果。
参数说明：file 是 main 传进来的用户输入文件名，来源于命令行 argv[1]。
变量说明：input 是文件输入流；line 保存当前行；date 保存解析出的日期；value 保存解析出的比特币数量。
实现逻辑：打开输入文件并检查表头清理空白后必须是 date | value，然后逐行解析；每行独立 try/catch，合法行输出计算结果，非法行输出错误并继续处理下一行。
*/
void BitcoinExchange::convert(std::string const &file) const
{
    std::ifstream input;
    std::string line;
    std::string date;
    double value;

    input.open(file.c_str());
    if (input.is_open() == false)
        throw std::runtime_error("Error: could not open file.");
    if (!std::getline(input, line) || trim(line) != "date | value")
        throw std::runtime_error("Error: bad input file.");
    while (std::getline(input, line))
    {
        if (trim(line).empty() == true)
            continue;
        try
        {
            parseInputLine(line, date, value);
            std::cout << date << " => " << value << " = " << value * getRate(date) << std::endl;
        }
        catch (std::exception const &error)
        {
            std::cerr << error.what() << std::endl;
        }
    }
}
