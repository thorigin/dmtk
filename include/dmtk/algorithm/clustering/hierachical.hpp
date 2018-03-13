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
 * @file Agglomerative Clustering Methods
 */

#ifndef DMTK_ALGORITHM_CLUSTERING_HIERARCHICAL_HPP
#define DMTK_ALGORITHM_CLUSTERING_HIERARCHICAL_HPP

#include <tuple>

#include <boost/functional/hash.hpp>

#include "common.hpp"
#include "dmtk/util/tuple_hash.hpp"
#include "dmtk/data.hpp"
#include "metric.hpp"
#include "dmtk/util/preprocessor.hpp"
#include "dmtk/util/graph.hpp"

DMTK_NAMESPACE_BEGIN

namespace detail {

    /**
     * @brief Metric cache used by hierarchical clustering function
     *
     * @todo use pairwise_cache
     */
    template<typename Iterator, typename Metric>
    struct metric_handler {

        using key_type = std::tuple<size_t,size_t>;

        metric_handler(Metric&& metric_)
            : metric(std::forward<Metric>(metric_)) {}

        void operator()(Iterator lhs, Iterator rhs, std::tuple<Iterator, Iterator, fp_type>& selection) {
            if(lhs->first != rhs->first) {

                check_swap(lhs, rhs);

                fp_type result;
                key_type key = std::make_tuple(lhs->first, rhs->first);

                auto res_it = result_cache.find(key);
                if(res_it != result_cache.end()) {
                    result = std::get<1>(*res_it);
                } else {
                    result = metric(lhs->second, rhs->second);
                    result_cache[key] = result;
                }
                if(result < std::get<2>(selection)) {
                    selection = {lhs, rhs, result};
                }
            }
        }

        void invalidate(Iterator lhs, Iterator rhs) {
            check_swap(lhs, rhs);
            result_cache.erase({lhs->first, rhs->first});
        }

        void check_swap(Iterator& lhs, Iterator& rhs) {
            /**
             * Check if first index is greater than second, if so, swap
             * reduce memory usage by half if calls are not optimized.
             * if the distance between left and right is negative,
             * then the right side is after the left
             */
            if(lhs->first > rhs->first) {
                std::swap(lhs, rhs);
            }
        }

        Metric metric;
        std::unordered_map<key_type, fp_type> result_cache;

    };
}

/**
 * @brief Cluster Hierarchical Node Data
 *
 * Contains:
 * - The sequence_id of the node
 * - The metric value for the node
 */
struct hierarchical_cluster_node_data {

    using seq_id_type = size_t;
    using metric_type = fp_type;

    hierarchical_cluster_node_data()
    : seq_id(), metric() {}
    hierarchical_cluster_node_data(seq_id_type sequence_id, metric_type metric)
    : seq_id(sequence_id), metric(metric) { }
    
    hierarchical_cluster_node_data(const hierarchical_cluster_node_data&) = default;
    hierarchical_cluster_node_data(hierarchical_cluster_node_data&&) = default;
    hierarchical_cluster_node_data& operator=(const hierarchical_cluster_node_data&) = default;
    hierarchical_cluster_node_data& operator=(hierarchical_cluster_node_data&&) = default;

    seq_id_type seq_id;
    metric_type metric;

};

/**
 * @brief Cluster Hierarchical Node (alias)
 */
using hierarchical_cluster_node = graph_node_ptr<hierarchical_cluster_node_data>;

/**
 * @brief Hierarchical clustering
 *
 * Applies Hierarchical clustering using the specified Metric
 *
 * @param start the start of the range, first element will be given the sequence id 0, and values increment to the end
 * @param end the end of the range
 * @param metric The metric to use, defaults to default_metric_function
 *
 * @return Returns a cluster_hierarchical_node (graph_node_ptr)
 */
template<typename Iterator, typename Metric = default_metric_function>
auto hierarchical_clustering(Iterator start, Iterator end, Metric&& metric = Metric()) {

    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using sequence_index = size_t;

    sequence_index sequence_idx = 0;

    size_t k_clusters = std::distance(start, end);

    using cluster_type = cluster<value_type>; /** Make cluster copy values if input is a reference */
    using clusters_iterator = typename std::unordered_map<sequence_index, cluster_type>::iterator;

    /**
     * Stores the clusters
     */
    std::unordered_map<sequence_index, cluster_type> clusters(k_clusters);

    /**
     * Store graph nodes for merge operations
     */
    std::unordered_map<sequence_index, hierarchical_cluster_node> nodes(k_clusters);

    /**
     * Cache metric lookups
     */
    detail::metric_handler<clusters_iterator, Metric> m_handler(std::forward<Metric>(metric));

    /**
     * Initialize clusters and nodes
     */
    auto clust_seq = sequence_idx;
    std::for_each(start, end, [&](const auto& v) {
        clusters.emplace(clust_seq, cluster<value_type>{v});
        nodes.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(
                clust_seq
            ),
            std::forward_as_tuple(
                make_graph_node_ptr<hierarchical_cluster_node_data>(
                    hierarchical_cluster_node_data(clust_seq, 0)
                )
            )
        );
        ++clust_seq;
    });
   
    sequence_idx += k_clusters;

    /**
     * While there are more than 1 clusters remaining
     */
    while(clusters.size() > 1) {
        
        std::tuple<clusters_iterator, clusters_iterator, fp_type> merge_selection(clusters.end(), clusters.end(), std::numeric_limits<fp_type>::max());
        
        /**
         * Find the merge selection according to the metric specified
         */
        for(auto o_it = clusters.begin(), o_end = clusters.end(); o_it != o_end; ++o_it) {
            for(auto i_it = ++clusters_iterator(o_it), i_end = clusters.end(); i_it != i_end; ++i_it) {
                m_handler(o_it, i_it, merge_selection);
            }
        }

        /**
         * Merge the two best matched clusters found by the matric
         */
        auto [first_it, second_it, metric_factor] = merge_selection;

        DMTK_UNUSED(metric_factor);

        auto& [first_key, first_cluster_vec] = *first_it;
        auto& [second_key, second_cluster_vec] = *second_it;

        auto new_cluster_idx = ++sequence_idx;

        /**
         * Create new node and move the nodes which are to be merged into the
         * the new node as neighbours
         */
        nodes.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(
                new_cluster_idx
            ),
            std::forward_as_tuple(
                make_graph_node_ptr<hierarchical_cluster_node_data>(
                    hierarchical_cluster_node_data{
                        new_cluster_idx,
                        metric_factor
                    },
                    typename hierarchical_cluster_node::element_type::edges_vector{
                        std::move(nodes[first_key]),
                        std::move(nodes[second_key])
                    }
                )
            )
        );

        nodes.erase(first_key);
        nodes.erase(second_key);

        /**
         * Create new cluster
         */
        auto new_cluster_res = clusters.emplace(new_cluster_idx, cluster_type{});

        auto& [new_cluster_key, new_cluster_vec] = *(new_cluster_res.first);

        DMTK_UNUSED(new_cluster_key);

        /**
         * Copy the two selected clusters to be merge into the new cluster
         */
        new_cluster_vec.reserve(second_cluster_vec.size() + first_cluster_vec.size());
        std::copy(second_cluster_vec.begin(), second_cluster_vec.end(), std::back_inserter(new_cluster_vec));
        std::copy(first_cluster_vec.begin(), first_cluster_vec.end(), std::back_inserter(new_cluster_vec));

        /**
         * Remove merged clusters and centroids
         */
        clusters.erase(first_it);
        clusters.erase(second_it);

        /**
         * Invalidate any information regarding the existence of the erased
         * clusters to reduce memory footprint during runtime
         */
        m_handler.invalidate(first_it, second_it);
    }

    if(nodes.size() != 1) {
        std::cerr << "The number of nodes is: " << nodes.size() << "\n";
        throw std::runtime_error("One remaining root node should be in graph");
    }
    
    return nodes.begin()->second;
}

/**
 * @brief Hierarchical Clustering
 *
 */
template<typename Container, typename Metric = default_metric_function>
auto hierarchical_clustering(Container& cont, Metric&& metric = Metric()) {
    return hierarchical_clustering(std::begin(cont), std::end(cont), std::forward<Metric>(metric));
}


template<typename T>
auto collect_hierarchical_root_nodes(graph_node<T>& node) {
    std::vector<graph_node<T>> roots;
    visit(node, [&](auto& node) {
        if(node.empty()) {
            roots.push_back(node);
        }
    });
}

DMTK_NAMESPACE_END

namespace std {
    /**
     * std::hash specialization for dmtk::cluster_hierarchical_node_data
     */
    template<>
    struct hash<dmtk::hierarchical_cluster_node_data> {
        size_t operator()(const dmtk::hierarchical_cluster_node_data& chnd) const noexcept {
            size_t seed = 0;
            boost::hash_combine(seed, std::hash<dmtk::hierarchical_cluster_node_data::seq_id_type>()(chnd.seq_id));
            boost::hash_combine(seed, std::hash<dmtk::hierarchical_cluster_node_data::metric_type>()(chnd.metric));
            return seed;
        }
    };

    template<>
    struct equal_to<dmtk::hierarchical_cluster_node_data> {
        bool operator()(const dmtk::hierarchical_cluster_node_data& lhs, const dmtk::hierarchical_cluster_node_data& rhs) const noexcept {
            return  equal_to<dmtk::hierarchical_cluster_node_data::seq_id_type>()(lhs.seq_id, rhs.seq_id) &&
                    equal_to<dmtk::hierarchical_cluster_node_data::metric_type>()(lhs.metric, rhs.metric);
        }
    };
}

#endif /* DMTK_ALGORITHM_CLUSTERING_HIERARCHICAL_HPP */

