#ifndef DMTK_ALGORITHM_REGRESSION_HPP
#define DMTK_ALGORITHM_REGRESSION_HPP

DMTK_NAMESPACE_BEGIN

#include <iterator>
#include <numeric>
#include <functional>
#include <algorithm>
#include <type_traits>

/**
 * \brief Calculates the best fit for x and y values given using
 *        simple linear regression
 * @return a tuple of B0 and B1
 */
template<typename X, typename Y, typename T = float>
auto simple_linear_regression(const X& x, const Y& y) {

    using value_type = std::common_type_t<typename X::value_type, typename Y::value_type>;
    static_assert(std::is_floating_point_v<value_type>);
    auto x_count = std::distance(x.begin(), x.end());
    auto y_count = std::distance(x.begin(), x.end());

    size_t n = static_cast<size_t>(x_count);

    if(x_count != y_count) {
        throw std::runtime_error("x and y size mismatch");
    }

    value_type sum_x = 0;
    value_type sum_y = 0;
    value_type sum_xx = 0;
    value_type sum_xy = 0;

    for(size_t i = 0; i < n; ++i) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xx += (x[i] * x[i]);
        sum_xy += (x[i] * y[i]);
    }

    value_type m = (( n * sum_xy ) - (sum_x) * (sum_y)) / (n * sum_xx - sum_x * sum_x);
    value_type b = (sum_y - m * sum_x) / n;

    return std::make_tuple(b, m);
}

/**
 * \brief Residual Sums of Error Squared
 *
 * @return the sum
 */
template<typename Actual, typename Predicted>
auto rss(const Actual& actual, const Predicted& predicted) {
    using value_type = std::common_type_t<typename Actual::value_type, typename Predicted::value_type>;

    auto x_len = std::distance(actual.begin(), actual.end());
    auto y_len = std::distance(predicted.begin(), predicted.end());

    if(x_len != y_len) {
        throw std::runtime_error("x and y size mismatch");
    }

    value_type sum = 0;

    auto act_it = actual.begin(),
         act_end = actual.end();
    auto pred_it = predicted.begin();
    for(; act_it != act_end; ++act_it, ++pred_it) {
        auto& actual = *act_it;
        auto& prediction = *pred_it;
        auto epsilon = (actual - prediction);
        sum += epsilon * epsilon;
    }

    return sum;
}


template<typename Range, typename Intercept, typename Slope>
auto predict_by_simple_linear_regression(const Range& range, const std::tuple<Intercept, Slope>& fitted_line) {
    using value_type = std::remove_reference_t<typename Range::value_type>;
    std::vector<value_type> result;
    result.reserve(std::distance(range.begin(), range.end()));
    auto& [b0, b1] = fitted_line;
    for(auto x : range) {
        result.emplace_back(b0 + b1 * x);
    }
    return result;
}

DMTK_NAMESPACE_END


#endif /* DMTK_ALGORITHM_REGRESSION_HPP */

