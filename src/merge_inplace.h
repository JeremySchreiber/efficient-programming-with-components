#pragma once

#include <algorithm>
#include <iterator>
#include <functional>
#include "search.h"

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
inline
void merge_inplace_left_subproblem(I  f0,   N  n0,
                                   I  f1,   N  n1,
                                   I& f0_0, N& n0_0,
                                   I& f0_1, N& n0_1,
                                   I& f1_0, N& n1_0,
                                   I& f1_1, N& n1_1,
                                   R r) {
    // precondition: std::distance(f0, f1) == n0
    // precondition: is_sorted_n(f0, n0, r) and is_sorted_n(f1, n1, r)
    f0_0 = f0;
    n0_0 = n0 >> 1;
    f0_1 = f0;
    std::advance(f0_1, n0_0);
    f1_1 = lower_bound_n(f1, n1, *f0_1, r);
    f1_0 = std::rotate(f0_1, f1, f1_1);
    n0_1 = std::distance(f0_1, f1_0);
    ++f1_0;
    n1_0 = (n0 - n0_0) - 1;
    n1_1 = n1 - n0_1;
}

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
inline
void merge_inplace_right_subproblem(I  f0,   N  n0,
                                    I  f1,   N  n1,
                                    I& f0_0, N& n0_0,
                                    I& f0_1, N& n0_1,
                                    I& f1_0, N& n1_0,
                                    I& f1_1, N& n1_1,
                                    R r) {
    // precondition: std::distance(f0, f1) == n0
    // precondition: is_sorted_n(f0, n0, r) and is_sorted_n(f1, n1, r)
    f0_0 = f0;
    n0_1 = n1 >> 1;
    f1_1 = f1;
    std::advance(f1_1, n0_1);
    f0_1 = upper_bound_n(f0, n0, *f1_1, r);
    ++f1_1;
    f1_0 = std::rotate(f0_1, f1, f1_1);
    n0_0 = std::distance(f0_0, f0_1);
    n1_0 = n0 - n0_0;
    n1_1 = (n1 - n0_1) - 1;
}


template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
void merge_inplace_n(I f0, N n0, I f1, N n1, R r) {
    // precondition: std::distance(f0, f1) == n0
    // precondition is_sorted_n(f0, n0, r) && is_sorted_n(f1, n1, r)
    if (!n0 || !n1) return;
    I f0_0, f0_1, f1_0, f1_1;
    N n0_0, n0_1, n1_0, n1_1;
    if (n0 < n1)
        merge_inplace_left_subproblem(f0, n0,
            f1, n1,
            f0_0, n0_0,
            f0_1, n0_1,
            f1_0, n1_0,
            f1_1, n1_1,
            r);
    else
        merge_inplace_right_subproblem(f0, n0,
            f1, n1,
            f0_0, n0_0,
            f0_1, n0_1,
            f1_0, n1_0,
            f1_1, n1_1,
            r);
    merge_inplace_n(f0_0, n0_0, f0_1, n0_1, r);
    merge_inplace_n(f1_0, n1_0, f1_1, n1_1, r);
}

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
I sort_inplace_n(I first, N n, R r) {
    if (!n) return first;
    N half = n >> 1;
    if (!half) return ++first;
    I middle = sort_inplace_n(first, half, r);
    I last = sort_inplace_n(middle, n - half, r);
    merge_inplace_n(first, half, middle, n - half, r);
    return last;
}