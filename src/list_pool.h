#pragma once
#include <vector>
#include <cstddef>

template<typename T, typename N = std::size_t>
// Requires T is semi-regular
// Requires N is integral type
class list_pool
{
public:
    typedef N list_type;
    typedef T value_type;

private:
    struct node_t
    {
        T value;
        list_type next;
    };

    std::vector<node_t> pool;
    list_type free_list;

    node_t& node(list_type x) {
        return pool[x - 1];
    }

    const node_t& node(list_type x) const {
        return pool[x - 1];
    }

    list_type new_list() {
        pool.push_back(node_t());
        return list_type(pool.size());
    }

public:    
    list_type empty() const {
        return list_type(0);
    }
    
    bool is_empty(list_type x) const {
        return x == empty();
    }    

    list_pool() {
        free_list = empty();
    }

    T& value(list_type x) {       
        return node(x).value;
    }
    
    const T& value(list_type x) const {
        return node(x).value;
    }

    list_type& next(list_type x) {
        return node(x).next;
    }

    const list_type& next(list_type x) const {
        return node(x).next;
    }

    list_type free(list_type x) {
        list_type cdr = next(x);
        next(x) = free_list;  
        free_list = x;
        return cdr;
    }

    list_type allocate(const T& val, list_type tail) {
        list_type list = free_list;
        if (is_empty(free_list)) 
            list = new_list();
        else 
            free_list = next(free_list);        
        value(list) = val;
        next(list) = tail;
        return list;
    }


    struct iterator
    {
        typedef typename list_pool::value_type value_type;
        typedef typename list_pool::list_type differnece_type;
        typedef std::forward_iterator_tag iterator_category;
        typedef value_type& reference;
        typedef value_type* pointer;

        list_pool* pool;
        list_pool::list_type node;

        iterator() {} // creates a partially formed object
        iterator(list_pool& p, list_pool::list_type node) : pool{ &p }, node{ node } {}
        iterator(list_pool& p) : pool{ &p }, node{ p.empty() } {}
        
        reference operator*() const {
            return pool->value(node);
        }

        pointer operator->() const {
            return &**this;
        }

        iterator& operator++() {            
            node = pool->next(node);
            return *this;
        }

        iterator operator++(int) {
            iterator current(*this);
            ++(*this);
            return current;
        }

        friend
        bool operator==(const iterator& x, const iterator& y) {
            // assert(x.pool == y.pool)
            return x.node == y.node;
        }
        
        friend
        bool operator!=(const iterator& x, const iterator& y) {
            return !(x == y);
        }

        // extends the interface to Linked Iterator:

        friend
        void set_successor(iterator x, iterator y) {
            // assert(x.p == y.p)
            x.pool->next(x.node) = y.node;
        }

        // extend the interface to Singly Linked List Iterator

        friend
        void push_front(iterator& x, const T& value) {
            x.node = x.pool->allocate(value, x.node);
        }

        friend
        void push_back(iterator& x, const T& value) {
            typename list_pool::list_type tmp = x.pool->allocate(value, x.pool->next(x.node));
            x.pool->next(x.node) = tmp;
        }
    };


    iterator begin(list_type x) {
        return { *this, x };
    }

    iterator end(list_type x) {
        return { *this };
    }
};


template<typename T, typename N = std::size_t>
void free_list(list_pool<T, N>& pool, 
               typename list_pool<T, N>::list_type x) {
    while (!pool.is_empty(x)) {
        x = pool.free(x);
    }
}
