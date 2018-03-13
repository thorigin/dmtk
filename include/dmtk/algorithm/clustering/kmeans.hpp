///**
// * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
// * Unauthorized copying of this file, via any medium is strictly prohibited
// * Proprietary and confidential
// *
// * Written by
// *    Omar Thor <omar@thorigin.com>, 2018
// *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
// */
//
//#ifndef DMTK_ALGORITHM_CLUSTERING_KMEANS_HPP
//#define DMTK_ALGORITHM_CLUSTERING_KMEANS_HPP
//
//#include <vector>
//#include <unordered_map>
//#include <unordered_set>
//#include <tuple>
//#include <utility>
//#include "common.hpp"
//#include <iostream>
//
//#include "dmtk/util/preprocessor.hpp"
//
//DMTK_NAMESPACE_BEGIN
//
///**
// * Constructs k clusters from selection into a tuple of cluster map a d cluster centroids from the container cont
// *
// * @param cont
// * @param selection The initially selected element that are the seeds to clustering
// * @param k
// * @param cluster_map
// */
//template<typename FromContainer, typename SelectionContainer, typename MetricFunction, typename Random = std::mt19937>
//auto create_k_means_clusters_from_selection(FromContainer& cont, SelectionContainer&& selection, MetricFunction&& metric) {
//
//    const auto samples_count = cont.size();
//    const auto k_clusters = selection.size();
//
//
//    using cluster_traits = clustering_traits<FromContainer>;
//
//
//    /**
//     * Initialize centroids, and used_samples containers
//     */
//    typename cluster_traits::clusters_vec clusters_cont;
//    clusters_cont.reserve(k_clusters);
//    std::vector<typename cluster_traits::value_type> cluster_centroids;
//    clusters_cont.reserve(k_clusters);
//    std::unordered_set<size_t> used_samples(samples_count);
//
//    if(selection.empty()) {
//        throw std::runtime_error("Invalid cluster initialization");
//    }
//
//    /**
//     * Select random n initial seeds and add them into the k_clusters map
//     */
//    for(auto& [origin, origin_idx] : selection) {
//
//       typename cluster_traits::cluster cl;
//
//       /**
//        * Estimate average cluster size as samples * 0.75 / k
//        */
//       cl.reserve(samples_count * 0.75f / k_clusters);
//       /**
//        * Copy the initial element (the seed) into it own cluster
//        */
//       cl.emplace_back(origin);
//       /**
//        * Initialize cluster centroid
//        */
//       cluster_centroids.emplace_back(centroid(cl));
//       /**
//        * Move temp vec to cluster_map after initialization
//        */
//       clusters_cont.emplace_back(std::move(cl));
//       used_samples.emplace(origin_idx);
//    }
//
//    /**
//     * Iterate over all samples, excluding the K samples used for seeding the
//     * initial K clusters, and add them to the closest cluster by the clusters'
//     * mean
//     */
//    for(size_t sample_idx = 0; sample_idx < samples_count; ++sample_idx) {
//
//        /**
//         * Check if sample is already used
//         */
//        if(used_samples.find(sample_idx) == used_samples.end()) {
//
//            auto& sample = cont[sample_idx];
//
//            /**
//             * Find the closest cluster
//             */
//            size_t closest_cluster_idx = 0;
//            auto closest_cluster_dist = metric(cluster_centroids[closest_cluster_idx], sample);
//            for(size_t k_check_idx = 1; k_check_idx < k_clusters; ++k_check_idx) {
//
//                /**
//                 * The
//                 */
//                auto& centroid = cluster_centroids[k_check_idx];
//
//                /**
//                 * Calculate the euclidean distance between the cluster and the sample
//                 */
//                auto temp_dist = euclidean_distance_squared(centroid, sample);
//                if(temp_dist < closest_cluster_dist) {
//                    closest_cluster_dist = temp_dist;
//                    closest_cluster_idx = k_check_idx;
//                }
//            }
//
//            /**
//             * Add element to the closest cluster
//             */
//            used_samples.emplace(sample_idx);
//            auto& cluster_container = clusters_cont[closest_cluster_idx];
//            cluster_container.emplace_back(sample);
//
//            /**
//             * Recalculate cluster centroid
//             */
//            cluster_centroids[closest_cluster_idx] = centroid(cluster_container);
//        }
//    }
//
//    return std::make_tuple(std::move(clusters_cont), std::move(cluster_centroids));
//}
//
///**
// * Constructs k clusters into cluster_map from the container cont
// * @param cont
// * @param k
// * @param cluster_map
// */
//template<typename FromContainer, typename Random = std::mt19937>
//auto create_k_means_clusters(FromContainer& cont, const size_t& k_clusters, Random&& rand = Random(std::random_device{}())) {
//    return create_k_means_clusters_from_selection(cont, select_n_random(cont, k_clusters, rand), rand);
//}
//
//
///**
// * Rebalance K-means clusters
// * @param cont
// * @param test_runs
// * @param max_k_test
// * @param rand
// * @return
// */
//template<typename Container, typename Clusters, typename Centroids>
//auto rebalance_k_means_clusters(Container& cont, Clusters& clusters, Centroids& centroids, const size_t& max_iterations = 1000) {
//
//    const auto& k_clusters = std::distance(clusters.begin(), clusters.end());
//    if(k_clusters == 0) {
//        throw std::runtime_error("K clusters must be greater or equal to 1");
//    }
//    using clusters_value_type_iterator = typename Clusters::value_type::const_iterator;
//
//    if(clusters.size() != centroids.size()) {
//        throw std::runtime_error("Centroids and clusters dimension do not match");
//    }
//    /**
//     * Repeat reassignment until no movements are possible, i.e. coverage is achieved
//     */
//    bool was_moved = true;
//    size_t iterations = 0;
//    while(was_moved && iterations < max_iterations) {
//        /**
//         * Initialize was_moved to false on each iteration
//         */
//        was_moved = false;
//
//        /**
//         * Iterate over every sample in the provided container, attempt to relocate
//         * if necessary
//         */
//        auto clusters_end = clusters.end();
//        for(auto& sample : cont) {
//
//            /**
//             * Find the cluster the sample resides in and store the index of the sample in the vector
//             */
//            clusters_value_type_iterator sample_it;
//            auto cluster_of_sample_it = std::find_if(clusters.begin(), clusters_end, [&](const auto& cluster_entry) {
//                auto it = std::find(cluster_entry.begin(), cluster_entry.end(), sample);
//                if(sample_it != cluster_entry.end()) {
//                    sample_it = it;
//                    return true;
//                }
//                return false;
//            });
//
//
//            /**
//             * Validate that the sample exists in one of the clusters
//             */
//            if(cluster_of_sample_it != clusters_end) {
//
//                const auto cluster_of_sample_idx = std::distance(clusters.begin(), cluster_of_sample_it);
//
//                /**
//                 * Find the closest cluster (by mean) to sample
//                 */
//                auto clusters_it = clusters.begin();
//                auto closest_cluster_it = clusters_it;
//
//                /**
//                 * @TODO Use distance matrix
//                 */
//                auto closest_cluster_dist = average_distance_euclidean(*(clusters_it++), sample);
//
//                for(;clusters_it != clusters_end; ++clusters_it) {
//                    auto& cluster_it_vec = *clusters_it;
//                    /**
//                     * @TODO Use distance matrix
//                     */
//                    auto temp_dist = average_distance_euclidean(cluster_it_vec, sample);
//                    if(temp_dist < closest_cluster_dist) {
//                        closest_cluster_dist = temp_dist;
//                        closest_cluster_it = clusters_it;
//                    }
//                }
//
//                const auto closest_cluster_idx = std::distance(clusters.begin(), closest_cluster_it);
//
//                /**
//                 * calculate the distance of the sample to other samples in the
//                 * sample's assigned cluster, and compare it to the closest
//                 * cluster distance
//                 */
//                /**
//                 * @TODO Use distance matrix
//                 */
//                auto samples_assigned_cluster_dist = average_distance_euclidean(*cluster_of_sample_it, sample);
//                if(samples_assigned_cluster_dist > closest_cluster_dist) {
//
//                    /**
//                     * Mark state as was_moved, so that the loop will repeat
//                     * once more (until limit is hit)
//                     */
//                    was_moved = true;
//
//                    /**
//                     * erase remove sample from assigned cluster
//                     */
//                    (*cluster_of_sample_it).erase(
//                        std::remove(
//                            (*cluster_of_sample_it).begin(),
//                            (*cluster_of_sample_it).end(),
//                            *sample_it
//                        ),
//                        (*cluster_of_sample_it).end()
//                    );
//
//                    /**
//                     * Add sample into the new min cluster found
//                     */
//                    (*closest_cluster_it).emplace_back(sample);
//
//                    /**
//                     * Update centroids for previously assigned and
//                     * new assigned cluster
//                     */
//                    centroids[cluster_of_sample_idx] = centroid(*cluster_of_sample_it);
//                    centroids[closest_cluster_idx] = centroid(*closest_cluster_it);
//
//                }
//
//            } else {
//                std::cout << "The sample count is: " << cont.size() << ", cluster count: " << clusters.size() << ", centroids: " << centroids.size() <<  std::endl;
//                //std::cout << "The sample was: " << std::get<0>(*sample_it) << std::endl;
//                throw std::runtime_error("Sample in container provided does not exist in any of the clusters");
//            }
//            ++iterations;
//        }
//    }
//
//}
//
///**
// * Sample all possible K values [min_k_test, max_k_test) for test_runs number of test runs
// *
// * @param cont The input container
// * @param test_runs
// * @param max_k_test the maximum number of clusters to test (defaults to the number of data points in the container provided
// * @return the result map
// */
//template<typename Container, typename MetricFunction = default_metric_function, typename Random = std::mt19937>
//auto sample_kmeans(
//        Container& cont,
//        size_t test_runs = 1,
//        size_t min_k_test = 1,
//        size_t max_k_test = std::numeric_limits<size_t>::max(),
//        MetricFunction&& metric = MetricFunction(),
//        Random&& rand = Random(std::random_device{}())) {
//
//    using cluster_types = clustering_traits<Container>;
//
//    const auto samples_count = cont.size();
//    const auto k_clusters_max = std::min(max_k_test, samples_count+1);
//
//
//    /**
//     * Stores sampling results
//     */
//    typename cluster_types::cluster_score_vec result(k_clusters_max);
//
//    /**
//     * For every k clusters, starting with k = 1 clusters, and increment and measure
//     */
//    for(size_t k_clusters = min_k_test; k_clusters < k_clusters_max; ++k_clusters) {
//
//        for(size_t i = 0; i < test_runs; ++i) {
//
//            auto selection = select_n_random(cont, k_clusters, rand);
//            typename cluster_types::cluster selection_values;
//            selection_values.reserve(k_clusters);
//            for(auto& [k, v] : selection) {
//                DMTK_UNUSED(v);
//                selection_values.emplace_back(k);
//            }
//
//            /**
//             * Create the initial k clusters from the random selection above
//             */
//            auto [clusters, centroids] = create_k_means_clusters_from_selection(cont, selection, metric);
//
//            /**
//             * Rebalance the k-means clusters by iterating over all elements
//             * in the input container and checking if there is a cluster which
//             * has a mean that is closer than the currently assigned cluster. If
//             * one exists, swap clusters and repeat until no such clusters are
//             * found.
//             */
//            rebalance_k_means_clusters(cont, clusters, centroids);
//
//            /**
//             * Calculate the sum of squares error for all clusters
//             */
//            std::vector<fp_type> sum_of_squares_errors;
//            auto clusters_idx = 0;
//            for(auto it = clusters.begin(), end = clusters.end(); it != end; ++it, ++clusters_idx) {
//                auto sum_dist = sum_distance_euclidean(*it, centroids[clusters_idx]);
//                sum_of_squares_errors.emplace_back(sum_dist * sum_dist);
//            }
//
//            result[k_clusters].emplace_back(selection_values, sum_of_squares_errors);
//        }
//    }
//
//    return result;
//}
//
//
///**
// * Tests various kmeans values by splitting a input container into two parts and sampling the statistical success rate of it
// *
// * @param test_runs
// * @param max_k_test the maximum number of clusters to test (defaults to the number of data points in the container provided
// * @return the result map
// */
//template<typename Container, typename MetricFunction, typename Random = std::mt19937>
//auto minimize_kmeans(Container& cont, MetricFunction&& metric, size_t test_runs = 1, size_t max_k_test = std::numeric_limits<size_t>::max()) {
//    auto res = sample_kmeans(cont, test_runs,  max_k_test);
//    auto res_it = std::max_element(res.begin(), res.end(), [](const auto& left, const auto& right) {
//        return std::get<1>(left) > std::get<1>(right);
//    });
//    return *res_it;
//}
//
//
//DMTK_NAMESPACE_END
//
//#endif /* DMTK_ALGORITHM_CLUSTERING_KMEANS_HPP */
//
