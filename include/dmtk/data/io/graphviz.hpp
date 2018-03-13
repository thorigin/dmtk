/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_DATA_IO_CSV_HPP
#define DMTK_DATA_IO_CSV_HPP

#include "dmtk/element/element.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

DMTK_NAMESPACE_BEGIN


namespace detail {

    template<typename T>
    void graphviz_graph_dendrogram_helper(std::ostream& os, const graph_node<T>& n, size_t indent) {
        std::string tabs(indent*4, ' ');

        os << tabs << n.data << " [shape=box,rank=" << indent << ",label= " << n.data << "];\n";

        for(size_t i = 0, len = n.size(); i < len; ++i) {
            os << tabs << n.data << " -- " << n.edges[i].data << ";\n";
            if(!n.edges[i].empty()) {
                graphviz_graph_dendrogram_helper(os, n.edges[i], indent+2);
            }
        }
    }
}


template<typename T>
void graphviz_graph_dendrogram(std::ostream& os, const std::vector<graph_node<T>>& vec) {
    os << "graph dendrogram {\n\tsplines=ortho;shape=box;rankdir=TB;\n";
    for(auto& v : vec) {
        detail::graphviz_graph_dendrogram_helper(os, v, 1);
    }
    os << "}\n";
}

template<typename T>
void graphviz_graph_dendrogram(std::ostream& os, const graph_node<T>& n) {
    os << "graph dendrogram {\n";
    detail::graphviz_graph_dendrogram_helper(os, n, 1);
    os << "}\n";
}

DMTK_NAMESPACE_END

#endif /* DMTK_DATA_IO_CSV_HPP */

