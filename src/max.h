#pragma once
#include <numeric>

template<typename T, typename Compare>
// Requires T is StrictWeakOrdering
inline
const T& max(const T& a, const T& b, Compare cmp) {
    return cmp(b, a) ? a : b;
}

template<typename T>
// Requires T is StrictWeakOrdering
inline
const T& max(const T& a, const T& b) {
    return ::max(a, b, std::less<T>{})
}

