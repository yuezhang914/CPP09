/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:28:34 by yzhang2           #+#    #+#             */
/*   Updated: 2026/05/19 17:28:35 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




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
