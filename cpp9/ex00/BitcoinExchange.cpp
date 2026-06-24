/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:27:20 by yzhang2           #+#    #+#             */
/*   Updated: 2026/06/01 18:58:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "BitcoinExchange.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

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

static bool isLeapYear(int year)
{
    if (year % 400 == 0)
        return true;
    if (year % 100 == 0)
        return false;
    return year % 4 == 0;
}

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

BitcoinExchange::BitcoinExchange(void)
{
    loadDatabase("data.csv");
}

BitcoinExchange::BitcoinExchange(BitcoinExchange const &other)
{
    _rates = other._rates;
}

BitcoinExchange &BitcoinExchange::operator=(BitcoinExchange const &other)
{
    if (this != &other)
        _rates = other._rates;
    return *this;
}

BitcoinExchange::~BitcoinExchange(void)
{
}

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
