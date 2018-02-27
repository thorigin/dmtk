/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_ENTROPY_HPP
#define DMTK_ALGORITHM_ENTROPY_HPP


DMTK_NAMESPACE_BEGIN

namespace detail {
    /**
     * Very small value (the smallest) of T
     */
    template<typename T>
    const T EPSILON = std::numeric_limits<T>::min();
}

/**
 * Shannon's Entropy (core)
 */
template<typename T>
auto entropy_part(T&& freq) {
    return freq * std::log2<T>(freq + detail::EPSILON<T>);
}

/**
 * Shannon's Entropy
 *
 * @param first
 * @param last
 */
template<typename Iterator>
auto entropy(Iterator first, Iterator last) {
    return std::accumulate(first, last, decltype(*first)(0), [&](const auto& sum, const auto& val) { return sum - entropy_part(val); });
}

/**
 * Shannon's Entropy
 *
 * @param first
 * @param last
 * @param op Unary operator
 */
template<typename Iterator, typename UnaryOperator>
auto entropy(Iterator first, Iterator last, UnaryOperator op) {
    return std::accumulate(first, last, decltype(op(*first))(0), [&](const auto& sum, const auto& val) { return sum -(entropy_part(op(val)));});
}

/**
 * Shannon's Entropy
 * @param cont A container
 */
template<typename Container>
auto entropy(Container&& cont) {
    return entropy(std::begin(cont), std::end(cont));
}

/**
 * Shannon's Entropy gain
 */
//template<typename T>
//auto entropy_gain(T entropy_a, T entropy_d) {
//    return entropy(cont.begin(), cont.end());
//}


DMTK_NAMESPACE_END


#endif /* DMTK_ALGORITHM_ENTROPY_HPP */

