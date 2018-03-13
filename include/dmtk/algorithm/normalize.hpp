/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_NORMALIZE_HPP
#define DMTK_ALGORITHM_NORMALIZE_HPP

DMTK_NAMESPACE_BEGIN

namespace detail {

    template<typename T>
    auto apply_normalization_atom(T x, const T& min, const T& max) {
        auto range = max-min;
        if(range != 0) {
            x = (x - min) / range;
        } else if(max != 0) {
            x =  x - min / max;
        }
    }

    template<typename T>
    void apply_normalization(T& x, const T& min, const T& max, arithmetic_atom_tag) {
        apply_normalization_atom(x, min, max);
    }

    template<typename T>
    void apply_normalization(T&, const T&, const T&, skip_atom_tag) {}

    template<typename T>
    void apply_normalization(T& x, const T& min, const T& max) {
        apply_normalization(x, min, max, select_atom_tag_t<T>{});
    }

    template<size_t SkipLast, typename T, typename ... Rest>
    void apply_normalization(std::vector<T, Rest...>& value, const T& min, const T& max) {
        for(auto it = value.begin(), end = value.end() - SkipLast; it != end; ++it) {
            *it = apply_normalization(*it, min, max);
        }
    }

    template<typename ...T, size_t ... Indexes>
    void apply_normalization_helper(std::tuple<T...>& tuple, const std::tuple<T...>& min, const std::tuple<T...>& max, std::index_sequence<Indexes...>) {
        ((
            std::get<Indexes>(tuple) = apply_normalization(
                std::get<Indexes>(tuple),
                std::get<Indexes>(min),
                std::get<Indexes>(max)
            )
        ), ...);
    }

    template<size_t SkipLastN, typename ...T>
    void apply_normalization(std::tuple<T...>& tuple, const std::tuple<T...>& min, const std::tuple<T...>& max) {
        apply_normalization_helper(tuple, min, max, std::make_index_sequence<sizeof...(T)-SkipLastN>{});
    }

}

/**
 * Minmax normalization of values
 *
 * @param cont
 * @tparam skip_last optional, if value type is a vector or a tuple, it skips the number of elements from the right
 * @return
 */
template<typename Iterator>
void normalize(Iterator start, Iterator end) {
    if(start != end) {
        auto it = start;
        auto min = (*it++),
             max = (*it++);
        for(; it != end; ++it) {
            using namespace detail;
            find_min_max(*it, min, max);
        }

        for(auto nit = start; nit != end; ++nit) {
            auto&& x = *nit;
            using namespace detail;
            apply_normalization(x, min, max);
        }
    }
}


/**
 * Range based normalization of values
 * It is able to handle tuples, vector
 * @param cont
 * @return
 */
template<typename Container>
void normalize(Container& cont) {
    normalize(std::begin(cont), std::end(cont));
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_NORMALIZE_HPP */

