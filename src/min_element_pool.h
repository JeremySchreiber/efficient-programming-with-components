#pragma once
#include "list_pool.h"

template<typename T, typename N = std::size_t>
using list_type_t = typename list_pool<T, N>::list_type;

template<typename Compare, typename T, typename N = std::size_t>
list_type_t<T, N> 
min_element_pool(const list_pool<T, N>& pool, list_type_t<T, N> list, Compare cmp) {
    if (pool.is_empty(list)) return list;

    list_type_t<T, N> min_el = list;
    list = pool.next(list);

    while (!pool.is_empty(list)) {
        if (cmp(pool.value(list), pool.value(min_el)))
            min_el = list;
        list = pool.next(list);
    }

    return min_el;
}

template<typename T, typename N = std::size_t>
inline
list_type_t<T, N>
min_element_pool(const list_pool<T, N>& pool, list_type_t<T, N> list) {
    return ::min_element_pool(pool, list, std::less<T>{});
}