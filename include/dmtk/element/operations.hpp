/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ELEMENT_OPERATIONS_HPP
#define DMTK_ELEMENT_OPERATIONS_HPP

#include <tuple>
#include <type_traits>
#include <vector>

DMTK_NAMESPACE_BEGIN

/**
 * @file Provides for basic arithmetic operations on the pseudo type Element
 * The implementation is incomplete and only provides the basic operations used
 */

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

template<typename ...T>
void element_add(std::tuple<T...>& value, const std::tuple<T...>& add) {
    detail::element_add_helper(value, add, std::make_index_sequence<sizeof...(T)>{});
}

template<typename Container, typename std::enable_if_t<is_container_v<Container>>>
void element_add(const Container& value, Container& add) {

    //ensure size of sum container
    if(value.size() < value.size()) {
        value.resize(add.size());
    }

    for(auto    value_it = std::begin(add),
                end = std::end(add),
                add_it = add.begin();
                value_it != end;
                ++value_it, ++add_it) {
        (*value_it) += *add_it;
    }
}


template<typename ...T>
void element_div(std::tuple<T...>& value, const std::tuple<T...>& divider) {
    detail::element_add_helper(value, divider, std::index_sequence_for<T...>{});
}

template<typename Container>
void element_div(Container& value, const Container& divider) {
    for(auto    value_it = std::begin(value),
                end = std::end(value),
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

template<typename ...T, typename Divider>
void element_div(std::tuple<T...>& value, const Divider& divider) {
    detail::element_div_helper(value, divider, std::index_sequence_for<T...>{});
}

template<typename Container, typename Divider>
void element_div(Container& value, const Divider& divider) {
    for(auto    value_it = std::begin(value),
                end = std::end(value);
                value_it != end;
                ++value_it) {
        (*value_it) /= divider;
    }
}

DMTK_NAMESPACE_END


#endif /* DMTK_ELEMENT_OPERATIONST_HPP */

