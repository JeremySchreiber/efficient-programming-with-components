#pragma once
#include <iterator>
#include "binary_counter.h"

template<typename Compare>
class MinOp
{
    Compare cmp;

public:
    MinOp(const Compare& cmp) : cmp{ cmp } {}

    template<typename I>
    I operator()(I x, I y) {
        return cmp(*y, *x) ? y : x;
    }
};

template<typename I, typename Compare>
I min_element_binary(I first, I last, Compare cmp) {
    binary_counter<I, MinOp<Compare>> min_counter(MinOp<Compare>{cmp}, last);

    while (first != last) {
        min_counter.add(first);
        ++first;
    }

    return min_counter.reduce();
}
