/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */


#ifndef DMTK_UTIL_TYPES_HPP
#define DMTK_UTIL_TYPES_HPP

#include <type_traits>
#include <tuple>
#include <vector>

DMTK_NAMESPACE_BEGIN

/**
 * Checks statically whether or not T is a well form tuple
 */
template<typename T, typename Enable>
struct is_tuple;

template<typename T, typename Enable = void>
struct is_tuple : std::false_type {};

template<typename ... T>
struct is_tuple<std::tuple<T...>> : std::true_type {};

/**
 * @brief Checks statically whether or not T is a well formed container, e.g.,
 * std::vector, std::list, etc.
 */
template<typename T, typename Enable>
struct is_container;

template<typename T, typename Enable = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
    T,
    std::void_t<
        typename T::value_type,
        typename T::size_type,
        typename T::allocator_type,
        typename T::iterator,
        typename T::const_iterator,
        decltype(std::declval<T>().size()),
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        decltype(std::declval<T>().cbegin()),
        decltype(std::declval<T>().cend())
    >
> : std::true_type {};

/**
 * @brief A loser definition of a container where only a begin and end function 
 *        are required
 */
template<typename T, typename Enable>
struct is_range;

template<typename T, typename Enable = void>
struct is_range : std::false_type {};

template<typename T>
struct is_range<
    T,
    std::void_t<
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end())
    >
> : std::true_type {};

template<typename T>
constexpr bool is_tuple_v = is_tuple<T>::value;

template<typename T>
constexpr bool is_container_v = is_container<T>::value;

template<typename T>
constexpr bool is_range_v = is_range<T>::value;

template<typename T, typename Result = void>
using enable_if_container_t = std::enable_if_t<is_container_v<T>, Result>;

template<typename T, typename Result = void>
using enable_if_range_t = std::enable_if_t<is_range_v<T>, Result>;

template<typename T, typename Result = void>
using enable_if_tuple_t = std::enable_if_t<is_tuple_v<T>, Result>;


DMTK_NAMESPACE_END

#endif /* DMTK_UTIL_TYPES_HPP */

