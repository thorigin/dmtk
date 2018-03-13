#define BOOST_TEST_MODULE dmtk_algo_entropy
#include <boost/test/unit_test.hpp>
#include "dmtk.hpp"

using namespace dmtk;

BOOST_AUTO_TEST_CASE( sample_test ) {

    auto res = hmm_probability_of(
        {'F', 'F', 'F', 'F', 'F', 'L', 'L', 'L', 'L', 'F', 'F', 'F', 'F', 'F', 'F'},
        { 1, 2, 1, 5, 6, 2, 1, 6, 2, 4, 6, 2, 3, 6, 4 },
        {
            { 'F', 1, 1/6.0f},
            { 'F', 2, 1/6.0f},
            { 'F', 3, 1/6.0f},
            { 'F', 4, 1/6.0f},
            { 'F', 5, 1/6.0f},
            { 'F', 6, 1/6.0f},

            { 'L', 1, 1/8.0f},
            { 'L', 2, 1/8.0f},
            { 'L', 3, 1/8.0f},
            { 'L', 4, 1/8.0f},
            { 'L', 5, 1/8.0f},
            { 'L', 6, 3/8.0f},
        },
        {
            { 'F', 'L', 0.1},
            { 'L', 'F', 0.1},
            { 'L', 'L', 0.9},
            { 'F', 'F', 0.9}
        }
    );


    std::cout << "The result is: " << res << "\n";
}


