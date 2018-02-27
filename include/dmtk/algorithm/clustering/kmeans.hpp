/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */


#ifndef DMTK_ALGORITHM_CLUSTERING_KMEANS_HPP
#define DMTK_ALGORITHM_CLUSTERING_KMEANS_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <utility>
#include "dmtk/util/preprocessor.hpp"

DMTK_NAMESPACE_BEGIN

/**
 * Constructs k clusters from selection into a tuple of cluster map a d cluster centroids from the container cont
 *
 * @param cont
 * @param selection The initially selected element that are the seeds to clustering
 * @param k
 * @param cluster_map
 */
template<size_t SkipLastN = 0, typename FromContainer, typename SelectionContainer, typename Random = std::mt19937>
auto create_k_means_clusters_from_selection(FromContainer& cont, SelectionContainer&& selection) {

    const auto samples_count = cont.size();
    const auto k_clusters = selection.size();
    using value_type = typename FromContainer::value_type;
    using vec_type = std::vector<value_type>;

    /**
     * Initialize centroids, and used_samples containers
     */
    std::unordered_map<size_t, vec_type> cluster_map(k_clusters);
    std::unordered_map<size_t, value_type> cluster_centroid_map(k_clusters);
    std::unordered_set<size_t> used_samples(samples_count);

    if(selection.empty()) {
        throw std::runtime_error("Invalid cluster initialization");
    }

    /**
     * Select random n initial seeds and add them into the k_clusters map
     */
    size_t k_cluster_idx = 0;
    for(auto& [origin, origin_idx] : selection) {
       vec_type vec;
       /**
        * Estimate average cluster size as samples * 0.75 / k
        */
       vec.reserve(samples_count * 0.75f / k_clusters);
       /**
        * Move the initial element (the seed) into it own cluster
        */
       vec.emplace_back(origin);
       /**
        * Initialize cluster centroid
        */
       cluster_centroid_map[k_cluster_idx] = centroid<SkipLastN>(vec);
       /**
        * Move temp vec to cluster_map after initialization
        */
       cluster_map.emplace(k_cluster_idx++, std::move(vec));
       used_samples.emplace(origin_idx);
    }

    /**
     * Iterate over all samples, excluding the K samples used for seeding the
     * initial K clusters, and add them to the closest cluster by the clusters'
     * mean
     */
    for(size_t sample_idx = 0; sample_idx < samples_count; ++sample_idx) {

        /**
         * Check if sample is already used
         */
        if(used_samples.find(sample_idx) == used_samples.end()) {

            auto& sample = cont[sample_idx];

            size_t closest_cluster_idx = 0;
            auto closest_cluster_dist = distance_euclidean<SkipLastN>(cluster_centroid_map[closest_cluster_idx], sample);
            for(size_t k_check_idx = 1; k_check_idx < k_clusters; ++k_check_idx) {
                auto& cluster_centroid = cluster_centroid_map[k_check_idx];

                /**
                 * Calculate the euclidean distance between the cluster and the sample
                 */
                auto temp_dist = distance_euclidean_squared<SkipLastN>(cluster_centroid, sample);
                if(temp_dist < closest_cluster_dist) {
                    closest_cluster_dist = temp_dist;
                    closest_cluster_idx = k_check_idx;
                }
            }
            /**
             * Add element to the closest cluster
             */
            used_samples.emplace(sample_idx);
            auto& cluster_container = cluster_map[closest_cluster_idx];
            cluster_container.emplace_back(sample);
            /**
             * Recalculate cluster centroid
             */
            cluster_centroid_map[closest_cluster_idx] = centroid<SkipLastN>(cluster_container);
        }
    }

    return std::make_tuple(std::move(cluster_map), std::move(cluster_centroid_map));
}

/**
 * Constructs k clusters into cluster_map from the container cont
 * @param cont
 * @param k
 * @param cluster_map
 */
template<size_t SkipLastN = 0, typename FromContainer, typename Random = std::mt19937>
auto create_k_means_clusters(FromContainer& cont, const size_t& k_clusters, Random&& rand = Random(std::random_device{}())) {
    return create_k_means_clusters_from_selection<SkipLastN>(cont, select_n_random(cont, k_clusters, rand), rand);
}


/**
 * Rebalance K-means clusters
 * @param cont
 * @param test_runs
 * @param max_k_test
 * @param rand
 * @return
 */
template<size_t SkipLastN = 0, typename Container, typename ClustersContainer, typename CentroidsContainer>
auto rebalance_k_means_clusters(Container& cont, ClustersContainer& clusters, CentroidsContainer& centroids) {

    using cluster_mapped_type_const_iterator_type = typename ClustersContainer::mapped_type::const_iterator;

    /**
     * Repeat reassignment until no movements are possible, i.e. coverage is achieved
     */
    bool was_moved = true;

    while(was_moved) {
        /**
         * Initialize was_moved to false on each iteration
         */
        was_moved = false;

        /**
         * Iterate over every sample in the provided container, attempt to relocate
         * if necessary
         */
        auto clusters_end = clusters.end();
        for(auto& sample : cont) {

            /**
             * Find the cluster the sample resides in and store the index of the sample in the vector
             */
            cluster_mapped_type_const_iterator_type sample_cluster_value_it;
            auto sample_cluster_it = std::find_if(clusters.begin(), clusters.end(), [&](const auto& cluster_entry) {
                auto sample_it = std::find(cluster_entry.second.begin(), cluster_entry.second.end(), sample);
                if(sample_it != cluster_entry.second.end()) {
                    sample_cluster_value_it = sample_it;
                    return true;
                }
                return false;
            });

            /**
             * Validate that the sample exists in one of the clusters
             */
            if(sample_cluster_it != clusters_end) {

                auto& [sample_cluster_idx, sample_cluster_vec] = (*sample_cluster_it);

                /**
                 * Find the closest cluster (by mean) to sample
                 */
                auto clusters_it = clusters.begin();
                auto min_cluster_it = clusters_it;
                auto min_dist = average_distance_euclidean<SkipLastN>((*(clusters_it++)).second, sample);

                for(;clusters_it != clusters_end; ++clusters_it) {
                    auto& [k_idx, cluster_it_vec] = *clusters_it;
                    DMTK_UNUSED(k_idx);
                    auto temp_dist = average_distance_euclidean<SkipLastN>(cluster_it_vec, sample);
                    if(temp_dist < min_dist) {
                        min_dist = temp_dist;
                        min_cluster_it = clusters_it;
                    }
                }

                auto& [ min_cluster_idx, min_cluster_vec] = *min_cluster_it;

                /**
                 * If the sample is further away from its current cluster,
                 * then reassign clusters
                 */
                auto sample_dist = average_distance_euclidean<SkipLastN>(sample_cluster_vec, sample);
                if(sample_dist > min_dist) {
                    was_moved = true;
                    sample_cluster_vec.erase(sample_cluster_value_it);
                    min_cluster_vec.emplace_back(sample);

                    /**
                     * Update centroids for assigned/unassigned clusters
                     */
                    centroids[sample_cluster_idx] = centroid<SkipLastN>(sample_cluster_vec);
                    centroids[min_cluster_idx] = centroid<SkipLastN>(min_cluster_vec);
                }

            } else {
                throw std::runtime_error("Sample in container provided does not exist in any of the clusters");
            }
        }
    }

}
/**
 * Sample all possible K values [min_k_test, max_k_test) for test_runs number of test runs
 *
 * @param cont The input container
 * @param test_runs
 * @param max_k_test the maximum number of clusters to test (defaults to the number of data points in the container provided
 * @return the result map
 */
template<size_t SkipLastN = 0, typename Container, typename Random = std::mt19937>
auto sample_kmeans(Container& cont, size_t test_runs = 1, size_t min_k_test = 1, size_t max_k_test = std::numeric_limits<size_t>::max(), Random&& rand = Random(std::random_device{}())) {

    using value_type = typename Container::value_type;
    const auto samples_count = cont.size();
    const auto k_clusters_max = std::min(max_k_test, samples_count+1);

    /**
     * distance from each element to its cluster centroid
     * i.e. what is being minimized (the average)
     */
    //std::map<size_t, std::vector<float>> test_results;

    /**
     * tuple with key that is the elements, and value is a vector of SSE values of each cluster
     */
    using selection_info = std::tuple<std::vector<value_type>, std::vector<float>>;
    /**
     * list of tuples {k = elements vector, v = average}
     */
    using selection_vector = std::vector<selection_info>;

    /**
     * map of {k = cluster count, v = vector of selection_info}
     */
    std::unordered_map<size_t, selection_vector> selection_proximity(k_clusters_max);

    /**
     * For every k clusters, starting with k = 1 clusters, and increment and measure
     */
    for(size_t k_clusters = min_k_test; k_clusters < k_clusters_max; ++k_clusters) {
        
        for(size_t i = 0; i < test_runs; ++i) {

            auto selection = select_n_random(cont, k_clusters, rand);
            std::vector<value_type> selection_values;
            selection_values.reserve(k_clusters);
            for(auto& [k, v] : selection) {
                DMTK_UNUSED(v);
                selection_values.emplace_back(k);
            }

            /**
             * Create the initial k clusters from the random selection above
             */
            auto [clusters, centroids] = create_k_means_clusters_from_selection<SkipLastN>(cont, selection);

            /**
             * Rebalance the k-means clusters by iterating over all elements
             * in the input container and checking if there is a cluster which
             * has a mean that is closer than the currently assigned cluster. If
             * one exists, swap clusters and repeat until no such clusters are
             * found.
             */
            rebalance_k_means_clusters<SkipLastN>(cont, clusters, centroids);

            /**
             * Calculate the sum of squares error for all clusters
             */
            std::vector<float> sum_of_squares_errors;
            std::for_each(clusters.begin(), clusters.end(), [&](const auto& pair) {
                const auto& [idx, vec] = pair;
                auto sum_dist = sum_distance_euclidean<SkipLastN>(vec, centroids[idx]);
                sum_of_squares_errors.emplace_back(sum_dist * sum_dist);
            });

            selection_proximity[k_clusters].emplace_back(selection_values, sum_of_squares_errors);
        }
    }

    return selection_proximity;
}


/**
 * Tests various kmeans values by splitting a input container into two parts and sampling the statistical success rate of it
 *
 * @param test_runs
 * @param max_k_test the maximum number of clusters to test (defaults to the number of data points in the container provided
 * @return the result map
 */
template<size_t SkipLastN = 0, typename Container, typename Random = std::mt19937>
auto minimize_kmeans(Container& cont, size_t test_runs = 1, size_t max_k_test = std::numeric_limits<size_t>::max()) {
    auto res = sample_kmeans(cont, test_runs,  max_k_test);
    auto res_it = std::max_element(res.begin(), res.end(), [](const auto& left, const auto& right) {
        return std::get<1>(left) > std::get<1>(right);
    });
    return *res_it;
}


DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_CLUSTERING_KMEANS_HPP */

