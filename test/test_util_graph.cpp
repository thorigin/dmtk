

#define BOOST_TEST_MODULE dmtk_algo_for_each
#include <boost/test/unit_test.hpp>

#include <tuple>

#include "dmtk.hpp"

using namespace dmtk;


BOOST_AUTO_TEST_CASE( for_each_container_lambda) {
//    std::vector<int> v{1, 2, 3, 4};
//    int sum = 0;
//    dmtk::for_each(v, [&](auto& v) { sum += v; });
//    BOOST_REQUIRE_EQUAL(sum, 10);
}
//
//
//BOOST_AUTO_TEST_CASE( for_each_tuple_lambda) {
//    auto tuple = std::make_tuple(1, 2, 3, 4);
//    int sum = 0;
//    dmtk::for_each(tuple, [&](auto& v) { sum += v; });
//    BOOST_REQUIRE_EQUAL(sum, 10);
//}