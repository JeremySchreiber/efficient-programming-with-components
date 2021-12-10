#pragma once

template<typename T, typename Compare>
// requires T is StrictWeakOrdering
inline
void sort_two(T& a, T& b, Compare cmp) {
    using std::swap;
    if (cmp(b, a))
        swap(a, b);        
}