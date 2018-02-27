/* Lab03: KNN
 * Omar Thor
 * Aurora Hernandez
 */

#include "dmtk.hpp"


using namespace dmtk;

int main(int, char**) {

    /**
     * Read in the csv data
     */
    auto data = csv<float, float, std::string>("datasets/Lab2known.txt");

    /**
     * Normalize the csv data read in
     */
    normalize(data);
    
    /**
     * Number of samples per K
     */
    const size_t nr_of_samples = 100;

    /**
     * Maximum number of K (number of clusters) to test
     * @param 
     * @param 
     * @return 
     */
    const size_t max_k = data.size();

    /**
     * Gather sample data for K-means from for all possible K values (1-80)
     */
    auto res = sample_kmeans<1>(data, nr_of_samples);

        std::cout << "Per K, the SSE for all elements to their assigned clusters, for all test runs:\n";
        for(auto [k, info ] : res) {
            std::cout << "K = " << k << " { ";
            bool first = true;
            for(auto [k, v] : info) {
                DMTK_UNUSED(k);
                //Omit empty clusters
                //if(!v.empty()) {
                    if(first) {
                        first = false;
                    } else {
                        std::cout << ", ";
                    }
                    bool first2 = true;            
                    std::cout << "(";
                    for(auto& sse : v) {
                        if(first2) {
                            first2 = false;
                        } else {
                            std::cout << ", ";
                        }
                        std::cout << sse;
                    }
                    std::cout << ")";
                //}
            }
            std::cout << " } \n";
        }

        std::cout << "\n\nPer K given below, the average of SSE for all clusters of all test runs:\n";
        for(size_t k = 1; k < max_k+1; ++k) {

            auto [dist, best_n] = optimizer(
                [&res, &k](const auto& n) {
                    //lookup k = 1
                    //get the second value of the k=1, n-ths result
                    return std::get<1>(res[k][n]);
                },
                std::less<void>(),
                0, nr_of_samples
            );
            DMTK_UNUSED(best_n);

            auto avg_res = avg(dist, [](auto& x) { return x; });
            std::cout << "For K = " << k << ", avg SSE of all clusters = " << avg_res << "\n";
        }

        std::cout << "\n\n";

    return 0;
}

