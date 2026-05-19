/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:28:27 by yzhang2           #+#    #+#             */
/*   Updated: 2026/05/19 17:28:28 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#include "PmergeMe.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/time.h>

struct IntPair
{
    int high;
    int low;
};

static void insertVectorValue(std::vector<int> &chain, int value)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    right = chain.size();
    while (left < right)
    {
        middle = left + (right - left) / 2;
        if (chain[middle] < value)
            left = middle + 1;
        else
            right = middle;
    }
    chain.insert(chain.begin() + left, value);
}

static void insertVectorPending(std::vector<int> &chain, std::vector<int> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertVectorValue(chain, pending[0]);
    previous = 1;
    current = 3;
    while (previous < pending.size())
    {
        end = current;
        if (end > pending.size())
            end = pending.size();
        index = end;
        while (index > previous)
        {
            insertVectorValue(chain, pending[index - 1]);
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

static int takeVectorLow(std::vector<IntPair> &pairs, int high)
{
    std::size_t i;
    int low;

    i = 0;
    while (i < pairs.size())
    {
        if (pairs[i].high == high)
        {
            low = pairs[i].low;
            pairs.erase(pairs.begin() + i);
            return low;
        }
        i++;
    }
    return high;
}

static std::vector<int> fordJohnsonVector(std::vector<int> values)
{
    std::vector<IntPair> pairs;
    std::vector<int> highs;
    std::vector<int> pending;
    std::vector<int> chain;
    IntPair pair;
    bool hasStray;
    int stray;
    std::size_t i;

    if (values.size() < 2)
        return values;
    hasStray = false;
    i = 0;
    while (i + 1 < values.size())
    {
        if (values[i] < values[i + 1])
        {
            pair.high = values[i + 1];
            pair.low = values[i];
        }
        else
        {
            pair.high = values[i];
            pair.low = values[i + 1];
        }
        pairs.push_back(pair);
        highs.push_back(pair.high);
        i += 2;
    }
    if (i < values.size())
    {
        hasStray = true;
        stray = values[i];
    }
    chain = fordJohnsonVector(highs);
    i = 0;
    while (i < chain.size())
    {
        pending.push_back(takeVectorLow(pairs, chain[i]));
        i++;
    }
    if (hasStray == true)
        pending.push_back(stray);
    insertVectorPending(chain, pending);
    return chain;
}

static void insertListValue(std::list<int> &chain, int value)
{
    std::list<int>::iterator it;

    it = chain.begin();
    while (it != chain.end() && *it < value)
        it++;
    chain.insert(it, value);
}

static int getListValue(std::list<int> const &values, std::size_t index)
{
    std::list<int>::const_iterator it;
    std::size_t i;

    it = values.begin();
    i = 0;
    while (i < index && it != values.end())
    {
        it++;
        i++;
    }
    return *it;
}

static void insertListPending(std::list<int> &chain, std::list<int> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertListValue(chain, getListValue(pending, 0));
    previous = 1;
    current = 3;
    while (previous < pending.size())
    {
        end = current;
        if (end > pending.size())
            end = pending.size();
        index = end;
        while (index > previous)
        {
            insertListValue(chain, getListValue(pending, index - 1));
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

static int takeListLow(std::list<IntPair> &pairs, int high)
{
    std::list<IntPair>::iterator it;
    int low;

    it = pairs.begin();
    while (it != pairs.end())
    {
        if (it->high == high)
        {
            low = it->low;
            pairs.erase(it);
            return low;
        }
        it++;
    }
    return high;
}

static std::list<int> fordJohnsonList(std::list<int> values)
{
    std::list<IntPair> pairs;
    std::list<int> highs;
    std::list<int> pending;
    std::list<int> chain;
    std::list<int>::iterator it;
    IntPair pair;
    bool hasStray;
    int first;
    int second;
    int stray;

    if (values.size() < 2)
        return values;
    hasStray = false;
    it = values.begin();
    while (it != values.end())
    {
        first = *it;
        it++;
        if (it == values.end())
        {
            hasStray = true;
            stray = first;
        }
        else
        {
            second = *it;
            it++;
            if (first < second)
            {
                pair.high = second;
                pair.low = first;
            }
            else
            {
                pair.high = first;
                pair.low = second;
            }
            pairs.push_back(pair);
            highs.push_back(pair.high);
        }
    }
    chain = fordJohnsonList(highs);
    it = chain.begin();
    while (it != chain.end())
    {
        pending.push_back(takeListLow(pairs, *it));
        it++;
    }
    if (hasStray == true)
        pending.push_back(stray);
    insertListPending(chain, pending);
    return chain;
}

static double getTimeInMicroseconds(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return static_cast<double>(time.tv_sec) * 1000000.0 + static_cast<double>(time.tv_usec);
}

static void printValues(char **values, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        std::cout << values[i];
        if (i + 1 < count)
            std::cout << " ";
        i++;
    }
}

static void printVector(std::vector<int> const &values)
{
    std::size_t i;

    i = 0;
    while (i < values.size())
    {
        std::cout << values[i];
        if (i + 1 < values.size())
            std::cout << " ";
        i++;
    }
}

PmergeMe::PmergeMe(void)
{
    _vectorTime = 0;
    _listTime = 0;
}

PmergeMe::PmergeMe(PmergeMe const &other)
{
    _vector = other._vector;
    _list = other._list;
    _vectorTime = other._vectorTime;
    _listTime = other._listTime;
}

PmergeMe &PmergeMe::operator=(PmergeMe const &other)
{
    if (this != &other)
    {
        _vector = other._vector;
        _list = other._list;
        _vectorTime = other._vectorTime;
        _listTime = other._listTime;
    }
    return *this;
}

PmergeMe::~PmergeMe(void)
{
}

void PmergeMe::sortVector(char **values, int count)
{
    double start;
    int i;

    start = getTimeInMicroseconds();
    _vector.clear();
    i = 0;
    while (i < count)
    {
        _vector.push_back(std::atoi(values[i]));
        i++;
    }
    _vector = fordJohnsonVector(_vector);
    _vectorTime = getTimeInMicroseconds() - start;
}

void PmergeMe::sortList(char **values, int count)
{
    double start;
    int i;

    start = getTimeInMicroseconds();
    _list.clear();
    i = 0;
    while (i < count)
    {
        _list.push_back(std::atoi(values[i]));
        i++;
    }
    _list = fordJohnsonList(_list);
    _listTime = getTimeInMicroseconds() - start;
}

void PmergeMe::sort(char **values, int count)
{
    std::cout << "Before: ";
    printValues(values, count);
    std::cout << std::endl;
    sortVector(values, count);
    sortList(values, count);
    std::cout << "After:  ";
    printVector(_vector);
    std::cout << std::endl;
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time to process a range of " << count << " elements with std::vector<int>: " << _vectorTime << " us" << std::endl;
    std::cout << "Time to process a range of " << count << " elements with std::list<int>: " << _listTime << " us" << std::endl;
}
