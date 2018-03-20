/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_OPTIMIZE_HPP
#define DMTK_ALGORITHM_OPTIMIZE_HPP


/**
 * \file Contains optimization utilities for finding an optimum value based on
 *       minimization or maximizing (or alternatively a custom comparator).
 */

DMTK_NAMESPACE_BEGIN

namespace detail {
    /**
     * \brief Iterative Optimize given an algorithm over n runs and find the optimal n argument
     * @tparam Algorithm a functor accepting
     * @tparam Comparator the comparison operator for
     */
    template<typename Algorithm, typename Comparator>
    struct iterative_optimize_helper {

        using algorithm_type = Algorithm;
        using comparator_type = Comparator;

        iterative_optimize_helper(algorithm_type && algorithm, comparator_type&& comp, size_t from_n, size_t to_n)
            : algorithm(std::forward<algorithm_type>(algorithm)),
              comparator(std::forward<comparator_type>(comp)),
              from_n(from_n),
              to_n(to_n) {}

        /**
         * Apply (optional) arguments to the algorithm
         * @param arguments
         * @return the optimized result over n
         */
        auto operator()() {
            size_t opt_index = from_n;
            auto min_val = algorithm(from_n);
            for(size_t i = from_n+1, len = to_n; i < len; ++i) {
                auto temp_val = algorithm(i);
                if(comparator(temp_val, min_val)) {
                    opt_index = i;
                    min_val = temp_val;
                }
            }
            return std::make_tuple(std::move(min_val), opt_index);
        }

        algorithm_type algorithm;
        comparator_type comparator;
        size_t from_n, to_n;
    };
}

/**
 * Optimize the input algorithm based on the comparator (comp) given, over the range [from_n, to_n]
 *
 * @param algorithm the algorithm to optimize. The algorithm must perform it's
 *        operation and then return a value which is comparable by the specified
 *        comparator which will then ultimately be the deciding factor on which
 *        is most optimized to the specified problem.
 * @param from_n the starting range of n
 * @param to_n the ending range of n (exclusive)
 * @param comp the comparison of the result
 * @return a tuple of the optimized value and the n
 */
template<typename Algorithm, typename Comparator>
auto optimize_n(Algorithm&& algorithm, size_t from_n, size_t to_n, Comparator&& comp) {
    return detail::iterative_optimize_helper<Algorithm, Comparator>(
        std::forward<Algorithm>(algorithm),
        std::forward<Comparator>(comp),
        from_n,
        to_n
    )();
}

/**
 * Optimize the input algorithm based on the comparator (comp) given, over the range [0, to_n]
 *
 * @param algorithm the algorithm to optimize. The algorithm must perform it's 
 *        operation and then return a value which is comparable by the specified
 *        comparator which will then ultimately be the deciding factor on which
 *        is most optimized to the specified problem.
 * @param n The number of iterations n (exclusive)
 * @param comp the comparison of the result (default '''std::less<void>''')
 * @return a tuple of the optimized value and the n
 */
template<typename Algorithm, typename Comparator>
auto optimize_n(Algorithm&& algorithm, size_t n, Comparator&& comp) {
    return detail::iterative_optimize_helper<Algorithm, Comparator>(
        std::forward<Algorithm>(algorithm),
        std::forward<Comparator>(comp),
        0,
        n
    )();
}

/**
 * \brief Maximize an algorithm output over n iterations given as argument (of
 *        type size_t) to the algorithm provided.
 *
 * @param algorithm
 * @param n
 * @return a tuple of the maximized value and the n
 */
template<typename Algorithm, typename Comparator = std::greater<void>>
auto maximize(Algorithm&& algorithm, size_t n, Comparator&& comparator = Comparator()) {
    return detail::iterative_optimize_helper<Algorithm, Comparator>(
        std::forward<Algorithm>(algorithm),
        std::forward<Comparator>(comparator),
        0,
        n
    )();
}

/**
 * \brief Minimize an algorithm output over n iterations given as argument (of
 *        type size_t) to the algorithm provided.
 *
 * @param algorithm
 * @param n
 * @return a tuple of the minimized value and the n
 */
template<typename Algorithm, typename Comparator = std::less<void>>
auto minimize(Algorithm&& algorithm, size_t n, Comparator&& comparator = Comparator()) {
    return detail::iterative_optimize_helper<Algorithm, Comparator>(
        std::forward<Algorithm>(algorithm),
        std::forward<Comparator>(comparator),
        0,
        n
    )();
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_OPTIMIZE_HPP */

