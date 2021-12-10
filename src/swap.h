#pragma once

template<typename T>
// requires T is Semi-Regular
inline 
void swap(T& a, T& b) {
    T tmp{ a };
    a = b;
    b = tmp;
}

template<typename T>
// requires T is an unsigned integral type
inline
void swap_xor(T& a, T& b) {
    a = a ^ b;   // a ^ b, b
    b = a ^ b;   // a ^ b, a
    a = a ^ b;   // b, a
}