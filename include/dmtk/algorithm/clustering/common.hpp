/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

/**
 * @file Defines common type aliases and utilities used by clustering algorithms
 */

#ifndef DMTK_ALGORITHM_CLUSTERING_COMMON_HPP
#define DMTK_ALGORITHM_CLUSTERING_COMMON_HPP

#include <unordered_map>
#include "dmtk/config.hpp"

DMTK_NAMESPACE_BEGIN

/**
 * @brief Cluster container, a common data type used for storing the result of
 *        clustering algorithms.
 */
template<typename T>
using cluster = std::vector<T>;

/**
 * @brief Cluster container, a common data type used for storing the result of
 *        clustering algorithms.
 *
 * It stores, in a vector, a vector of all elements T that belong to that cluster
 */
template<typename T>
using clusters_vec = std::vector<cluster<T>>;

/**
 * @brief Cluster container result type, a tuple of a cluster_container given
 *        a rating value of type R, typically Sum of Errors Squared based
 *        on a metric such as euclidean distance.
 */
template<typename T, typename R>
using cluster_score = std::tuple<clusters_vec<T>, R>;

/**
 * @brief Cluster container result container, contains a vector of cluster
 *        container scores.
 */
template<typename T, typename R>
using cluster_score_vec = std::vector<cluster_score<T, R>>;

/**
 * @brief Clustering scores,, a map of a vector of
 *        cluster_score_vec.
 *
 * The key is the K value selected, and the value is an instance of
 * cluster_container_result_container
 */
template<typename T, typename R>
using sampling_scores = std::unordered_map<size_t, cluster_score_vec<T, R>>;

/**
 * @brief Clustering container traits helper class
 */
template<typename Container, typename Result = fp_type>
struct clustering_traits {
    /**
     * @brief The value type of the container
     */
    using value_type = typename Container::value_type;

    /**
     * @brief The result type of sampling metric
     */
    using result_type = Result;

    /**
     * @brief Cluster container, a common data type used for storing the result of
     *        clustering algorithms.
     */
    using cluster = std::vector<value_type>;

    /**
     * @brief Cluster container, a common data type used for storing the result of
     *        clustering algorithms.
     *
     * It stores, in a vector, a vector of all elements T that belong to that cluster
     */
    using clusters_vec = std::vector<cluster>;

    /**
     * @brief Cluster container result type, a tuple of a cluster_container given
     *        a rating value of type R, typically Sum of Errors Squared based
     *        on a metric such as euclidean distance.
     */
    using cluster_score = std::tuple<clusters_vec, result_type>;

    /**
     * @brief Cluster container result container, contains a vector of cluster
     *        container scores
     */
    using cluster_score_vec = std::vector<cluster_score>;

    /**
     * @brief Clustering scores,, a map of a vector of
     *        cluster_score_vec.
     *
     * The key is the K value selected, and the value is an instance of
     * cluster_container_result_container
     */
    using sampling_scores = std::unordered_map<size_t, cluster_score_vec>;
};

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_CLUSTERING_COMMON_HPP */

