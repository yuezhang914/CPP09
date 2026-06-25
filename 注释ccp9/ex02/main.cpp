#include "PmergeMe.hpp"

#include <climits>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool isPositiveInteger(char const *text)
{
    long value;
    char *end;
    int i;

    if (text == NULL || text[0] == '\0')
        return false;
    i = 0;
    while (text[i] != '\0')
    {
        if (isDigit(text[i]) == false)
            return false;
        i++;
    }
    value = std::strtol(text, &end, 10);
    if (*end != '\0')
        return false;
    return value > 0 && value <= INT_MAX;
}

static void validateInput(char **values, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        if (isPositiveInteger(values[i]) == false)
            throw std::runtime_error("Error");
        i++;
    }
}

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw std::runtime_error("Error");
        validateInput(argv + 1, argc - 1);
        PmergeMe sorter;
        sorter.sort(argv + 1, argc - 1);
    }
    catch (std::exception const &error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
