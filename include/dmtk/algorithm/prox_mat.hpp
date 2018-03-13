/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_DIST_MATRIX_HPP
#define DMTK_ALGORITHM_DIST_MATRIX_HPP

#include <algorithm>
#include <utility>
#include <iomanip>
#include <iosfwd>
#include <iterator>
#include "dmtk/util.hpp"
#include "../element/functions.hpp"

DMTK_NAMESPACE_BEGIN

/**
 * @todo optimize exploit symmetry
 * 
 * @param start
 * @param end
 * @param metric
 */
struct proximity_matrix {

    template<typename Iterator, typename DistanceMetric>
    proximity_matrix(Iterator start, Iterator end, DistanceMetric&& metric)
        : mat(), rows(), cols() {

        auto    top_it = start,
                left_it = start;

        rows = cols = std::distance(start, end);

        mat.reserve(rows * cols);

        for(size_t y = 0; y < rows; ++y) {
            top_it = start;
            for(size_t x = 0; x < cols; ++x) {
                mat[y * rows + x] = metric(*top_it, *left_it);
                ++top_it;
            }
            ++left_it;
        }
    }

    auto operator()(const size_t& x, const size_t& y) {
        return mat[y * cols + x];
    }

    void merge(const size_t& x, const size_t& y) {
        DMTK_UNUSED(x);
        DMTK_UNUSED(y);
    }

    friend std::ostream& operator<<(std::ostream& os, const proximity_matrix& pm) {
        os << " - Proximity Matrix - \n";
        constexpr auto width = 10;
        os << std::setw(width) << 'x';
        for(size_t x = 0; x < pm.cols; ++x) {
            os << std::setw(width) << x;
        }
        os << '\n';
        for(size_t y = 0; y < pm.rows; ++y) {
            os << std::setw(width) << y;
            for(size_t x = 0; x < pm.cols; ++x) {
                os << std::setw(width) << pm.mat[y * pm.rows + x];
            }
            os << '\n';
        }
        os << '\n';
        return os;
    }
    
    std::vector<fp_type> mat;
    size_t rows;
    size_t cols;
};

/**
 * Create a distance matrix map for the given range. The result is an
 * unordered_map with the key of std::pair<value_type, value_type> where
 * value_type the element type of the iterator range. The value is the
 * result of the distance function.
 *
 * Example usage:
 *
 * 
 * @return the distance matrix (as a map of pairs)
 */
template<typename DistanceMetric, typename Iterator>
auto prox_mat(Iterator it, Iterator end, DistanceMetric&& metric = DistanceMetric()) {
    return proximity_matrix(it, end, std::forward<DistanceMetric>(metric));
}

/**
 * Create a distance matrix map for the given range. The result is an
 * unordered_map with the key of std::pair<value_type, value_type> where
 * value_type the element type of the iterator range. The value is the
 * result of the distance function.
 *
 * Example usage:
 *
 *
 * @return the distance matrix (as a map of pairs)
 */
template<typename DistanceMetric, typename Container>
auto prox_mat(Container& cont, DistanceMetric&& metric = DistanceMetric()) {
    return prox_mat(std::begin(cont), std::end(cont), std::forward<DistanceMetric>(metric));
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_DIST_MATRIX_HPP */

