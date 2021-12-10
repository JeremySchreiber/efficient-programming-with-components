#pragma once
#include <utility>
#include <iterator>

template <typename I, typename P>
// requires: I is InputIterator
// requires: P is UnaryPredicate on ValueType(I)
I find_if(I first, I last, P pred) {
    // Precondition: [first, last) is a valid range
    while (first != last && !pred(*first)) ++first;
    return first;
    // Postcondition: return value is either last OR pred(*first) = true
}

template <typename I, typename N, typename P>
// requires: I is InputIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
std::pair<I, N> find_if_n(I first, N n, P pred) {
    // Precondition: [first, n) is a valid range
    while (n != N(0) && !pred(*first)) {
        ++first;
        --n;
    }
    return std::make_pair(first, n);    
}

template <typename I, typename P>
// requires: I is InputIterator
// requires: P is UnaryPredicate on ValueType(I)
I find_if_not(I first, I last, P pred) {
    // Precondition: [first, last) is a valid range
    while (first != last && pred(*first)) ++first;
    return first;
    // Postcondition: return value is either last OR pred(*first) = false
}

template <typename I, typename N, typename P>
// requires: I is InputIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
std::pair<I, N> find_if_not_n(I first, N n, P pred) {
    // Precondition: [first, n) is a valid range
    while (n != N(0) && pred(*first)) {
        ++first;
        --n;
    }
    return std::make_pair(first, n);    
}

template <typename I, typename P>
// requires: I is InputIterator
// requires: P is UnaryPredicate on ValueType(I)
inline
bool all_of(I first, I last, P pred) {
    // Precondition: [first, last) is a valid range
    return find_if_not(first, last, pred) == last;    
}

template <typename I, typename P>
// requires: I is InputIterator
// requires: P is UnaryPredicate on ValueType(I)
inline
bool none_of(I first, I last, P pred) {
    // Precondition: [first, last) is a valid range
    return find_if(first, last, pred) == last;
}

template <typename I, typename P>
// requires: I is InputIterator
// requires: P is UnaryPredicate on ValueType(I)
inline
bool any_of(I first, I last, P pred) {
    // Precondition: [first, last) is a valid range
    return find_if(first, last, pred) != last;
}

template <typename I, typename P>
// requires: I is InputIterator
// requires: P is UnaryPredicate on ValueType(I)
bool is_partitioned(I first, I last, P pred) {
    // Precondition: [first, last) is a valid range
    return find_if(find_if_not(first, last, pred), last, pred) == last;
}

template <typename I, typename N, typename P>
// requires: I is InputIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
bool is_partitioned_n(I first, N n, P pred) {
    // Precondition: [first, last) is a valid range
    std::pair<I, N> x = find_if_not_n(first, n, pred);
    return find_if_n(x.first, x.second, pred).second == N(0);
}

template <typename I, typename N>
void advance(I& it, N n, std::input_iterator_tag) {
    while (n--) ++it;
}

template <typename I, typename N>
void advance(I& it, N n, std::random_access_iterator_tag) {
    it += n;
}

template <typename I, typename N>
inline
void advance(I& it, N n) {
    advance(it, n, typename std::iterator_traits<I>::iterator_category{});
}

template <typename I>
typename std::iterator_traits<I>::difference_type distance(I first, I last) {
    return distance(first, last, typename std::iterator_traits<I>::iterator_category{});
}

template <typename I>
typename std::iterator_traits<I>::difference_type distance(I first, I last, std::input_iterator_tag) {
    typename std::iterator_traits<I>::difference_type n(0);
    while (first != last) {
        ++n;
        ++first;
    }
    return n;
}

template <typename I>
typename std::iterator_traits<I>::difference_type distance(I first, I last, std::random_access_iterator_tag) {
    return last - first;
}

template <typename I, typename N, typename P>
// requires: I is ForwardIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
I partition_point_n(I first, N n, P pred) {
    while(n) {
        N half = n >> 1;
        I middle = first;
        ::advance(middle, half);
        if (pred(*middle)) {
            first = ++middle;
            n -= (half + 1);
        }
        else {            
            n = half;
        }
    }
    return first;
}

template <typename I, typename P>
// requires: I is ForwardIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
I partition_point(I first, I last, P pred) {
    return partition_point_n(first, distance(first, last), pred);
}

template <typename I, typename R>
// requires: I is forward iterator
// requires: R is WeakStrictOrdering on the value type of I
bool is_sorted(I first, I last, R r) {
    if (first == last) return true;
    I previous = first;
    while (++first != last && !r(*first, *previous)) previous = first;
    return first == last;
}

template <typename I>
// requires: I is ForwardIterator with a totally ordered value type
inline
bool is_sorted(I first, I last) {
    typedef typename std::iterator_traits<I>::value_type T;
    return is_sorted(first, last, std::less<T>{});
}

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
bool is_sorted_n(I first, N n, R r) {
    if (!n) return true;
    I previous = first;    
    while (n && !r(*++first, *previous)) {
        previous = first;
        --n;
    }
    return !n;
}

template <typename I, typename N>
// requires: I is ForwardIterator
// requires: N is integral
// requires: P is UnaryPredicate on ValueType(I)
inline
bool is_sorted_n(I first, N n) {
    typedef typename std::iterator_traits<I>::value_type T;
    return is_sorted_n(first, n, std::less<T>{});
}

template <typename R, typename T>
// requires: R is a StrictWeakOrdering
// requires: T is an argument type of R
class lower_bound_predicate
{
private:
    R r;
    const T* a;
public:
    lower_bound_predicate(const R& r, const T& a) : r{ r }, a{ &a } {}
    bool operator()(const T& x) { return r(x, *a); }
};

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
inline
I lower_bound_n(I first, N n, const typename std::iterator_traits<I>::value_type& a, R r) {
    // precondition: is_sorted_n(first, n, r)
    typedef typename std::iterator_traits<I>::value_type T;
    return partition_point_n(first, n, lower_bound_predicate<R, T>{r, a});
}

template <typename I, typename R>
// requires: I is ForwardIterator
// requires: R is WeakStrictOrdering on the value type of I
inline
I lower_bound(I first, I last, const typename std::iterator_traits<I>::value_type& a, R r) {
    // precondition: is_sorted(first, last, r)    
    return lower_bound_n(first, distance(first, last), a, r);
}

template <typename I, typename N>
// requires: I is ForwardIterator
// requires: N is Integral
inline
I lower_bound_n(I first, N n, const typename std::iterator_traits<I>::value_type& a) {
    // precondition: is_sorted_n(first, n, r)
    typedef typename std::iterator_traits<I>::value_type T;
    return lower_bound_n(first, n, a, std::less<T>{});
}

template <typename I>
// requires: I is ForwardIterator
inline
I lower_bound(I first, I last, const typename std::iterator_traits<I>::value_type& a) {
    // precondition: is_sorted(first, last, r)    
    return lower_bound_n(first, distance(first, last), a);
}

template <typename R, typename T>
class upper_bound_predicate
{
private:
    R r;
    const T* a;
public:
    upper_bound_predicate(const R& r, const T& a) : r{ r }, a{ &a } {}
    bool operator()(const T& x) { return !r(*a, x); }
};

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
inline
I upper_bound_n(I first, N n, const typename std::iterator_traits<I>::value_type& a, R r) {
    // precondition: is_sorted_n(first, n, r)
    typedef typename std::iterator_traits<I>::value_type T;
    return partition_point_n(first, n, upper_bound_predicate<R, T>{r, a});
}

template <typename I, typename R>
// requires: I is ForwardIterator
// requires: R is WeakStrictOrdering on the value type of I
inline
I upper_bound(I first, I last, const typename std::iterator_traits<I>::value_type& a, R r) {
    // precondition: is_sorted(first, last, r)    
    return upper_bound_n(first, distance(first, last), a, r);
}

template <typename I, typename N>
// requires: I is ForwardIterator
// requires: N is Integral
inline
I upper_bound_n(I first, N n, const typename std::iterator_traits<I>::value_type& a) {
    // precondition: is_sorted_n(first, n, r)
    typedef typename std::iterator_traits<I>::value_type T;
    return upper_bound_n(first, n, a, std::less<T>{});
}

template <typename I>
// requires: I is ForwardIterator
inline
I upper_bound(I first, I last, const typename std::iterator_traits<I>::value_type& a) {
    // precondition: is_sorted(first, last, r)    
    return upper_bound_n(first, distance(first, last), a);
}
