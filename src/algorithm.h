#pragma once
#include <algorithm>

#define InputIterator typename
#define ForwardIterator typename
#define RandomAccessIterator typename
#define BidirectionalIterator typename
#define Integral typename

template <InputIterator I>
inline
I successor(I x) { return ++x; }

template <BidirectionalIterator I>
inline
I predecessor(I x) { return --x; }

template <ForwardIterator I, Integral N>
N iota(I first, I last, N start = N(0), N step = N(1)) {    
    using T = typename std::iterator_traits<I>::value_type;
    while (first != last) {
        *first = T(start);
        start += step;
        ++first;        
    }
    return start;
}

template <ForwardIterator I>
void iota(I first, I last) {
    using T = typename std::iterator_traits<I>::value_type;
    iota(first, last, T(0), T(1));
}

template <RandomAccessIterator I>
void random_iota(I first, I last) {
    iota(first, last);
    std::random_shuffle(first, last);
}

template <BidirectionalIterator I>
void reverse_iota(I first, I last) {
    iota(first, last);
    std::reverse(first, last);
}

template <ForwardIterator I>
void print_range(I first, I last) {
    while (first != last) {
        std::cout << *first << " ";
        ++first;
    }
    std::cout << std::endl;
}
