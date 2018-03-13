/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_CLUSTERING_METRIC_HPP
#define DMTK_ALGORITHM_CLUSTERING_METRIC_HPP

#include <type_traits>
#include <functional>
#include "dmtk/element/function/euclidean.hpp"
#include "dmtk/util/tuple_hash.hpp"

DMTK_NAMESPACE_BEGIN

enum euclidean_distance_metric_type {
    min,
    max
};

/**
 * @brief Euclidean min distance metric (MIN or MAX)
 *
 * Finds the MIN or MAX distance between cluster X and Y such that
 * D(X, Y) = min_{x in X, y in X} d(x, y)
 */
template<euclidean_distance_metric_type type = euclidean_distance_metric_type::min>
struct euclidean_distance_metric {

    std::conditional_t<type == min, std::less<void>, std::greater<void>> comparator;

    template<typename Cluster>
    auto operator()(const Cluster& a, const Cluster& b) {
        static_assert(is_container_v<Cluster>, "Arguments are containers");
        fp_type best = type == min ? std::numeric_limits<fp_type>::max() : std::numeric_limits<fp_type>::min();        
        for(auto ait = a.begin(), a_end = a.end(); ait != a_end; ++ait) {
            for(auto bit = b.begin(), b_end = b.end(); bit != b_end; ++bit) {
                auto tmp = euclidean_distance_squared(*ait, *bit);                
                if(comparator(tmp, best)) {
                    best = tmp;
                }
            }
        }
        return best;
    }   

};

/**
 * @brief Euclidean Min Distance Metric (alias)
 *
 * @see euclidean_distance_metric
 */
using euclidean_min_distance_metric = euclidean_distance_metric<min>;

/**
 * @brief Euclidean Max Distance Metric (alias)
 *
 * @see euclidean_distance_metric
 */
using euclidean_max_distance_metric = euclidean_distance_metric<max>;

/**
 * @brief Euclidean Centroid Metric (MIN or MAX)
 *
 * Finds the euclidean distance between two clusters
 */
struct euclidean_centroid_metric {

    template<typename Cluster>
    auto operator()(const Cluster& a, const Cluster& b) {
        return euclidean_distance_squared(centroid(a), centroid(b));
    }

};

/**
 * @brief Group Average Metric
 *
 * Finds the euclidean average of all points of two clusters
 */
struct euclidean_group_average_metric {

    template<typename Cluster>
    auto operator()(const Cluster& a, const Cluster& b) {
        static_assert(is_container_v<Cluster>, "Arguments are containers");
        fp_type sum = 0;
        fp_type count = a.size() * b.size();
        for(auto ait = a.begin(), a_end = a.end(); ait != a_end; ++ait) {
            for(auto bit = b.begin(), b_end = b.end(); bit != b_end; ++bit) {
                sum += euclidean_distance(*ait, *bit);
            }
        }
        fp_type average_group = sum / count;
        return average_group;
    }

};

/**
 * @brief Default metric function
 */
using default_metric_function = euclidean_min_distance_metric;


DMTK_NAMESPACE_END


#endif /* DMTK_ALGORITHM_CLUSTERING_METRIC_HPP */

