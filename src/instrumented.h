#pragma once
#include <utility>

struct instrumented_base {
    enum operations {
        n,
        construction,
        default_constructor,
        destructor,
        copy_constructor,
        move_constructor,
        copy_assignment,
        move_assignment,
        equality,
        comparison        
    };
    static const size_t number_ops = 10;
    static const char* counter_names[number_ops];
    static double counts[number_ops];
    static void initialize(size_t);
}; 

template<typename T>
// T can be Simiregular, Regular or Totally Ordered
struct instrumented : instrumented_base {
    typedef T value_type;

    T value;

    // Conversions to and from T
    explicit instrumented(T x) : value{ std::move(x) } { ++counts[construction];  }

    explicit operator T() const {
        return value;
    }

    // Semiregular
    instrumented() { ++counts[default_constructor]; }

    ~instrumented() { ++counts[destructor];  }

    instrumented(const instrumented& x) : value{ x.value } { ++counts[copy_constructor]; }

    instrumented(instrumented&& x) noexcept : value{ std::move(x.value) } { ++counts[move_constructor]; }

    instrumented& operator=(const instrumented& x) {
        ++counts[copy_assignment];
        value = x.value;
        return *this;
    }

    instrumented& operator=(instrumented&& x) noexcept {
        ++counts[move_assignment];
        value = std::move(x.value);
        return *this;
    }

    // Regular
    friend bool operator==(const instrumented& x, const instrumented& y) {
        ++counts[equality];
        return x.value == y.value;
    }
    friend bool operator!=(const instrumented& x, const instrumented& y) {
        return !(x == y);
    }

    // Totally Ordered
    friend bool operator<(const instrumented& x, const instrumented& y) {
        ++counts[comparison];
        return x.value < y.value;
    }
    friend bool operator>(const instrumented& x, const instrumented& y) {
        return y < x;
    }
    friend bool operator<=(const instrumented& x, const instrumented& y) {
        return !(y < x);
    }
    friend bool operator>=(const instrumented& x, const instrumented& y) {
        return !(x < y);
    }
};
