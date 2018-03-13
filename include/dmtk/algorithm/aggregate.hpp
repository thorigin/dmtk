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
struct min {
    template<typename Container, typename Comparator>
    auto operator()(const Container& cont, Comparator&& comp = std::less<void>()) {
        auto res = std::min_element(std::begin(cont), std::end(cont), std::forward<Comparator>(comp));
        return std::forward_as_tuple(res, std::distance(std::begin(cont), res));
    }
};

/**
 * Returns a tuple of an iterator to the maximum element and its index
 * @param cont the container to perform the scanning for
 * @param op
 * @return {Element, index}
 */
struct max {
    template<typename Container, typename Comparator>
    auto operator()(const Container& cont, Comparator&& comp = std::greater<void>()) {
        auto res = std::min_element(std::begin(cont), std::end(cont), std::forward<Comparator>(comp));
        return std::forward_as_tuple(res, std::distance(std::begin(cont), res));
    }
};

/**
 * Returns the sum of all the output of the UnaryOp in the container provided
 * @param cont
 * @param op (optional) Unary operator which is applied to every element in the input container
 * @return the sum
 */
struct sum {

    template<typename Iteratator, typename UnaryOp>
    auto operator()(Iteratator start, Iteratator end, UnaryOp&& op) {
        using res_type = decltype(op(*start));
        res_type sum = 0;
        for(auto it = start; it != end; ++it) {
            sum += op(*it);
        }
        return sum;
    }

    template<typename Container, typename UnaryOp>
    auto operator()(const Container& cont, UnaryOp&& op) {
        return (*this)(std::begin(cont), std::end(cont), op);
    }
    
    template<typename Container>
    auto operator()(const Container& cont) {
        return sum(cont, [](auto& x) { return x; });
    }

    template<typename Iteratator, typename UnaryOp>
    auto operator()(Iteratator start, Iteratator end) {
        return (*this)(start, end, [](auto& x) { return x; });
    }

};


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
template<typename Container>
auto centroid(Container& cont) {
    using value_type = typename Container::value_type;
    value_type center;
    auto size = std::distance(std::begin(cont), std::end(cont));
    using namespace detail;
    for(auto& val : cont) {
        element_add(center, val);
    }
    element_div(center, size);
    return center;
}


/**
 *
 * @param cont the container to calculate the centroid for
 * @return
 */
template<typename Container>
struct centroid_f {

    using result_type = typename Container::value_type;

    auto operator()(Container& cont) {
        return centroid(cont);
    }
};


/**
 * Calculates the sum of distance (euclidean) between all the points in the cointainer
 * and the element provided
 *
 * If the element provided exists in cont, its weight will be considered
 * @param cont
 * @param element
 * @return the average
 */
template<typename Container, typename Result = float>
Result sum_squared(const Container& cont, const typename Container::value_type& element) {
    auto it = std::begin(cont),
         end = std::end(cont);
    if(it != end) {
        Result sum = 0;
        for(; it != end; ++it) {
            sum += euclidean_distance(*it, element);
        }
        return sum;
    } else {
        return 0;
    }
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
template<typename Container, typename Result = float>
Result sum_distance_euclidean(const Container& cont, const typename Container::value_type& element) {
    auto it = std::begin(cont),
         end = std::end(cont);
    if(it != end) {
        Result sum = 0;
        for(; it != end; ++it) {
            sum += euclidean_distance(*it, element);
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
template<typename Container, typename Result = float>
Result average_distance_euclidean(const Container& cont, const typename Container::value_type& element) {
    auto it = std::begin(cont),
         end = std::end(cont);
    if(it != end) {
        Result sum = 0;
        for(; it != end; ++it) {
            sum += euclidean_distance(*it, element);
        }
        return static_cast<Result>(sum) / static_cast<Result>(cont.size());
    } else {
        return 0;
    }
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_AGGREGATE_HPP */

