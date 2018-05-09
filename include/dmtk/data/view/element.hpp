/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_DATA_VIEW_ELEMENT_HPP
#define DMTK_DATA_VIEW_ELEMENT_HPP

#include <tuple>
#include <vector>
#include <utility>

#include "dmtk/config.hpp"

/**
 * @file Provides for pseudo element views, i.e. the ability to slice and
 *       manipulate the view of imported data.
 */

DMTK_NAMESPACE_BEGIN

namespace detail {

    template<typename ... T, size_t Index>
    std::tuple_element_t<Index, std::tuple<T...>>& element_view_helper(std::tuple<T...>& tuple, std::index_sequence<Index>) {
        return std::get<Index>(tuple);
    }

    template<typename ... T, size_t FirstIndex, size_t SecondIndex, size_t ... RestIndexes>
    auto element_view_helper(std::tuple<T...>& tuple, std::index_sequence<FirstIndex, SecondIndex, RestIndexes...>) {
        return std::tie(std::get<FirstIndex>(tuple), std::get<SecondIndex>(tuple), std::get<RestIndexes>(tuple)...);
    }

    template<typename ... T, size_t Index>
    auto element_copy_helper(const std::tuple<T...>& tuple, std::index_sequence<Index>) {
        return std::get<Index>(tuple);
    }

    template<typename ... T, size_t FirstIndex, size_t SecondIndex, size_t ... RestIndexes>
    auto element_copy_helper(const std::tuple<T...>& tuple, std::index_sequence<FirstIndex, SecondIndex, RestIndexes...>) {
        return std::make_tuple(std::get<FirstIndex>(tuple), std::get<SecondIndex>(tuple), std::get<RestIndexes>(tuple)...);
    }

}

/**
 * @brief Accepts a pseudo element and creates a view of the element given the
 *        specified selected indexes
 */
template<size_t ... Indexes, typename Element>
auto element_view(Element& ele) -> decltype(detail::element_view_helper(ele, std::index_sequence<Indexes...>{})) {
    return detail::element_view_helper(ele, std::index_sequence<Indexes...>{});
}


/**
 * @brief Accepts a pseudo element and creates a copy of the element given the
 *        specified selected indexes
 */
template<size_t ... Indexes, typename Element>
auto element_copy(const Element& ele) {
    return detail::element_copy_helper(ele, std::index_sequence<Indexes...>{});
}

/**
 * @brief Template element_view result type given a specified set of indexes
 *        and element type
 */
template<typename Element, size_t ... Indexes>
struct element_view_result {
    using type = decltype(element_view<Indexes...>(std::declval<Element&>()));
};

/**
 * @brief Template element_copy result type given a specified set of indexes
 *        and element type
 */
template<typename Element, size_t ... Indexes>
struct element_copy_result {
    using type = decltype(element_copy<Indexes...>(std::declval<Element&>()));
};

/**
 * @brief Alias for the element_view result type given a specified set of indexes
 *        and element type
 */
template<typename Element, size_t ... Indexes>
using element_view_result_t = typename element_view_result<Element, Indexes...>::type;

/**
 * @brief Alias for the element_copy result type given a specified set of indexes
 *        and element type
 */
template<typename Element, size_t ... Indexes>
using element_copy_result_t = typename element_copy_result<Element, Indexes...>::type;

/**
 * @brief Alias for the element_copy result type given a specified set of indexes
 *        and element type
 */
template<typename Element, size_t ... Indexes>
using element_copy_result_t = typename element_copy_result<Element, Indexes...>::type;


/**
 * @brief Accepts a pseudo element and creates a view of the element given the
 *        specified selected indexes
 */
template<typename Element, size_t ... Indexes>
struct element_view_f {

    using result_type = element_view_result_t<Element, Indexes...>;
    
    result_type operator()(Element& ele) const {
        return element_view<Indexes...>(ele);
    }
};

DMTK_NAMESPACE_END

#endif /* DMTK_DATA_VIEW_ELEMENT_HPP */

