/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:28:08 by yzhang2           #+#    #+#             */
/*   Updated: 2026/05/19 17:28:09 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string>

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
