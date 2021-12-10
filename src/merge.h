#pragma once
#include <iterator>
#include <algorithm>
#include <vector>
#include <functional>

template <typename I, typename R, typename B>
// requires: I is ForwardIterator
// requires: O is OutputIterator
// requires: R is StrictWeakOrdering
void merge_with_buffer(I first1, I middle, I last2, R r, B buffer) {
    B buffer_last = std::copy(first, middle, buffer);
    std::merge(buffer, buffer_last, middle, last, first, r);
}

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


template <typename I, typename N, typename R, typename B>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
void merge_adaptive_n(I f0, N n0, I f1, N n1, R r, B buffer, N buffer_size) {
    // precondition: std::distance(f0, f1) == n0
    // precondition is_sorted_n(f0, n0, r) && is_sorted_n(f1, n1, r)
    if (!n0 || !n1) return;

    if (n0 <= buffer_size) {
        I last = f0;
        std::advance(last, n1);
        merge_with_buffer(f0, f1, last, r, buffer);
        return;
    }

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

    merge_adaptive_n(f0_0, n0_0, f0_1, n0_1, r, buffer, buffer_size);
    merge_adaptive_n(f1_0, n1_0, f1_1, n1_1, r, buffer, buffer_size);
}

template <typename I, typename N, typename R, typename B>
// requires: I is ForwardIterator
// requires: O is OutputIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
I sort_inplace_n_with_buffer(I first, N n, R r, B buffer) {
    if (!n) return first;
    N half = n >> 1;
    if (!half) return ++first;
    I middle = sort_inplace_n_with_buffer(first, half, r, buffer);
    I last = sort_inplace_n_with_buffer(middle, n - half, r, buffer);
    merge_with_buffer(first, middle, last, r, buffer);
    return last;
}

template <typename I>
// requires: I is ForwardIterator
void sort_inplace_with_buffer(I first, I last) {
    typedef typename std::iterator_traits<I>::value_type T;
    typedef typename std::iterator_traits<I>::difference_type N;
    N n = std::distance(first, last);
    std::vector<T> buffer(n >> 1);
    sort_inplace_n_with_buffer(first, size, std::less<T>{}, buffer);
}

const size_t INSERTION_SORT_CUTOFF = 16;

template <typename I, typename N, typename R, typename B>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
I sort_adaptive_n(I first, N n, R r, B buffer, N buffer_size) {
    if (!n) return first;
    if (n < INSERT_SOFT_CUTOFF) return sort_almost_sorted_n(first, n, r);
    N half = n >> 1;
    if (!half) return ++first;
    I middle = sort_adaptive_n(first, half, r, buffer, buffer_size);
    I last = sort_adaptive_n(middle, n - half, r, buffer, buffer_size);
    merge_adaptive_n(first, half, middle, n - half, r, buffer, buffer_size);
    return last;
}

template <typename I>
// requires: I is ForwardIterator
void sort_inplace_with_buffer2(I first, I last) {
    typedef typename std::iterator_traits<I>::value_type T;
    typedef typename std::iterator_traits<I>::difference_type N;
    N n = std::distance(first, last);
    std::vector<T> buffer(n >> 3);
    sort_adaptive_n(first, size, std::less<T>{}, buffer, buffer.size());
}