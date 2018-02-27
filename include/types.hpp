#ifndef TYPES_HPP
#define TYPES_HPP

/**
 * Checks statically whether or not T is a well formed container, e.g.,
 * std::vector, std::list, etc.
 */
template<typename T, typename Enable>
struct is_container;

template<typename T, typename Enable = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
    T,
    std::conditional_t<
        false,
        std::void_t<
            typename T::value_type,
            typename T::size_type,
            typename T::allocator_type,
            typename T::iterator,
            typename T::const_iterator,
            decltype(std::declval<T>().size()),
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()),
            decltype(std::declval<T>().cbegin()),
            decltype(std::declval<T>().cend())
            >,
        void
        >
    > : std::true_type {};

template<typename T>
constexpr bool is_container_v = is_container<T>::value;


#endif /* TYPES_HPP */

