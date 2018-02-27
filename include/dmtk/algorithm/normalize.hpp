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
            return (x - min) / range;
        } else if(max != 0) {
            return x - min / max;
        } else {
            return x;
        }
    }

    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max, arithmetic_atom_tag) {
        return apply_normalization_atom(x, min, max);
    }

    template<typename T>
    auto apply_normalization(T& x, const T&, const T&, skip_atom_tag) {
        return x;
    }

    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max) {
        return apply_normalization(x, min, max, select_atom_tag_t<T>{});
    }

    template<size_t SkipLast, typename T, typename ... Rest>
    auto& apply_normalization(std::vector<T, Rest...>& value, const T& min, const T& max) {
        for(auto it = value.begin(), end = value.end() - SkipLast; it != end; ++it) {
            *it = apply_normalization(*it, min, max);
        }
        return value;
    }

    template<typename ...T, size_t ... Indexes>
    auto& apply_normalization_helper(std::tuple<T...>& tuple, const std::tuple<T...>& min, const std::tuple<T...>& max, std::index_sequence<Indexes...>) {
        ((
            std::get<Indexes>(tuple) = apply_normalization(
                std::get<Indexes>(tuple),
                std::get<Indexes>(min),
                std::get<Indexes>(max)
            )
        ), ...);
        return tuple;
    }

    template<size_t SkipLastN, typename ...T>
    auto& apply_normalization(std::tuple<T...>& tuple, const std::tuple<T...>& min, const std::tuple<T...>& max) {
        return apply_normalization_helper(tuple, min, max, std::make_index_sequence<sizeof...(T)-SkipLastN>{});
    }

}

/**
 * Minmax normalization of values
 *
 * @param cont
 * @tparam skip_last optional, if value type is a vector or a tuple, it skips the number of elements from the right
 * @return
 */
template<size_t SkipLastN = 0, typename Iterator>
void normalize(Iterator start, Iterator end) {
    if(start != end) {
        auto it = start;
        auto min = (*it++),
             max = (*it++);
        for(; it != end; ++it) {
            using namespace detail;
            find_min_max<SkipLastN>(*it, min, max);
        }

        std::transform(start, end, start, [&](auto& x) {
            using namespace detail;
            return apply_normalization<SkipLastN>(x, min, max);
        });
    }
}


/**
 * Range based normalization of values
 * It is able to handle tuples, vector
 * @param cont
 * @return
 */
template<size_t skip_last = 0, typename Container>
void normalize(Container& cont) {
    normalize<skip_last>(std::begin(cont), std::end(cont));
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_NORMALIZE_HPP */

