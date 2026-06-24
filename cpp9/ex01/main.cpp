/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:27:49 by yzhang2           #+#    #+#             */
/*   Updated: 2026/06/02 20:50:14 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "RPN.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

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
