/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_UTIL_GRAPH_VISITOR_HPP
#define DMTK_UTIL_GRAPH_VISITOR_HPP

#include <unordered_set>
#include "node.hpp"

DMTK_NAMESPACE_BEGIN

namespace detail {

    /**
     * @brief visit depth first implementation
     *
     * @todo recursive, to be optimized
     */
    template<typename T, typename Visitor>
    void visit_depth_first(const graph_node<T>& n, Visitor visitor, std::unordered_set<graph_node<T>>& visited) {
        /**
         * Check if node has already been visited
         * @param root
         * @param visitor
         * @param visited
         */
        visitor(n);
        visited.emplace(n);
        for(const auto& e : n.edges) {
            if(visited.find(e) == visited.end()) {
                visit_depth_first(e, visitor, visited);                
            }
        }
    }
}

struct visit_strategy {};

struct depth_first_strategy : visit_strategy {};

struct breadth_first_strategy : visit_strategy {};

template<typename T, typename Visitor>
void visit(const graph_node<T>& n, Visitor visitor, depth_first_strategy) {
    std::unordered_set<graph_node<T>> visited(32);
    detail::visit_depth_first(n, visitor, visited);
}

/**
 *
 * @param n
 * @param visitor
 */
template<typename T, typename Visitor>
void visit(const graph_node<T>& n, Visitor visitor) {
    visit(n, visitor, depth_first_strategy{});
}


DMTK_NAMESPACE_END


#endif /* DMTK_UTIL_GRAPH_VISITOR_HPP */

