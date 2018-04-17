#define BOOST_TEST_MODULE dmtk_algo_hmm
#include <boost/test/unit_test.hpp>
#include <iomanip>

#include "dmtk.hpp"

using namespace dmtk;

BOOST_AUTO_TEST_CASE( test_algo_regression ) {

    //vectors of { ID, price, bedrooms, bathrooms, sqft_living, sqft_lot }
    auto train_data = csv<int, float, float, float, float, float>("datasets/KingCountyHomes-train.csv");
    auto test_data = csv<int, float, float, float, float, float>("datasets/KingCountyHomes-test.csv");

    std::cout << "----------------------------------------------------------\n";    
    std::cout << "\n\nPart 6.1:\n\n";
    std::cout << "----------------------------------------------------------\n";
    {        
        auto price_test_view = view<1>(test_data);
        auto predict_equation_1 = predict_by_simple_linear_regression(view<2>(test_data), std::make_tuple(-1250000.0f, 550000.0f));
        auto predict_equation_2 = predict_by_simple_linear_regression(view<2>(test_data), std::make_tuple(30000.0f, 250.0f));        

        auto rss_eq_1 = rss(price_test_view, predict_equation_1);
        auto rss_eq_2 = rss(price_test_view, predict_equation_2);
        
        std::cout << "The RSS of equation 1 is: " << rss_eq_1 << "\n\n";
        std::cout << "The RSS of equation 2 is: " << rss_eq_2 << "\n\n";
    
    }
    std::cout << "----------------------------------------------------------\n";

    std::cout << "\n\nPart 6.3:\n\n";

    std::cout << "----------------------------------------------------------\n";
    {
        //nondependent variable
        auto bedrooms_view = view<2>(train_data);
        // dependent variable
        auto price_view = view<1>(train_data);
        std::cout << "Testing simple linear regression - Bedrooms:\n";
        //Test simple linear regression for bedrooms
        auto fitted_line = simple_linear_regression(bedrooms_view, price_view);
        auto& [b0, b1] = fitted_line;
        std::cout << "\n\n\tr(x) = " << b0 << " " << b1 << "x\n\n";
        /* view of actual test prices */
        auto price_test_view = view<1>(test_data);
        auto bathrooms_test_view = view<2>(test_data);
        auto price_predictions = predict_by_simple_linear_regression(bathrooms_test_view, fitted_line);
        std::cout << "Testing RSS: " << rss(price_test_view, price_predictions) << "\n\n";
    }
    std::cout << "----------------------------------------------------------\n";
    {
        //nondependent variable
        auto bathrooms = view<3>(train_data);
        // dependent variable
        auto price_view = view<1>(train_data);
        std::cout << "Testing simple linear regression - Bathroom:\n";
        //Test simple linear regression for bedrooms
        auto fitted_line = simple_linear_regression(bathrooms, price_view);
        auto& [b0, b1] = fitted_line;
        std::cout << "\n\n\tr(x) = " << b0 << " " << b1 << "x\n\n";
        /* view of actual test prices */
        auto price_test_view = view<1>(test_data);
        auto bathrooms_test_view = view<3>(test_data);
        auto price_predictions = predict_by_simple_linear_regression(bathrooms_test_view, fitted_line);
        std::cout << "Testing RSS: " << rss(price_test_view, price_predictions) << "\n\n";
    }
    std::cout << "----------------------------------------------------------\n";
    {
        //nondependent variable
        auto sqrt_living = view<4>(train_data);
        // dependent variable
        auto price_view = view<1>(train_data);
        std::cout << "Testing simple linear regression - Sqft. Living:\n";
        //Test simple linear regression for bedrooms
        auto fitted_line = simple_linear_regression(sqrt_living, price_view);
        auto& [b0, b1] = fitted_line;
        std::cout << "\n\n\tr(x) = " << b0 << " " << b1 << "x\n\n";
        /* view of actual test prices */
        auto price_test_view = view<1>(test_data);
        auto bathrooms_test_view = view<4>(test_data);
        auto price_predictions = predict_by_simple_linear_regression(bathrooms_test_view, fitted_line);
        std::cout << "Testing RSS: " << rss(price_test_view, price_predictions) << "\n\n";
    }
    std::cout << "----------------------------------------------------------\n";
    {
        //nondependent variable
        auto sqrt_lot = view<5>(train_data);
        // dependent variable
        auto price_view = view<1>(train_data);
        std::cout << "Testing simple linear regression - Sqft. Lot:\n";
        //Test simple linear regression for bedrooms
        auto fitted_line = simple_linear_regression(sqrt_lot, price_view);
        auto& [b0, b1] = fitted_line;
        std::cout << "\n\n\tr(x) = " << b0 << " " << b1 << "x\n\n";
        /* view of actual test prices */
        auto price_test_view = view<1>(test_data);
        auto bathrooms_test_view = view<5>(test_data);
        auto price_predictions = predict_by_simple_linear_regression(bathrooms_test_view, fitted_line);
        std::cout << "Testing RSS: " << rss(price_test_view, price_predictions) << "\n\n";
    }
    std::cout << "----------------------------------------------------------\n";    
}
