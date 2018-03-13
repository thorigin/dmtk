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
#include <type_traits>

#include "dmtk/util/tuple_hash.hpp"

/**
 * @file Pairwise cache implementation
 */

DMTK_NAMESPACE_BEGIN

/**
 * @brief This is useful when caching pairs of calues for which (Key1, Key2) is
 *         mapped to the same value of (Key2, Key1)
 */
template<typename T>
struct mapping_reverse_as_equal {
    void operator()(T& lhs, T& rhs) {
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
};

/**
 * @brief Every pair is a uniquely mapped to a different value
 */
template<typename T>
struct mapping_as_unique {
    void operator()(T& lhs, T& rhs) {
        //changing nothing mean pairs are mapped as they were
    }
};

/**
 * @brief Contains definition of a pairwise cache
 *
 * Allows the use of a mapping type such as mapping_as_unique (default) or
 * mapping_reverse_as_equal to safe memory where the direction does not matter
 *
 * Note: Not a container
 */
template<typename K, typename T, typename Generator = void, typename Mapping = mapping_as_unique<K>>
struct pairwise_cache {

    using key_type = K;
    using value_type = T;
    using map_key_type = std::tuple<key_type, key_type>;
    using generator_type = Generator;
    using mapping_type = Mapping;
    using cache_map_type = std::unordered_map<key_type, value_type>;

    pairwise_cache() : cache(128) {}

    pairwise_cache(std::enable_if_t<!std::is_same_v<Generator, void>, Generator> gen) : generator(gen), cache(128) {}

    auto operator()(const key_type& first, const key_type& second) {
        auto& lhs = first;
        auto& rhs = second;
        mapping(lhs, rhs);
        auto res_it = cache.find({lhs, rhs});
        if(res_it == cache.end()) {
            if constexpr(std::is_same_v<generator_type, void>) {
                return res_it;
            } else {
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
            }
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
        mapping(lhs, rhs);
        cache.erase({lhs, rhs});
    }

    void invalidateAll() {
        cache.clear();
    }

    mapping_type mapping;
    generator_type generator;
    cache_map_type cache;

};

/**
 * Makes a piecewise cache instance
 */
template<typename Key, typename Value, typename Generator>
pairwise_cache<Key, Value, Generator> make_pairwise_cache(Generator gen) {
    return {gen};
}

template<typename Key, typename Value, typename Generator = void>
pairwise_cache<Key, Value, Generator> make_pairwise_cache() {
    return {};
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_OPTIMIZE_HPP */

