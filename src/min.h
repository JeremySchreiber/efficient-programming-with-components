#pragma once
#include <numeric>

template<typename T, typename Compare>
// requires StrictWeakOrdering on T
inline
const T& min(const T& a, const T& b, Compare cmp) {
    return cmp(b, a) ? b : a;
}

template<typename T>
inline
const T& min(const T& a, const T& b) {
    return ::min(a, b, std::less<T>{});
}