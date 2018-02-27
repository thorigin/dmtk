#ifndef ELEMENT_HPP
#define ELEMENT_HPPh

#include <tuple>
#include <type_traits>
#include <vector>
#include "types.hpp"

/**
 * @file Provides pseudo type Element which is used throughout algorithm header
 * Element can be a tuple or a vector, or a custom data type which defines the following operations
 * This header file provides basic operations on Element
 */

/**
 * typedef for tuple
 */
template<typename ... T>
using node = std::tuple<T...>;

struct atom_tag {};

struct arithmetic_atom_tag : atom_tag {};

struct skip_atom_tag : atom_tag {};

template<typename T>
constexpr bool should_skip_atom_v = std::is_arithmetic<std::decay_t<T>>::value;

/**
 * Provides for compile time selection on whether or not a type is arithmetic
 */
template<typename T>
using select_atom_tag_t = std::conditional_t<
    should_skip_atom_v<T>,
    arithmetic_atom_tag,
    skip_atom_tag
>;


template<typename T>
void find_min_max_atom(const T& value, T& min, T& max) {
    find_min_max_atom(value, min, max, select_atom_tag_t<T>{});
}

template<typename T>
void find_min_max_atom(const T&, T&, T&, skip_atom_tag) {}

template<typename T>
void find_min_max_atom(const T& value, T& min, T& max, arithmetic_atom_tag) {
    if(value > max) {
        max = value;
    }
    if(value < min) {
        min = value;
    }
}

template<typename T>
void find_min_max(const T& value, T& min, T& max) {
    find_min_max_atom(value, min, max);
}

namespace detail {

    template<typename ... T, size_t ... Indexes>
    void find_min_max_tuple(const std::tuple<T...>& value, std::tuple<T...>& min, std::tuple<T...>& max, std::index_sequence<Indexes...>) {
        (find_min_max_atom(std::get<Indexes>(value), std::get<Indexes>(min), std::get<Indexes>(max)), ...);
    }

    template<typename T, typename ... Rest>
    void find_min_max_vector(const std::vector<T, Rest...>& value, std::vector<T, Rest...>& min, std::vector<T, Rest...>& max) {
        for(    auto it = value.begin(), end = value.end(),
                min_it = min.begin(), min_end = min.end(),
                max_it = max.begin(), max_end = max.end();
                it != end && min_it != min_end && max_it != max_end;
                ++it,
                ++min_it,
                ++max_it
                ) {
            find_min_max_atom(*it, *min_it, *max_it);
        }
    }
}

/**
 * find_min_max overload for tuple
 *
 * @param value
 * @param min
 * @param max
 */
template<size_t SkipLastN, typename ... T>
void find_min_max(const std::tuple<T...>& value, std::tuple<T...>& min, std::tuple<T...>& max) {
    detail::find_min_max_tuple(value, min, max, std::make_index_sequence<sizeof...(T) - SkipLastN>{});
}


namespace detail {

    template<typename ... T, size_t ... Indexes, typename Result = float>
    Result distance_euclidean_tuple(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2, std::index_sequence<Indexes...>) {
        return (0.0f + ... + ((std::get<Indexes>(tuple1) - std::get<Indexes>(tuple2)) * (std::get<Indexes>(tuple1) - std::get<Indexes>(tuple2))));
    }
}

/**
 * Euclidean distance function that calculates the distance of values in a two
 * tuples. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @param SkipLastN Skip the last number of elements in the tuple
 * @return the distance
 */
template<size_t SkipLastN = 0, typename ...T>
auto distance_euclidean(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2){
    return detail::distance_euclidean_tuple(tuple1, tuple2, std::make_index_sequence<sizeof...(T) - SkipLastN>{});
}

/**
 * Euclidean distance function that calculates the distance of values in a two
 * vectors. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @return the distance
 */
template<size_t SkipLastN = 0, typename Container>
auto distance_euclidean(const Container& cont1, const Container& cont2){
    typename Container::value_type sum = 0;
    for(auto
            it_1 = std::begin(cont1),
            it_1_end = std::end(cont1),
            it_2 = std::begin(cont2);
            it_1  != it_1_end;
            ++it_1, ++it_2) {
        sum += (*it_1 - *it_2) * (*it_1 - *it_2);
    }
    return sqrt(sum);
}

template<typename T>
void element_add_atom(T& value, const T& add) {
    element_add_atom(value, add, select_atom_tag_t<T>{});
}

template<typename T>
void element_add_atom(T& value, const T& add, arithmetic_atom_tag) {
    value += add;
}

template<typename T>
void element_add_atom(T& value, const T& add, skip_atom_tag) {}

namespace detail {
    template<typename ...T, size_t ... Indexes>
    void element_add_helper(std::tuple<T...>& value, const std::tuple<T...>& add, std::index_sequence<Indexes...>) {
        ((std::get<Indexes>(value) += std::get<Indexes>(add)), ...);
    }
}

template<size_t SkipLastN = 0, typename ...T>
void element_add(std::tuple<T...>& value, const std::tuple<T...>& add) {
    detail::element_add_helper(value, add, std::make_index_sequence<sizeof...(T)-SkipLastN>{});
}

template<size_t SkipLastN = 0, typename Container, typename std::enable_if_t<is_container_v<Container>>>
void element_add(const Container& value, Container& add) {

    //ensure size of sum container
    if(value.size() < value.size()) {
        value.resize(add.size());
    }

    for(auto    value_it = std::begin(add),
                end = std::end(add)-SkipLastN,
                add_it = add.begin();
                value_it != end;
                ++value_it, ++add_it) {
        (*value_it) += *add_it;
    }
}


template<size_t SkipLastN = 0, typename ...T>
void element_div(std::tuple<T...>& value, const std::tuple<T...>& divider) {
    detail::element_add_helper(value, divider, std::make_index_sequence<sizeof...(T)-SkipLastN>{});
}

template<size_t SkipLastN = 0, typename Container>
void element_div(Container& value, const Container& divider) {
    for(auto    value_it = std::begin(value),
                end = std::end(value)-SkipLastN,
                divider_it = divider.begin();
                value_it != end;
                ++value_it, ++divider_it) {
        (*value_it) /= divider_it;
    }
}

namespace detail {
    template<typename ...T, size_t ... Indexes, typename Divider>
    void element_div_helper(std::tuple<T...>& value, const Divider& divider, std::index_sequence<Indexes...>) {
        ((std::get<Indexes>(value) /= divider), ...);
    }
}

template<size_t SkipLastN = 0, typename ...T, typename Divider>
void element_div(std::tuple<T...>& value, const Divider& divider) {
    detail::element_div_helper(value, divider, std::make_index_sequence<sizeof...(T)-SkipLastN>{});
}

template<size_t SkipLastN = 0, typename Container, typename Divider>
void element_div(Container& value, const Divider& divider) {
    for(auto    value_it = std::begin(value),
                end = std::end(value)-SkipLastN;
                value_it != end;
                ++value_it) {
        (*value_it) /= divider;
    }
}

/**
 * By convention, the label is always the last value in a tuple
 * @param tuple
 * @return a reference to the label
 */
template<typename ...T>
auto& get_label(std::tuple<T...>& tuple) {
    return std::get<sizeof...(T)-1>(tuple);
}

/**
 * By convention, the label is always the last value in a tuple
 * @param tuple
 * @return a reference to the label
 */
template<typename ...T>
const auto& get_label(const std::tuple<T...>& tuple) {
    return std::get<sizeof...(T)-1>(tuple);
}

/**
 * By convention, the label is always the last value in a Container
 * @param cont the input container
 * @return a reference to the label
 */
template<typename Container>
auto& get_label(Container& cont) {
    return *(cont.begin() + cont.size()-1);
}


/**
 * By convention, the label is always the last value in a Container
 * @param cont the input container
 * @return a reference to the label
 */
template<typename Container>
const auto& get_label(const Container& cont) {
    return *(cont.begin() + cont.size()-1);
}



#endif /* ELEMENT_HPP */

