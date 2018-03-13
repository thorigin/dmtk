/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_FOR_EACH_HPP
#define DMTK_ALGORITHM_FOR_EACH_HPP

#include <functional>
#include <tuple>

#include "dmtk/util/types.hpp"


DMTK_NAMESPACE_BEGIN

namespace detail {

    template<typename ... T, typename UnaryFunction, size_t ... Indexes>
    void for_each_tuple(std::tuple<T...>& tuple, UnaryFunction func, std::index_sequence<Indexes...>) {
        ((func(std::get<Indexes>(tuple))), ...);
    }
}


/**
 * @brief Apply UnaryFunction  (func) to every element in the range of [start, end).
 */
template<typename Iterator, typename UnaryFunction>
void for_each(Iterator start, Iterator end, UnaryFunction func) {
    std::for_each(start, end, func);
}

/**
 * @brief Apply UnaryFunction (func) to every element in the range specified
 */
template<typename Range, typename UnaryFunction>
enable_if_range_t<Range>
for_each(Range& range, UnaryFunction&& func) {
    std::for_each(std::begin(range), std::end(range), func);
}

/**
 * @brief Apply UnaryFunction (func) to every element in the tuple
 */
template<typename ... T, typename UnaryFunction>
void
for_each(std::tuple<T...>& tuple, UnaryFunction&& func) {
    detail::for_each_tuple(tuple, func, std::index_sequence_for<T...>{});
}


DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_OPTIMIZE_HPP */

