/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_CACHE_PAIRWISE_HPP
#define DMTK_ALGORITHM_CACHE_PAIRWISE_HPP

#include <tuple>
#include <unordered_map>
#include <functional>

#include "dmtk/util/tuple_hash.hpp"

/**
 * @file Pairwise cache implementation
 */

DMTK_NAMESPACE_BEGIN

/**
 * @brief Contains definition of a pairwise cache
 *
 * This is useful when caching pairs of calues for which
 * (Key1, Key2) is mapped to the same value of (Key2, Key1)
 *
 * Note: Not a container
 */
template<typename K, typename T, typename Generator>
struct pairwise_cache {

    using key_type = K;
    using value_type = T;
    using map_key_type = std::tuple<key_type, key_type>;
    using generator_type = Generator;
    using cache_map_type = std::unordered_map<key_type, value_type>;

    pairwise_cache(Generator gen) : generator(gen), cache(128) {}
    
    auto operator()(const key_type& first, const key_type& second) {

        auto& lhs = first;
        auto& rhs = second;
        check_swap(lhs, rhs);
        auto res_it = cache.find({lhs, rhs});
        if(res_it == cache.end()) {
            return cache.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(
                    first,
                    second
                ),
                std::forward_as_tuple(
                    generator_type(
                        lhs,
                        rhs
                    )
                )
            ).first;
        } else {
            return res_it;
        }
    }

    /**
     * Manually emplace a pairwise entry into the cache
     */
    auto emplace(key_type&& first_key, key_type&& second_key, value_type&& value) {
        return cache.emplace(
            std::piecewise_construct,
            std::forward<key_type>(first_key, second_key),
            std::forward<value_type>(value)
        ).first;
    }

    void invalidate(const key_type& first, const key_type& second) {
        auto& lhs = first;
        auto& rhs = second;
        check_swap(lhs, rhs);
        cache.erase({lhs, rhs});
    }

    void invalidateAll() {
        cache.clear();
    }
    
private:
    template<typename LHS, typename RHS>
    void check_swap(LHS& lhs, RHS& rhs) {
        /**
         * Check if first index is greater than second, if so, swap
         * reduce memory usage by half if calls are not optimized.
         * if the distance between left and right is negative,
         * then the right side is after the left
         */
        if(lhs->first > rhs->first) {
            std::swap(lhs, rhs);
        }
    }

    generator_type generator;
    cache_map_type cache;

};

/**
 * Makes a piecewise cache instance
 */
template<typename PairKey, typename Value, typename Generator>
pairwise_cache<PairKey, Value, Generator> make_piecewise_cache(Generator gen) {
    return {gen};
}

        
DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_OPTIMIZE_HPP */

