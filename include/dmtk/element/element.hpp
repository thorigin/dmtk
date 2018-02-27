/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ELEMENT_ELEMENT_HPP
#define DMTK_ELEMENT_ELEMENT_HPP

#include <tuple>
#include <type_traits>
#include <vector>

#include "dmtk/config.hpp"
#include "dmtk/types.hpp"


DMTK_NAMESPACE_BEGIN

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

/**
 * Contains instruction about
 */
template<size_t Start = 0, size_t End = 0>
struct element_padding {
    constexpr static size_t start = Start;
    constexpr static size_t end = End;
};

/**
 * Element padding none type
 */
using element_padding_none = element_padding<0, 0>;


/**
 * Atom tag
 */
struct atom_tag {};

/**
 * Arithmetic atom tag
 */
struct arithmetic_atom_tag : atom_tag {};

/**
 * Skip atom tag
 */
struct skip_atom_tag : atom_tag {};

/**
 * Compile time predicate for checking if argument should
 * be skipped
 */
template<typename T>
constexpr bool should_skip_atom_v = !std::is_arithmetic<std::decay_t<T>>::value;

/**
 * Provides for compile time selection on whether or not a type is arithmetic
 * for tag dispatching
 */
template<typename T>
using select_atom_tag_t = std::conditional_t<
    should_skip_atom_v<T>,
    skip_atom_tag,
    arithmetic_atom_tag
>;

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

DMTK_NAMESPACE_END

#endif /* DMTK_ELEMENT_ELEMENT_HPP */

