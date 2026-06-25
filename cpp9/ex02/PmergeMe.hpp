#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <list>
#include <vector>

class PmergeMe
{
    private:
        std::vector<int> _vector;
        std::list<int> _list;
        double _vectorTime;
        double _listTime;

        void sortVector(char **values, int count);
        void sortList(char **values, int count);

    public:
        PmergeMe(void);
        PmergeMe(PmergeMe const &other);
        PmergeMe &operator=(PmergeMe const &other);
        ~PmergeMe(void);

        void sort(char **values, int count);
};

#endif
