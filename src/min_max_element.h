#pragma once
#include <utility>
#include <iterator>

template<typename I, typename Compare>
// requires I is ForwardIterator
// requires Compare is a StrictWeakOrdering on the ValueType(I)
std::pair<I, I> min_max_element(I first, I last, Compare cmp) {
    if (first == last)
        return std::make_pair(last, last);
    
    I min_el = first++;
    if (first == last)
        return std::make_pair(min_el, min_el);

    I max_el = first++;
    if (cmp(*max_el, *min_el))
        std::swap(min_el, max_el);

    while (std::distance(first, last) >= 2) {
        I potential_min = first;
        I potential_max = std::next(first);

        if (cmp(*potential_max, *potential_min))
            std::swap(potential_min, potential_max);        
        if (cmp(*potential_min, *min_el))
            min_el = potential_min;
        if (!cmp(*potential_max, *max_el))
            max_el = potential_max;

        std::advance(first, 2);       
    }

    if (first != last) {
        if (cmp(*first, *min_el))
            min_el = first;
        else if (!cmp(*first, *max_el))
            max_el = first;
    }

    return std::make_pair(min_el, max_el);
}


template<typename I>
// requires I is ForwardIterator
inline
std::pair<I, I> min_max_element(I first, I last) {
    return ::min_max_element(first, last, std::less<typename std::iterator_traits<I>::value_type>{});
}