/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */


#ifndef DMTK_UTIL_TUPLE_HASH_HPP
#define DMTK_UTIL_TUPLE_HASH_HPP

#include <utility>
#include <boost/functional/hash.hpp>


DMTK_NAMESPACE_BEGIN

namespace detail {

    /**
     * Helper function to apply boost::hash_combine on all elements of a tuple
     */
    template <typename ... T, size_t ... Indexes>
    void hash_tuple_helper(size_t& seed, const std::tuple<T...>& tuple, std::index_sequence<Indexes...>) {
        ((boost::hash_combine(seed, std::get<Indexes>(tuple))),...);
    }
}

DMTK_NAMESPACE_END
namespace std{
    
    /**
     * @brief Provides specialization for all tuple types (but not the value
     *        types which a tuple is composed of) such that tuples may be used
     *        for hashing purposes.
     * @see dmtk::detail::hash_tuple_helper
     */
    template <typename ... T>
    struct hash<std::tuple<T...>> {
        size_t operator()(const std::tuple<T...>& tuple) const
        {
            size_t seed = 0;
            dmtk::detail::hash_tuple_helper(seed, tuple, std::index_sequence_for<T...>{});
            return seed;
        }
    };
}


#endif /* DMTK_UTIL_TUPLE_HASH_HPP */

