#include "PmergeMe.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/time.h>

struct SortItem
{
    int value;
    std::size_t id;
};

struct ItemPair
{
    SortItem high;
    SortItem low;
};

struct PendingItem
{
    SortItem item;
    std::size_t highId;
    bool hasHigh;
};

static SortItem makeSortItem(int value, std::size_t id)
{
    SortItem item;

    item.value = value;
    item.id = id;
    return item;
}

static std::size_t findVectorHighLimit(std::vector<SortItem> const &chain, std::size_t highId)
{
    std::size_t i;

    i = 0;
    while (i < chain.size())
    {
        if (chain[i].id == highId)
            return i;
        i++;
    }
    return chain.size();
}

static void insertVectorItem(std::vector<SortItem> &chain, PendingItem const &pending)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    if (pending.hasHigh == true)
        right = findVectorHighLimit(chain, pending.highId);
    else
        right = chain.size();
    while (left < right)
    {
        middle = left + (right - left) / 2;
        if (chain[middle].value < pending.item.value)
            left = middle + 1;
        else
            right = middle;
    }
    chain.insert(chain.begin() + left, pending.item);
}

static ItemPair takeVectorPair(std::vector<ItemPair> &pairs, std::size_t highId)
{
    std::size_t i;
    ItemPair pair;

    i = 0;
    while (i < pairs.size())
    {
        if (pairs[i].high.id == highId)
        {
            pair = pairs[i];
            pairs.erase(pairs.begin() + i);
            return pair;
        }
        i++;
    }
    pair.high = makeSortItem(0, 0);
    pair.low = makeSortItem(0, 0);
    return pair;
}

static void insertVectorPending(std::vector<SortItem> &chain, std::vector<PendingItem> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertVectorItem(chain, pending[0]);
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
            insertVectorItem(chain, pending[index - 1]);
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

static void makeVectorPair(SortItem const &first, SortItem const &second, ItemPair &pair)
{
    if (first.value < second.value)
    {
        pair.high = second;
        pair.low = first;
    }
    else
    {
        pair.high = first;
        pair.low = second;
    }
}

static void pushVectorPending(std::vector<PendingItem> &pending, SortItem item, std::size_t highId, bool hasHigh)
{
    PendingItem pendingItem;

    pendingItem.item = item;
    pendingItem.highId = highId;
    pendingItem.hasHigh = hasHigh;
    pending.push_back(pendingItem);
}

static std::vector<SortItem> fordJohnsonVector(std::vector<SortItem> values)
{
    std::vector<ItemPair> pairs;
    std::vector<SortItem> highs;
    std::vector<PendingItem> pending;
    std::vector<SortItem> chain;
    ItemPair pair;
    bool hasStray;
    SortItem stray;
    std::size_t i;

    if (values.size() < 2)
        return values;
    hasStray = false;
    i = 0;
    while (i + 1 < values.size())
    {
        makeVectorPair(values[i], values[i + 1], pair);
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
        pair = takeVectorPair(pairs, chain[i].id);
        pushVectorPending(pending, pair.low, pair.high.id, true);
        i++;
    }
    if (hasStray == true)
        pushVectorPending(pending, stray, 0, false);
    insertVectorPending(chain, pending);
    return chain;
}

static std::list<SortItem>::iterator findListHighLimit(std::list<SortItem> &chain, std::size_t highId)
{
    std::list<SortItem>::iterator it;

    it = chain.begin();
    while (it != chain.end())
    {
        if (it->id == highId)
            return it;
        it++;
    }
    return chain.end();
}

static std::size_t getListRangeSize(std::list<SortItem>::iterator first, std::list<SortItem>::iterator last)
{
    std::size_t size;

    size = 0;
    while (first != last)
    {
        first++;
        size++;
    }
    return size;
}

static std::list<SortItem>::iterator moveListIterator(std::list<SortItem>::iterator it, std::size_t steps)
{
    while (steps > 0)
    {
        it++;
        steps--;
    }
    return it;
}

static std::list<SortItem>::iterator findListInsertPosition(std::list<SortItem> &chain, std::list<SortItem>::iterator limit, int value)
{
    std::list<SortItem>::iterator left;
    std::list<SortItem>::iterator middle;
    std::size_t count;
    std::size_t step;

    left = chain.begin();
    count = getListRangeSize(left, limit);
    while (count > 0)
    {
        step = count / 2;
        middle = moveListIterator(left, step);
        if (middle->value < value)
        {
            left = middle;
            left++;
            count = count - step - 1;
        }
        else
            count = step;
    }
    return left;
}

static void insertListItem(std::list<SortItem> &chain, PendingItem const &pending)
{
    std::list<SortItem>::iterator limit;
    std::list<SortItem>::iterator position;

    if (pending.hasHigh == true)
        limit = findListHighLimit(chain, pending.highId);
    else
        limit = chain.end();
    position = findListInsertPosition(chain, limit, pending.item.value);
    chain.insert(position, pending.item);
}

static PendingItem getListPending(std::list<PendingItem> const &pending, std::size_t index)
{
    std::list<PendingItem>::const_iterator it;
    std::size_t i;

    it = pending.begin();
    i = 0;
    while (i < index && it != pending.end())
    {
        it++;
        i++;
    }
    return *it;
}

static ItemPair takeListPair(std::list<ItemPair> &pairs, std::size_t highId)
{
    std::list<ItemPair>::iterator it;
    ItemPair pair;

    it = pairs.begin();
    while (it != pairs.end())
    {
        if (it->high.id == highId)
        {
            pair = *it;
            pairs.erase(it);
            return pair;
        }
        it++;
    }
    pair.high = makeSortItem(0, 0);
    pair.low = makeSortItem(0, 0);
    return pair;
}

static void insertListPending(std::list<SortItem> &chain, std::list<PendingItem> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertListItem(chain, getListPending(pending, 0));
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
            insertListItem(chain, getListPending(pending, index - 1));
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

static void makeListPair(SortItem const &first, SortItem const &second, ItemPair &pair)
{
    if (first.value < second.value)
    {
        pair.high = second;
        pair.low = first;
    }
    else
    {
        pair.high = first;
        pair.low = second;
    }
}

static void pushListPending(std::list<PendingItem> &pending, SortItem item, std::size_t highId, bool hasHigh)
{
    PendingItem pendingItem;

    pendingItem.item = item;
    pendingItem.highId = highId;
    pendingItem.hasHigh = hasHigh;
    pending.push_back(pendingItem);
}

static std::list<SortItem> fordJohnsonList(std::list<SortItem> values)
{
    std::list<ItemPair> pairs;
    std::list<SortItem> highs;
    std::list<PendingItem> pending;
    std::list<SortItem> chain;
    std::list<SortItem>::iterator it;
    ItemPair pair;
    bool hasStray;
    SortItem first;
    SortItem second;
    SortItem stray;

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
            makeListPair(first, second, pair);
            pairs.push_back(pair);
            highs.push_back(pair.high);
        }
    }
    chain = fordJohnsonList(highs);
    it = chain.begin();
    while (it != chain.end())
    {
        pair = takeListPair(pairs, it->id);
        pushListPending(pending, pair.low, pair.high.id, true);
        it++;
    }
    if (hasStray == true)
        pushListPending(pending, stray, 0, false);
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
    std::vector<SortItem> items;
    double start;
    int i;

    start = getTimeInMicroseconds();
    _vector.clear();
    i = 0;
    while (i < count)
    {
        items.push_back(makeSortItem(std::atoi(values[i]), static_cast<std::size_t>(i + 1)));
        i++;
    }
    items = fordJohnsonVector(items);
    i = 0;
    while (i < static_cast<int>(items.size()))
    {
        _vector.push_back(items[static_cast<std::size_t>(i)].value);
        i++;
    }
    _vectorTime = getTimeInMicroseconds() - start;
}

void PmergeMe::sortList(char **values, int count)
{
    std::list<SortItem> items;
    std::list<SortItem>::iterator it;
    double start;
    int i;

    start = getTimeInMicroseconds();
    _list.clear();
    i = 0;
    while (i < count)
    {
        items.push_back(makeSortItem(std::atoi(values[i]), static_cast<std::size_t>(i + 1)));
        i++;
    }
    items = fordJohnsonList(items);
    it = items.begin();
    while (it != items.end())
    {
        _list.push_back(it->value);
        it++;
    }
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
