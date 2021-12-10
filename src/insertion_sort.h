#pragma once
#include <iterator>
#include "algorithm.h"
#include "search.h"
#include "min_element.h"


template <typename I>
// requires: I is ForwardIterator
void rotate_right_by_1(I first, I last, std::forward_iterator_tag) {
	if (first == last) return;
	I f = first;
	while (++first != last) std::iter_swap(f, first);
}

template <typename I>
// requires: I is BiDirectionalIterator
void rotate_right_by_1(I first, I last, std::bidirectional_iterator_tag) {
	typedef typename std::iterator_traits<I>::value_type T;
	I prev_last = last;
	--prev_last;
	T x = *prev_last;
	std::copy_backward(first, prev_last, last);
	*first = x;
}

template <typename I>
inline
void rotate_right_by_1(I first, I last) {
	return rotate_right_by_1(first, last, typename std::iterator_traits<I>::iterator_category{});
}

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
I binary_insert_n(I first, N n, I current, R r) {
	// precondition: is_sorted(first, current) && current is a valid iterator
	//               && std::distance(first, current) == n
	I insertion_point = upper_bound_n(first, n, *current, r);
	rotate_right_by_1(insertion_point, ++current);
	return insertion_point;
}

template <typename I, typename N, typename R>
// requires: I is ForwardIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
I binary_insertion_sort_n(I first, N n, R r) {		
	if (n == N(0)) return first;
	I current = first;	
	++current;	
	N i(1);
	while (i < n) {
		// invariant: is_sorted(first, current) && std::distance(first, current) == i
		binary_insert_n(first, i++, current++, r);
	}
	return current;
}

template <typename I, typename R>
// requires: I is ForwardIterator
// requires: R is WeakStrictOrdering on the value type of I
inline
I binary_insertion_sort(I first, I last, R r) {
	return binary_insertion_sort_n(first, std::distance(first, last), r);
}

template <typename I, typename N>
// requires: I is ForwardIterator
// requires: N is Integral
inline
I binary_insertion_sort_n(I first, N n) {
	typedef typename std::iterator_traits<I>::value_type T;
	return binary_insertion_sort_n(first, n, std::less<T>{});
}

template <typename I>
// requires: I is ForwardIterator
inline
I binary_insertion_sort(I first, I last) {
	typedef typename std::iterator_traits<I>::value_type T;
	return binary_insertion_sort_n(first, std::distance(first, last), std::less<T>{});
}

template <typename I, typename R>
// requires: I is BidirectionalIterator
// requires: R is WeakStrictOrdering on the value type of I
I linear_insert(I first, I current, R r) {
	// precondition: is_sorted(first, current, r) && current is a valid iterator
	typedef typename std::iterator_traits<I>::value_type T;
	T value = *current;
	while (first != current && r(value, *predecessor(current))) {
		*current = *predecessor(current);
		--current;
	}
	*current = value;
	return current;
}

template <typename I, typename R>
// requires: I is BidirectionalIterator
// requires: R is WeakStrictOrdering on the value type of I
I linear_insert_with_sentinel(I first, I current, R r) {
	// precondition: is_sorted(first, current, r) && 
	//               current is a valid iterator &&
	//               first != current &&
	//               !r(*current, *first)
	typedef typename std::iterator_traits<I>::value_type T;
	T value = *current;
	while (r(value, *predecessor(current))) {
		*current = *predecessor(current);
		--current;
	}
	*current = value;
	return current;
}

template <typename I, typename N, typename R>
// requires: I is BidirectionalIterator
// requires: N is Integral
// requires: R is WeakStrictOrdering on the value type of I
I linear_insertion_sort_n(I first, N n, R r) {
	if (n == N(0)) return first;
	I current = first;
	++current;
	N i(1);
	while (i < n) {
		// invariant: is_sorted(first, current) && std::distance(first, current) == i
		linear_insert(first, current++, r);
		++i;
	}
	return current;
}

template <typename I, typename R>
// requires: I is BidirectionalIterator
// requires: R is WeakStrictOrdering on the value type of I
void insertion_sort_classic(I first, I last, R r) {
	if (first == last) return;
	I current = first;
	++current;
	while (current != last) {
		linear_insert(first, current, r);
		++current;
	}
}

template <typename I, typename R>
// requires: I is ForwardIterator
// requires: R is WeakStrictOrdering on the value type of I
void selection_sort(I first, I last, R r) {
	while (first != last) {
		std::swap(*first, *min_element(first, last, r)); // not stable!
		++first;
	}
}

template <typename I, typename R>
// requires: I is ForwardIterator
// requires: R is WeakStrictOrdering on the value type of I
void stable_selection_sort(I first, I last, R r) {
	while (first != last) {
		rotate_right_by_1(first, ++min_element(first, last, r));
		++first;
	}
}

template <typename I, typename R>
// requires: I is BidirectionalIterator
// requires: R is WeakStrictOrdering on the value type of I
void insertion_sort_suffix(I first, I last, R r) {
	// precondition: there is a valid sorted range before first that contains a sentinel OR first is the sentinel
	if (first == last) return;
	I current = first;
	++current;
	while (current != last) {
		linear_insert_with_sentinel(first, current, r);
		++current;
	}
}

template <typename I, typename R>
// requires: I is BidirectionalIterator
// requires: R is WeakStrictOrdering on the value type of I
void insertion_sort(I first, I last, R r) {
	if (first == last) return;
	I current = first;
	++current;
	if (current == last) return;
	rotate_right_by_1(first, ++min_element(first, last, r));
	insertion_sort_suffix(current, last, r);	
}

template <typename I, typename R>
// requires: I is BidirectionalIterator
// requires: R is WeakStrictOrdering on the value type of I
void insertion_sort_unstable(I first, I last, R r) {
	if (first == last) return;
	I current = first;
	++current;
	if (current == last) return;
	std::swap(*first, *min_element(first, last, r));
	insertion_sort_suffix(current, last, r);
}

template <typename I, typename N>
// requires: I is ForwardIterator
// requires: N is Integral
inline
I linear_insertion_sort_n(I first, N n) {
	typedef typename std::iterator_traits<I>::value_type T;
	return linear_insertion_sort_n(first, n, std::less<T>{});
}

template <typename I>
// requires: I is ForwardIterator
inline
I linear_insertion_sort(I first, I last) {
	typedef typename std::iterator_traits<I>::value_type T;
	return linear_insertion_sort_n(first, std::distance(first, last), std::less<T>{});
}
