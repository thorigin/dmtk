/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_UTIL_GRAPH_NODE_HPP
#define DMTK_UTIL_GRAPH_NODE_HPP

#include <utility>
#include <functional>
#include <boost/functional/hash.hpp>

#include "dmtk/config.hpp"
#include <memory>


DMTK_NAMESPACE_BEGIN

/**
 * @brief Generic graph node
 */
template<typename T>
struct graph_node;

/**
 * @brief Graph Node Pointer (alias, std::shared_ptr)
 */
template<typename T>
using graph_node_ptr = std::shared_ptr<graph_node<T>>;

/**
 * @brief Generic graph node
 */
template<typename T>
struct graph_node {

    using edges_vector = std::vector<graph_node_ptr<T>>;
    using value_type = T;

    graph_node() : value(), edges() {}

    graph_node(const value_type& value) : value(value), edges() {}
    graph_node(value_type&& value) : value(std::move(value)), edges() {}

    graph_node(const value_type& value_, edges_vector&& edges_) : value(std::move(value_)), edges(std::move(edges_)) {}
    graph_node(value_type&& value_, edges_vector&& edges_) : value(value_), edges(std::move(edges_)) {}

    graph_node(const graph_node&) = default;
    graph_node(graph_node&&) = default;
    graph_node& operator=(const graph_node&) = default;
    graph_node& operator=(graph_node&&) = default;

    auto size() const {
        return edges.size();
    }

    auto size() {
        return edges.size();
    }

    bool empty() {
        return edges.empty();
    }

    bool empty() const {
        return edges.empty();
    }

    T value;

    edges_vector edges;

};

/**
 * @brief Helper function to create a new shared_ptr for graph_node
 *
 * @param arguments the arguments to pass to the constructor of graph_node
 * @return the graph_node_ptr created
 */
template<typename T, typename ... Arguments>
graph_node_ptr<T> make_graph_node_ptr(Arguments&& ... args) {
    return std::make_shared<graph_node<T>>(std::forward<Arguments>(args)...);
}

DMTK_NAMESPACE_END

/**
 * Specialize std::hash for graph_node
 */
namespace std {

    /**
     * @brief std::hash specialization for graph_node
     */
    template<typename T>
    struct hash<dmtk::graph_node<T>> {
        size_t operator()(const dmtk::graph_node<T>& n) const {
            using value_type = typename dmtk::graph_node<T>::value_type;
            size_t seed = 0;
            boost::hash_combine(seed, hash<value_type>()(n.value));
            /**
             * To avoid recursion, only edge's data entries are considered
             */
            for(const auto& e : n.edges) {
                boost::hash_combine(seed, hash<value_type>()(e.value));
            }
            return seed;
        }
    };
   
    /**
     * @brief std::equal_to specialization for graph_node
     */
    template<typename T>
    struct equal_to<dmtk::graph_node<T>> {
        bool operator()(const dmtk::graph_node<T>& lhs, const dmtk::graph_node<T>& rhs) const {
            using value_type = typename dmtk::graph_node<T>::value_type;
            if(!equal_to<T>()(lhs.value, rhs.value)) {
                return false;
            }
            if(lhs.size() != rhs.size()) {
                return false;
            }
            /**
             * Compare only data of edges
             */
            for(size_t i = 0, len = lhs.size(); i < len; ++i) {
                if(!equal_to<value_type>()(lhs.edges[i].value, rhs.edges[i].value)) {
                    return false;
                }
            }
            return true;
        }
    };
}

#endif /* DMTK_UTIL_GRAPH_NODE_HPP */
