#define BOOST_TEST_MODULE dmtk_algo_clustering_hierarchical
#include <boost/test/unit_test.hpp>

#include <tuple>

#include "dmtk.hpp"

using namespace dmtk;

BOOST_AUTO_TEST_CASE( euclidean_min_distance_metric_3_ele) {
    
        std::vector<std::tuple<int, int>> data{{1, 1},{2, 2},{10, 10}};
        
        BOOST_REQUIRE_EQUAL(data.size(), 3);
         
        auto res = hierarchical_clustering(data, euclidean_min_distance_metric{});
        
        BOOST_REQUIRE_EQUAL(res->value.seq_id, 5);
        
//        std::fstream fout("graphviz/hierarchical-data.dot", std::ios::out);
//        graphviz_graph_dendrogram(fout, vec);
        
       
}

