#pragma once
#include <utility>
#include <iterator>
#include "binary_counter.h"
#include "list_pool.h"
#include "min_element_pool.h"


template<typename Compare>
class IterCmp
{
    Compare cmp;

public:
    IterCmp(const Compare& cmp) : cmp{ cmp } {}

    template <typename I>
    bool operator()(I x, I y) {
        return cmp(*x, *y);
    }
};




template<typename Compare>
class RandomAccessCompare
{
    Compare cmp;

public:
    RandomAccessCompare(const Compare& cmp) : cmp{ cmp } {}

    template <typename I>
    // requires RandomAccessIterator(I)
    bool operator()(I x, I y) {
        return (x < y) ? !cmp(*y, *x) : cmp(*x, *y);
    }
};



template<typename Compare>
class ReverseIterCmp
{
    Compare cmp;

public:
    ReverseIterCmp(const Compare& cmp) : cmp{ cmp } {}

    template <typename I>
    // requires RandomAccessIterator(I)
    bool operator()(I x, I y) {
        return !cmp(*y, *x);
    }
};



template<typename Compare>
class IndexCompare
{
    Compare cmp;

public:
    IndexCompare(const Compare& cmp) : cmp{ cmp } {}

    template <typename I, typename N>    
    bool operator()(const std::pair<I, N>& x, const std::pair<I, N>& y) {
        return (x.second < y.second) ? !cmp(y.first, x.first) : cmp(x.first, y.first);
    }
};



template<typename Pool, typename Compare>
class PoolCombiner
{
    Pool* pool;
    Compare cmp;

public:
    PoolCombiner(Pool& pool, const Compare& cmp) :
        pool{ &pool }, 
        cmp { cmp } 
    {}

    template<typename I, typename L>
    std::pair<I, L> operator()(const std::pair<I, L>& x, const std::pair<I, L>& y) {
        return cmp(x.first, y.first) ? combine(x, y) : combine(y, x);            
    }

    template<typename I, typename L>
    std::pair<I, L> combine(const std::pair<I, L>& x, const std::pair<I, L>& y) {
        free_list(*pool, y.second);
        return std::make_pair(x.first, pool->allocate(y.first, x.second));
    }
};





template<typename I, typename Pool, typename Compare>
class PoolIndexCombiner
{
    Pool* pool;
    Compare cmp;
    I begin;

public:
    PoolIndexCombiner(Pool& pool, const Compare& cmp, I begin) :
        pool{ &pool },
        cmp{ cmp },
        begin{ begin }
    {}

    template<typename I, typename L>
    std::pair<I, L> operator()(const std::pair<I, L>& x, const std::pair<I, L>& y) {
        return cmp(x.first, y.first) ? combine(x, y) : combine(y, x);
    }

    template<typename I, typename L>
    std::pair<I, L> combine(const std::pair<I, L>& x, const std::pair<I, L>& y) {
        free_list(*pool, y.second);
        return std::make_pair(x.first, pool->allocate(std::make_pair(y.first, std::distance(begin, y.first)), x.second));
    }
};


template<typename Pool, typename Compare>
class PoolListsCombiner
{
    Pool* pool;
    Compare cmp;

    enum Direction { RIGHT, LEFT};

public:
    PoolListsCombiner(Pool& pool, const Compare& cmp) :
        pool{ &pool },
        cmp{ cmp }
    {}

    template<typename I, typename L>
    std::pair<I, L> operator()(const std::pair<I, L>& x, const std::pair<I, L>& y) {
        return cmp(x.first, y.first) ? combine(x, y, LEFT) : combine(y, x, RIGHT);
    }

    template<typename I, typename L>
    std::pair<I, L> combine(const std::pair<I, L>& x, const std::pair<I, L>& y, Direction d) {
        free_list(*pool, y.second.first);
        free_list(*pool, y.second.second);
        if (d == RIGHT)
            return std::make_pair(x.first, std::make_pair(pool->allocate(y.first, x.second.first), x.second.second));
        else
            return std::make_pair(x.first, std::make_pair(x.second.first, pool->allocate(y.first, x.second.second)));
    }
};





template<typename I, typename Compare>
std::pair<I, I> min2_elements(I first, I last, Compare cmp) {
    using pool_t = list_pool<I>;
    using list_t = typename pool_t::list_type;
    using counter_t = std::pair<I, list_t>;
    using combiner_t = PoolCombiner<pool_t, IterCmp<Compare>>;
    
    pool_t pool;    
    binary_counter<counter_t, combiner_t> counter(combiner_t{ pool, cmp }, std::make_pair(last, pool.empty()));

    while (first != last) 
        counter.add(std::make_pair(first++, pool.empty()));        
    counter_t result = counter.reduce();    
    return std::make_pair(result.first, result.second == pool.empty() ? last : pool.value(min_element_pool(pool, result.second, IterCmp<Compare>{cmp})));    
}

template<typename I>
inline
std::pair<I, I> min2_elements(I first, I last) {
    return min2_elements(first, last, std::less<typename std::iterator_traits<I>::value_type>{});
}




template<typename I, typename Compare>
// requires I is a RandomAccessIterator
// and Compare is a StrictWeakOrdering on ValueType(I)
std::pair<I, I> min2_elements_stable0(I first, I last, Compare cmp) {
    using pool_t = list_pool<I>;
    using list_t = typename pool_t::list_type;
    using counter_t = std::pair<I, list_t>;
    using combiner_t = PoolCombiner<pool_t, IterCmp<Compare>>;

    pool_t pool;
    binary_counter<counter_t, combiner_t> counter(combiner_t{ pool, cmp }, std::make_pair(last, pool.empty()));

    while (first != last)
        counter.add(std::make_pair(first++, pool.empty()));
    counter_t result = counter.reduce();
    return std::make_pair(result.first, result.second == pool.empty() ? last : pool.value(min_element_pool(pool, result.second, RandomAccessCompare<Compare>{cmp})));
}




template<typename I, typename Compare>
// requires I is a RandomAccessIterator
// and Compare is a StrictWeakOrdering on ValueType(I)
std::pair<I, I> min2_elements_stable1(I first, I last, Compare cmp) {
    using N = typename std::iterator_traits<I>::difference_type;
    using pair_type = std::pair<I, N>;
    using pool_t = list_pool<pair_type>;
    using list_t = typename pool_t::list_type;
    using counter_t = std::pair<I, list_t>;
    using combiner_t = PoolIndexCombiner<I, pool_t, IterCmp<Compare>>;

    pool_t pool;
    binary_counter<counter_t, combiner_t> counter(combiner_t{ pool, cmp, first }, std::make_pair(last, pool.empty()));

    while (first != last)
        counter.add(std::make_pair(first++, pool.empty()));
    counter_t result = counter.reduce();
    return std::make_pair(result.first, (result.second == pool.empty()) ? last : pool.value(min_element_pool(pool, result.second, IndexCompare<IterCmp<Compare>>{cmp})).first);
}



template<typename I, typename Compare>
std::pair<I, I> min2_elements_stable2(I first, I last, Compare cmp) {
    using pool_t = list_pool<I>;
    using list_t = typename pool_t::list_type;
    using counter_t = std::pair<I, std::pair<list_t, list_t>>;
    using combiner_t = PoolListsCombiner<pool_t, IterCmp<Compare>>;

    pool_t pool;
    binary_counter<counter_t, combiner_t> counter(combiner_t{ pool, cmp }, std::make_pair(last, std::make_pair(pool.empty(), pool.empty())));

    while (first != last)
        counter.add(std::make_pair(first++, std::make_pair(pool.empty(), pool.empty())));
    counter_t result = counter.reduce();           

    I min_right = pool.value(min_element_pool(pool, result.second.first, IterCmp<Compare>{cmp}));    
    I min_left = pool.value(min_element_pool(pool, result.second.second, ReverseIterCmp<Compare>{cmp}));    

    I min2;
    if (pool.is_empty(min_right) && pool.is_empty(min_left)) min2 = last;
    else if (!pool.is_empty(min_right) && !pool.is_empty(min_left)) min2 = cmp(*min_left, *min_right) ? min_left : min_right;
    else if (!pool.is_empty(min_right)) min2 = min_right;
    else min2 = min_left;   
    
    return std::make_pair(result.first, min2);
}





template<typename I, typename Compare>
std::pair<I, I> min2_elements_practical(I first, I last, Compare cmp) {
    if (first == last) return std::make_pair(last, last);
    
    I min = first++;
    if (first == last) return std::make_pair(min, min);

    I min2 = first++;
    if (cmp(*min2, *min)) std::swap(min, min2);

    while (first != last) {
        if (cmp(*first, *min2)) {            
            if (cmp(*first, *min)) {
                min2 = min;
                min = first;
            }
            else min2 = first;               
        }
        ++first;
    }

    return std::make_pair(min, min2);    
}