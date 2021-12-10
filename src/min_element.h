#pragma once
#include <numeric>
#include <iterator>

template<typename I, typename Compare>
// requires I is ForwardIterator
// requires Compare is a StrictWeakOrdering on the ValueType(I)
I min_element(I first, I last, Compare cmp) {
    if (first == last)
        return first;
    I min_el = first;
    ++first;
    while (first != last) {
        if (cmp(*first, *min_el))
            min_el = first;
        ++first;
    }
    return min_el;
}

template<typename I>
// requires I is ForwardIterators, T is StrictWeakOrdering
inline
I min_element(I first, I last) {
    return ::min_element(first, last, std::less<typename std::iterator_traits<I>::value_type>{});
}