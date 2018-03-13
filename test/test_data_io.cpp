#define BOOST_TEST_MODULE dmtk_data_io
#include <boost/test/unit_test.hpp>
#include "dmtk.hpp"

using namespace dmtk;

BOOST_AUTO_TEST_CASE( test_csv_into_value_int ) {

    std::vector<int> throws;

    csv_into("datasets/dice_rolls.txt", throws, false);

    BOOST_REQUIRE(!throws.empty());
    BOOST_REQUIRE_EQUAL(throws.size(), 500);
    BOOST_REQUIRE_EQUAL(throws[0], 2);
    BOOST_REQUIRE_EQUAL(throws[1], 1);
    BOOST_REQUIRE_EQUAL(throws[2], 6);
    //...
    BOOST_REQUIRE_EQUAL(throws[497], 6);
    BOOST_REQUIRE_EQUAL(throws[498], 1);
    BOOST_REQUIRE_EQUAL(throws[499], 3);

}

BOOST_AUTO_TEST_CASE( test_csv_value_int ) {

    std::vector<int> throws = csv<int>("datasets/dice_rolls.txt", false);

    BOOST_REQUIRE(!throws.empty());
    BOOST_REQUIRE_EQUAL(throws.size(), 500);
    BOOST_REQUIRE_EQUAL(throws[0], 2);
    BOOST_REQUIRE_EQUAL(throws[1], 1);
    BOOST_REQUIRE_EQUAL(throws[2], 6);
    //...
    BOOST_REQUIRE_EQUAL(throws[497], 6);
    BOOST_REQUIRE_EQUAL(throws[498], 1);
    BOOST_REQUIRE_EQUAL(throws[499], 3);
}