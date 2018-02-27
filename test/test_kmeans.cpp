/* Lab03: KNN
 * Omar Thor
 * Aurora Hernandez
 */

#include "algorithm.hpp"

/**
 * Test another model against this model data
 *
 * @param other_model the other model to test points from
 * @param k the k value to test
 * @return the result map
 */
template<typename Container>
std::map<size_t, bool> run_tests(const Container& samples, const Container& test, size_t k) {
    std::map<size_t, bool> res;
    size_t correct = 0;
    for(size_t test_idx = 0, test_len = samples.size(); test_idx < test_len; ++test_idx) {
        const auto& prediction = predict_by_knn<1>(samples, test[test_idx], k);
        const auto& actual = get_label(test[test_idx]);
        auto result = prediction == actual;
        res[test_idx] = result;
        if(result) {
            ++correct;
        }
        std::cout << "Testing " << (test_idx + 1) << " > prediction: " << prediction << "(Actual: " << actual << ")\n";
    }

    return res;
}

int main(int, char**) {

    auto data = csv<float, float, std::string>("datasets/Lab2known.txt");

    normalize(data);
//
    const size_t nr_of_samples = 1000;

    const size_t max_k = data.size();

    /**
     * Gather sample data for kmeans from k=1 to k=10
     */
    auto res = sample_kmeans<1>(data, nr_of_samples);

    std::cout << "Per K, the average distance for all elements to their assigned clusters:\n";
    for(auto [k, info ] : res) {
        std::cout << "K = " << k << " { ";
        for(auto [k, v] : info) {
            UNUSED(k);
            std::cout << v << ", ";
        }
        std::cout << " } \n";
    }

    std::cout << "\n\nPer K given below, the best initial selection of nodes based on samples above are:\n";
    for(size_t k = 1; k < max_k; ++k) {

        auto [dist, best_n] = optimizer(
            [&res, &k](const auto& n) {
                //lookup k = 1
                //get the second value of the k=1, n-ths result
                return std::get<1>(res[k][n]);
            },
            std::less<void>(),
            0, nr_of_samples
        );

        std::cout << "\n\nFor K = " << k << ", avg proximity to centroid of nodes in each cluster = " << dist << "\n\t{";
        for(auto [attr1, attr2, attr3] : std::get<0>(res[k][best_n])) {
            UNUSED(attr3);
            std::cout << '(' << attr1 << ',' << attr2 << "), ";
        }

        std::cout << "}\n";
    }

    std::cout << "\n\n";

    return 0;
}

