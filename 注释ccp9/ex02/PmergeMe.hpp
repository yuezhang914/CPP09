#ifndef PMERGEME_HPP
#define PMERGEME_HPP

/*
包含库说明：
list：使用 std::list<int> 作为 ex02 的第二种容器，并实现链表版本排序。
vector：使用 std::vector<int> 作为 ex02 的第一种容器，并实现数组式容器版本排序。
*/
#include <list>
#include <vector>

/*
类用途：PmergeMe 负责把命令行中的正整数序列分别放入 vector 和 list，并用 merge-insert / Ford-Johnson 思路排序和计时。
成员变量：_vector 保存 vector 版本数据；_list 保存 list 版本数据；_vectorTime 保存 vector 装载加排序耗时；_listTime 保存 list 装载加排序耗时。
成员函数逻辑：sort 是公开入口，负责输出 Before、调用两个容器排序并输出 After 和时间；sortVector 和 sortList 是内部函数，分别处理两个容器。
*/
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
