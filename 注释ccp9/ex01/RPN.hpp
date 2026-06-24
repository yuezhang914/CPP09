#ifndef RPN_HPP
#define RPN_HPP

/*
包含库说明：
stack：使用 std::stack<long> 保存逆波兰表达式计算过程中的操作数。
string：使用 std::string 保存整条表达式和拆分出来的 token。
*/
#include <stack>
#include <string>

/*
类用途：RPN 负责计算逆波兰表达式，也就是数字在前、运算符在后的表达式。
成员变量：_numbers 是计算栈，保存还没有被运算符合并的数字，数据来源于 calculate 解析到的数字 token。
成员函数逻辑：calculate 负责读取表达式并驱动计算；applyOperator 只在类内部使用，负责从栈中取两个数字并执行一个运算符。
*/
class RPN
{
    private:
        std::stack<long> _numbers;

        void applyOperator(char op);

    public:
        RPN(void);
        RPN(RPN const &other);
        RPN &operator=(RPN const &other);
        ~RPN(void);

        long calculate(std::string const &expression);
};

#endif
