/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:28:27 by yzhang2           #+#    #+#             */
/*   Updated: 2026/06/23 15:10:45 by yzhang2          ###   ########.fr       */
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

struct PendingValue
{
    int value;
    int high;
    bool hasHigh;
};

static std::size_t findVectorLimit(std::vector<int> const &chain, int high)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    right = chain.size();
    while (left < right)
    {
        middle = left + (right - left) / 2;
        if (chain[middle] < high)
            left = middle + 1;
        else
            right = middle;
    }
    return left;
}

static void insertVectorValue(std::vector<int> &chain, PendingValue const &item)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    if (item.hasHigh == true)
        right = findVectorLimit(chain, item.high);
    else
        right = chain.size();
    while (left < right)
    {
        middle = left + (right - left) / 2;
        if (chain[middle] < item.value)
            left = middle + 1;
        else
            right = middle;
    }
    chain.insert(chain.begin() + left, item.value);
}

static void insertVectorPending(std::vector<int> &chain, std::vector<PendingValue> const &pending)
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

static IntPair takeVectorPair(std::vector<IntPair> &pairs, int high)
{
    std::size_t i;
    IntPair pair;

    i = 0;
    while (i < pairs.size())
    {
        if (pairs[i].high == high)
        {
            pair = pairs[i];
            pairs.erase(pairs.begin() + i);
            return pair;
        }
        i++;
    }
    pair.high = high;
    pair.low = high;
    return pair;
}

static std::vector<int> fordJohnsonVector(std::vector<int> values)
{
    std::vector<IntPair> pairs;
    std::vector<int> highs;
    std::vector<PendingValue> pending;
    std::vector<int> chain;
    IntPair pair;
    PendingValue item;
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
        pair = takeVectorPair(pairs, chain[i]);
        item.value = pair.low;
        item.high = pair.high;
        item.hasHigh = true;
        pending.push_back(item);
        i++;
    }
    if (hasStray == true)
    {
        item.value = stray;
        item.high = 0;
        item.hasHigh = false;
        pending.push_back(item);
    }
    insertVectorPending(chain, pending);
    return chain;
}

static void insertListValue(std::list<int> &chain, PendingValue const &item)
{
    std::list<int>::iterator limit;
    std::list<int>::iterator it;

    limit = chain.end();
    if (item.hasHigh == true)
    {
        limit = chain.begin();
        while (limit != chain.end() && *limit < item.high)
            limit++;
    }
    it = chain.begin();
    while (it != limit && *it < item.value)
        it++;
    chain.insert(it, item.value);
}

static PendingValue getListPending(std::list<PendingValue> const &values, std::size_t index)
{
    std::list<PendingValue>::const_iterator it;
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

static void insertListPending(std::list<int> &chain, std::list<PendingValue> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertListValue(chain, getListPending(pending, 0));
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
            insertListValue(chain, getListPending(pending, index - 1));
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

static IntPair takeListPair(std::list<IntPair> &pairs, int high)
{
    std::list<IntPair>::iterator it;
    IntPair pair;

    it = pairs.begin();
    while (it != pairs.end())
    {
        if (it->high == high)
        {
            pair = *it;
            pairs.erase(it);
            return pair;
        }
        it++;
    }
    pair.high = high;
    pair.low = high;
    return pair;
}

static std::list<int> fordJohnsonList(std::list<int> values)
{
    std::list<IntPair> pairs;
    std::list<int> highs;
    std::list<PendingValue> pending;
    std::list<int> chain;
    std::list<int>::iterator it;
    IntPair pair;
    PendingValue item;
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
        pair = takeListPair(pairs, *it);
        item.value = pair.low;
        item.high = pair.high;
        item.hasHigh = true;
        pending.push_back(item);
        it++;
    }
    if (hasStray == true)
    {
        item.value = stray;
        item.high = 0;
        item.hasHigh = false;
        pending.push_back(item);
    }
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
