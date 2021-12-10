#pragma once
#include <iterator>

template<typename I>
// requires I is BidirectionalIterator
void reverse(I first, I last) {
    using std::swap;     
    while (first != last && first != --last) {        
        swap(*first, *last);
        ++first;        
    }    
} 