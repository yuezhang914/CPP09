/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:27:33 by yzhang2           #+#    #+#             */
/*   Updated: 2026/06/01 18:56:19 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "BitcoinExchange.hpp"
#include <exception>
#include <cstdlib>
#include <iostream>

int main(int argc,char **argv)
{
    try
    {
     if(argc!=2)
            throw std::runtime_error("Error: can not open file.");
        BitcoinExchange exchange;
        exchange.convert(argv[1]);
    }
    catch(std::exception const &error)
    {
        std::cerr<<error.what()<<std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}