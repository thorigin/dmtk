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

DMTK_NAMESPACE_BEGIN

namespace detail {
    /**
     * Optimize given an algorithm over n runs and find the optimal n argument
     * @tparam Algorithm a functor accepting
     * @tparam Comparator the comparison operator for
     */
    template<typename Algorithm, typename Comparator>
    struct optimizer_helper {

        using algorithm_type = Algorithm;
        using comparator_type = Comparator;

        optimizer_helper(algorithm_type && algorithm, comparator_type&& comp, size_t from_n, size_t to_n)
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
            size_t min_idx = from_n;
            auto min_val = algorithm(from_n);
            for(size_t i = from_n+1, len = to_n; i < len; ++i) {
                auto temp_val = algorithm(i);
                if(comparator(temp_val, min_val)) {
                    min_idx = i;
                    min_val = temp_val;
                }
            }
            return std::make_tuple(std::move(min_val), min_idx);
        }

        algorithm_type algorithm;
        comparator_type comparator;
        size_t from_n, to_n;
    };
}

/**
 * Optimize the input algorithm based on the comparator (comp) given, over the range [from_n, to_n]
 *
 * @param algorithm the algorithm to optimize
 * @param comp the comparison of the result
 * @param from_n the starting range of n
 * @param to_n the ending range of n (exclusive)
 * @return the most optimal value
 */
template<typename Algorithm, typename Comparator>
auto optimizer(Algorithm&& algorithm, Comparator&& comp, size_t from_n, size_t to_n) {
    return detail::optimizer_helper<Algorithm, Comparator>(
        std::forward<Algorithm>(algorithm),
        std::forward<Comparator>(comp),
        from_n,
        to_n
    )();
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_OPTIMIZE_HPP */

