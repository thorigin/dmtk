/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_AGGREGATE_HPP
#define DMTK_ALGORITHM_AGGREGATE_HPP

#include <algorithm>
#include <utility>
#include <iterator>

#include "../element/functions.hpp"

DMTK_NAMESPACE_BEGIN

/**
 * Returns a tuple of the iterator to he minimum element and its index
 *
 * @param cont the container to perform the scanning for
 * @param op
 * @return {Element, index}
 */
template<typename Container, typename Comparator>
auto min(const Container& cont, Comparator&& comp = std::less<void>()) {
    auto res = std::min_element(std::begin(cont), std::end(cont), std::forward<Comparator>(comp));
    return std::forward_as_tuple(res, std::distance(std::begin(cont), res));
}

/**
 * Returns a tuple of an iterator to the maximum element and its index
 * @param cont the container to perform the scanning for
 * @param op
 * @return {Element, index}
 */
template<typename Container, typename Comparator>
auto max(const Container& cont, Comparator&& comp = std::greater<void>()) {
    auto res = std::min_element(std::begin(cont), std::end(cont), std::forward<Comparator>(comp));
    return std::forward_as_tuple(res, std::distance(std::begin(cont), res));
}

/**
 * Returns the sum of all the output of the UnaryOp in the container provided
 * @param cont
 * @param op
 * @return the sum
 */
template<typename Container, typename UnaryOp>
auto sum(const Container& cont, UnaryOp&& op) {
    using res_type = decltype(op(*cont.begin()));
    res_type sum = 0;
    for(auto& v : cont) {
        sum += op(v);
    }
    return sum;
}


/**
 * Returns the sum of all the value in the provided container
 * @param cont
 * @return the sum
 */
template<typename Container>
auto sum(const Container& cont) {
    return sum(cont, [](auto& x) { return x; });
}

/**
 * Calculate the average value of the container
 *
 * @param cont
 * @return the average
 */
template<typename Container>
auto avg(const Container& cont) {
    return avg(cont, [](auto& x) { return x; });
}

/**
 * Calculate the average value of the UnaryOp result in a container
 *
 * @param cont
 * @param op
 * @return the average
 */
template<typename Container, typename UnaryOp>
auto avg(const Container& cont, UnaryOp&& op) {
    using res_type = decltype(op(*cont.begin()));
    res_type sum = 0;
    for(auto& v : cont) {
        sum += op(v);
    }
    return sum / cont.size();
}

/**
 *
 * @param cont the container to calculate the centroid for
 * @return
 */
template<size_t SkipLastN = 0, typename Container>
auto centroid(Container& cont) {
    using value_type = typename Container::value_type;
    value_type center;
    auto size = std::distance(std::begin(cont), std::end(cont));
    using namespace detail;
    for(auto& val : cont) {
        element_add<SkipLastN>(center, val);
    }
    element_div<SkipLastN>(center, size);
    return center;
}


/**
 * Calculates the sum of distance (euclidean) between all the points in the cointainer
 * and the element provided
 *
 * If the element provided exists in cont, its weight will be considered
 * @param cont
 * @param element
 * @return the average
 */
template<size_t SkipLastN = 0, typename Container, typename Result = float>
Result sum_distance_euclidean(const Container& cont, const typename Container::value_type& element) {
    auto it = std::begin(cont),
         end = std::end(cont);
    if(it != end) {
        Result sum = 0;
        for(; it != end; ++it) {
            sum += distance_euclidean<SkipLastN>(*it, element);
        }
        return sum;
    } else {
        return 0;
    }
}

/**
 * Calculates the average distance (euclidean) between all the points in the cointainer
 * and the element provided
 *
 * If the element provided exists in cont, its weight will be considered
 * @param cont
 * @param element
 * @return the average
 */
template<size_t SkipLastN = 0, typename Container, typename Result = float>
Result average_distance_euclidean(const Container& cont, const typename Container::value_type& element) {
    auto it = std::begin(cont),
         end = std::end(cont);
    if(it != end) {
        Result sum = 0;
        for(; it != end; ++it) {
            sum += distance_euclidean<SkipLastN>(*it, element);
        }
        return static_cast<Result>(sum) / static_cast<Result>(cont.size());
    } else {
        return 0;
    }
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_AGGREGATE_HPP */

