/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ELEMENT_FUNCTION_DISTANCE_HPP
#define DMTK_ELEMENT_FUNCTION_DISTANCE_HPP

#include <cmath>
#include <tuple>
#include <vector>
#include <utility>
#include <iterator>

#include "dmtk/config.hpp"

DMTK_NAMESPACE_BEGIN


namespace detail {

    template<typename ... T, size_t ... Indexes>
    auto euclidean_distance_helper(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2, std::index_sequence<Indexes...>) {
        return (0.0f + ... + ((std::get<Indexes>(tuple1) - std::get<Indexes>(tuple2)) * (std::get<Indexes>(tuple1) - std::get<Indexes>(tuple2))));
    }
}

/**
 * Euclidean distance function that calculates the distance of values in a two
 * tuples. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @return the distance
 */
template<typename ...T>
auto euclidean_distance(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2){
    return std::sqrt(
        detail::euclidean_distance_helper(tuple1, tuple2, std::index_sequence_for<T...>{})
    );
}

/**
 * Euclidean distance function that calculates the distance squared of values in a two
 * tuples. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @return the distance squared
 */
template<typename ...T>
auto euclidean_distance_squared(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2){
    return detail::euclidean_distance_helper(tuple1, tuple2, std::index_sequence_for<T...>{});
}

/**
 * Euclidean distance function that calculates the distance of values in a two
 * vectors. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @return the distance
 */
template<typename Container>
auto euclidean_distance(const Container& cont1, const Container& cont2){
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

/**
 * Euclidean distance function that calculates the distance of values squared in a two
 * vectors. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @return the distance squared
 */
template<typename Container>
auto euclidean_distance_squared(const Container& cont1, const Container& cont2){
    typename Container::value_type sum = 0;
    for(auto
            it_1 = std::begin(cont1),
            it_1_end = std::end(cont1),
            it_2 = std::begin(cont2);
            it_1  != it_1_end;
            ++it_1, ++it_2) {
        sum += (*it_1 - *it_2) * (*it_1 - *it_2);
    }
    return sum;
}


DMTK_NAMESPACE_END

#endif /* DMTK_ELEMENT_FUNCTION_DISTANCE_HPP */

