#include "RPN.hpp"

/*
包含库说明：
sstream：使用 std::istringstream 按空格拆分表达式中的 token。
stdexcept：使用 std::runtime_error 在表达式非法、数字不足或除以 0 时抛出 Error。
*/
#include <sstream>
#include <stdexcept>

/*
函数用途：判断一个 token 是否是题目允许的一位数字。
参数说明：token 是从表达式字符串中按空格拆出的片段，来源于 calculate 中的输入流。
变量说明：本函数没有局部变量。
实现逻辑：只有长度为 1 且字符在 '0' 到 '9' 之间时才返回 true。
*/
static bool isNumberToken(std::string const &token)
{
    return token.size() == 1 && token[0] >= '0' && token[0] <= '9';
}

/*
函数用途：判断一个 token 是否是题目允许的运算符。
参数说明：token 是从表达式字符串中按空格拆出的片段，来源于 calculate 中的输入流。
变量说明：本函数没有局部变量。
实现逻辑：先要求 token 长度为 1，再检查它是不是 +、-、*、/ 中的一个。
*/
static bool isOperatorToken(std::string const &token)
{
    if (token.size() != 1)
        return false;
    return token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/';
}

/*
函数用途：默认构造函数，创建一个空的 RPN 计算器对象。
参数说明：本函数没有参数。
变量说明：本函数没有局部变量；_numbers 会由 std::stack 默认构造成空栈。
实现逻辑：不需要额外初始化，函数体为空。
*/
RPN::RPN(void)
{
}

/*
函数用途：拷贝构造函数，用另一个 RPN 对象初始化当前对象。
参数说明：other 是传入的已有对象引用，数据来源是调用拷贝构造时的实参。
变量说明：本函数没有局部变量；_numbers 是当前对象成员变量。
实现逻辑：复制 other._numbers，让新对象拥有同样的栈状态。
*/
RPN::RPN(RPN const &other)
{
    _numbers = other._numbers;
}

/*
函数用途：赋值运算符，把另一个 RPN 对象的栈状态赋给当前对象。
参数说明：other 是等号右边传入的对象引用。
变量说明：本函数没有局部变量；_numbers 是当前对象成员变量。
实现逻辑：先避免自己给自己赋值，再复制 other._numbers，最后返回当前对象引用。
*/
RPN &RPN::operator=(RPN const &other)
{
    if (this != &other)
        _numbers = other._numbers;
    return *this;
}

/*
函数用途：析构函数，负责 RPN 对象生命周期结束时的清理入口。
参数说明：本函数没有参数。
变量说明：本函数没有局部变量。
实现逻辑：类中没有手动申请的资源，stack 会自动释放，所以函数体为空。
*/
RPN::~RPN(void)
{
}

/*
函数用途：对栈顶两个数字执行一个运算符，并把结果压回栈中。
参数说明：op 是 calculate 传入的运算符字符，来源于当前读到的运算符 token。
变量说明：right 是右操作数，来自栈顶；left 是左操作数，来自弹出 right 后的新栈顶。
实现逻辑：先确认栈里至少有两个数字，再按 RPN 顺序取出 left 和 right；根据 op 执行加减乘除，除法额外检查 right 不能为 0。
*/
void RPN::applyOperator(char op)
{
    long right;
    long left;

    if (_numbers.size() < 2)
        throw std::runtime_error("Error");
    right = _numbers.top();
    _numbers.pop();
    left = _numbers.top();
    _numbers.pop();
    if (op == '+')
        _numbers.push(left + right);
    else if (op == '-')
        _numbers.push(left - right);
    else if (op == '*')
        _numbers.push(left * right);
    else
    {
        if (right == 0)
            throw std::runtime_error("Error");
        _numbers.push(left / right);
    }
}

/*
函数用途：计算一整条逆波兰表达式并返回结果。
参数说明：expression 是 main 传入的表达式字符串，来源于命令行 argv[1]。
变量说明：stream 是用来按空格读取 token 的字符串流；token 保存当前读到的数字或运算符。
实现逻辑：先清空旧栈；逐个读取 token，数字入栈，运算符调用 applyOperator；遇到非法 token 抛错；最后要求栈中只剩一个结果。
*/
long RPN::calculate(std::string const &expression)
{
    std::istringstream stream;
    std::string token;

    while (_numbers.empty() == false)
        _numbers.pop();
    stream.str(expression);
    while (stream >> token)
    {
        if (isNumberToken(token) == true)
            _numbers.push(token[0] - '0');
        else if (isOperatorToken(token) == true)
            applyOperator(token[0]);
        else
            throw std::runtime_error("Error");
    }
    if (_numbers.size() != 1)
        throw std::runtime_error("Error");
    return _numbers.top();
}
