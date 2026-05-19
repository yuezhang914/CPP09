/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:27:33 by yzhang2           #+#    #+#             */
/*   Updated: 2026/05/19 17:27:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "BitcoinExchange.hpp"

#include <cstdlib>
#include <iostream>
#include <exception>

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
