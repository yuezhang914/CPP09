/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:28:02 by yzhang2           #+#    #+#             */
/*   Updated: 2026/05/19 17:28:03 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#include "RPN.hpp"

#include <sstream>
#include <stdexcept>

static bool isNumberToken(std::string const &token)
{
    return token.size() == 1 && token[0] >= '0' && token[0] <= '9';
}

static bool isOperatorToken(std::string const &token)
{
    if (token.size() != 1)
        return false;
    return token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/';
}

RPN::RPN(void)
{
}

RPN::RPN(RPN const &other)
{
    _numbers = other._numbers;
}

RPN &RPN::operator=(RPN const &other)
{
    if (this != &other)
        _numbers = other._numbers;
    return *this;
}

RPN::~RPN(void)
{
}

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
