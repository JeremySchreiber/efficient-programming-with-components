#pragma once
#include "binary_counter.h"

template <typename I>
// requires I is Linked Iterator
I reverse_linked(I first, I last, I tail) {
    while (first != last) {
        I next = first;
        ++next;
        set_successor(first, tail);
        tail = first;
        first = next;
    }
    return tail;
}

template <typename I, typename Compare>
// requires I is Linked Iterator
I merged_linked_simple(I first1, I last1, I first2, I last2, Compare cmp) {
    I result = last1;
    while (first1 != last1 && first2 != last2) {
        I tmp = cmp(*first2, *first1) ? first2++ : first1++;
        set_successor(tmp, result);
        result = tmp;
    }
    return reverse_linked(result, last1, first1 == last1 ? first2 : first1);
}

template <typename I, typename Compare>
// requires I is Linked Iterator
struct mergesort_linked_operation
{
    typedef I argument_type;
    I nil;
    Compare cmp;
    mergesort_linked_operation(I nil, const Compare& cmp) : nil{ nil }, cmp{ cmp } {}
    I operator()(I x, I y) { return merged_linked_simple(x, nil, y, nil, cmp); }
};

template <typename I, typename Compare>
// requires I is Linked Iterator
I mergesort_linked(I first, I last, Compare cmp) {
    mergesort_linked_operation<I, Compare> op{ last, cmp };
    binary_counter<mergesort_linked_operation<I, Compare>> counter{ op, last };
    while (first != last) {
        I tmp = first++;
        set_successor(tmp, last);
        counter.add(tmp);
    }
    return counter.reduce();
}

template<typename I1, typename I2>
// requires I1 is InputIterator
// requires I2 is Linked List Iterator
I2 generate_list(I1 first, I1 last, I2 tail) {
    if (first == last) return tail;
    push_front(tail, *first);
    ++first;
    I2 front = tail;    
    while (first != last) {
        push_back(tail, *first);
        ++first;
        ++tail;
    }
    return front;
}