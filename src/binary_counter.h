#pragma once
#include <vector>

template<typename I, typename T, typename Op>
T add_to_counter(I first, I last, Op op, const T& zero, T carry) {
    while (first != last) {
        if (*first == zero) {
            *first = carry;
            return zero;
        }
        carry = op(*first, carry);
        *first = zero;
        ++first;
    }

    return carry;
}

template<typename I, typename T, typename Op>
T reduce_counter(I first, I last, Op op, const T& zero) {
    while (first != last && *first == zero)
        ++first;

    if (first == last) return zero;
    T result = *first;
    ++first;

    while (first != last) {
        if (*first != zero)
            result = op(*first, result);
        ++first;
    }

    return result;
}


template<typename Op, typename T = typename Op::argument_type>
// requires Op is a BinaryOperation(T)
// requires Op is associative
class binary_counter 
{
    std::vector<T> counter;
    Op op;
    T zero;    

public:
    binary_counter(const Op& op, const T& zero) :              
        op{ op },
        zero{zero}   {    
        counter.reserve(32);
    }
    
    void add(T x) {
        x = add_to_counter(counter.begin(), counter.end(), op, zero, x);
        if (x != zero) 
            counter.push_back(x);
    }
    
    T reduce() {
        return reduce_counter(counter.begin(), counter.end(), op, zero);
    }
};