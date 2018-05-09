/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_DATA_VIEW_CONTAINER_HPP
#define DMTK_DATA_VIEW_CONTAINER_HPP

#include <tuple>
#include <vector>
#include <utility>
#include <iterator>
#include <type_traits>

#include <boost/iterator/transform_iterator.hpp>

#include "dmtk/config.hpp"
#include "element.hpp"


/**
 * @file Provides facilities to select and manipulating data by narrowing the
 *       data to a set of specific attributes.
 */

DMTK_NAMESPACE_BEGIN

namespace detail {

    /**
     * @brief View container transforming iterator alias to boot transform iterator
     */
    template<typename Iterator, size_t ... Indexes>
    using view_container_transforming_iterator_t = boost::transform_iterator<
        element_view_f<
            typename std::iterator_traits<Iterator>::value_type,
            Indexes...
        >,
        Iterator
    >;
}

/**
 * @brief View container provides basic abstraction over a container which can
 *        be sliced in such a way to hide attributes of elements.
 */
template<typename Container, size_t ... Indexes>
struct view_container {

    constexpr static bool is_singleton = sizeof...(Indexes) == 1;

    using inner_value_type = typename Container::value_type;
    using inner_iterator =typename Container::iterator;
    using iterator = detail::view_container_transforming_iterator_t<inner_iterator, Indexes...>;
    using const_iterator =  detail::view_container_transforming_iterator_t<inner_iterator, Indexes...>;
    using value_type = element_view_result_t<inner_value_type, Indexes...>;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;
    using transformation_func_type = element_view_f<inner_value_type, Indexes...>;
    
    view_container(Container& cont) : ref(cont) {}

    view_container(const view_container& ) = default;
    view_container(view_container&& ) = default;
    view_container& operator=(const view_container&) = default;
    view_container& operator=(view_container&&) = default;


    auto operator[](const size_t& index) {
        return element_view<Indexes...>(ref[index]);
    }

    auto operator[](const size_t& index) const {
        return element_view<Indexes...>(ref[index]);
    }

    auto begin() {
        return boost::make_transform_iterator(ref.begin(), transformation_func_type{});
    }

    auto end() {
        return boost::make_transform_iterator(ref.end(), transformation_func_type{});
    }

    auto begin() const {
        return boost::make_transform_iterator(ref.begin(), transformation_func_type{});
    }

    auto end() const {
        return boost::make_transform_iterator(ref.end(), transformation_func_type{});
    }

    auto size() {
        return ref.size();
    }


    auto size() const {
        return ref.size();
    }

    private:
        Container& ref;
};

template<size_t ... Indexes, typename Container>
auto view(Container& cont) {
    static_assert(sizeof...(Indexes) > 0, "At least one or more index must be specified for view");
    return view_container<Container, Indexes...>{cont};
}

template<size_t ... Indexes, typename Container>
auto copy(const Container& cont) {
    static_assert(sizeof...(Indexes) > 0, "At least one or more index must be specified for copy");
    std::vector<element_copy_result_t<typename Container::value_type, Indexes...>> vec;
    vec.reserve(cont.size());
    for(auto&& v : cont) {
        vec.emplace_back(element_copy<Indexes...>(v));
    }
    return vec;
}

/**
 * Maps a container by a UnaryFunction to a container
 * @param cont
 * @param func
 * @return
 */
template<typename Container, typename UnaryFunction>
auto map(const Container& cont, UnaryFunction func) {
    using result_type = decltype(func(std::declval<typename Container::value_type&>()));
    std::vector<result_type> vec;
    vec.reserve(cont.size());
    for(auto&& v : cont) {
        vec.emplace_back(func(v));
    }
    return vec;
}



DMTK_NAMESPACE_END

#endif /* DMTK_DATA_VIEW_CONTAINER_HPP */

