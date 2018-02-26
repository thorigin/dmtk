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

int main(int argc, char** argv) {

    try {
        
        auto test = csv<float, float, std::string>("datasets/Lab2known.txt", true);

        std::cout << "Size: " << test.size() << '\n';

        normalize<1>(test);

        auto opt = test_knn_values<1>(test, 10);


        auto res_it = std::max_element(opt.begin(), opt.end(), [](const auto& left, const auto& right) {
            return left.first > right.first;
        });

        size_t best_k = (*res_it).first;
        float best_k_accuracy = (*res_it).second;

        std::cout << "The best K value is: " << best_k << " with an estimated accuracy of " << best_k_accuracy << "\n";

        auto real = csv<float, float, std::string>("datasets/Lab2unknown.txt", true);
        //    
        normalize(real);

        run_tests(test, real, best_k);

    }catch(std::exception* ex) {
        std::cout << "Failed running " << __FILE__ << ", message:\n\t" << ex->what() << '\n';
        throw ex;
    }

    return 0;
}

