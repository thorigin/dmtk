/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_KNN_HPP
#define DMTK_ALGORITHM_KNN_HPP

#include "dmtk/element.hpp"
#include <unordered_set>
#include <unordered_map>
#include <limits>



DMTK_NAMESPACE_BEGIN

/**
 * Predict the label of the row by using k samples from the model provided
 *
 * @param model
 * @param row
 * @param k
 * @return the predicated label
 */
template<typename Container>
auto predict_by_knn(
    const Container& sample_model,
    const typename Container::value_type& test_element,
    const size_t& k)
    -> std::decay_t<decltype(get_label(test_element))> {

   std::unordered_set<size_t> used_rows;

    using label_type = std::decay_t<decltype(get_label(test_element))>;
    std::unordered_map<label_type, size_t> counters;

    //Test every row for every other row, at current K
    //Check if k closest rows are above the threshold
    //First, find the k closest
    auto sample_size = sample_model.size();

    //Find K closest points and store accuracy
    for (size_t k_sample_idx = 0; k_sample_idx < k; ++k_sample_idx) {

        /**
         * The closest sample row index and distance that is unused
         */
        auto closest_sample_row_dist = euclidean_distance(test_element, sample_model[0]);
        size_t closest_sample_row_idx = 0;

        for (size_t sample_row_idx = 1; sample_row_idx < sample_size; ++sample_row_idx) {

            //Check if row has not already been sampled
            if(used_rows.find(sample_row_idx) == used_rows.end()) {

                auto temp_sample_dist = euclidean_distance(test_element, sample_model[sample_row_idx]);

                if(closest_sample_row_dist > temp_sample_dist) {
                    closest_sample_row_dist = temp_sample_dist;
                    closest_sample_row_idx = sample_row_idx;
                }
            }
        }

        //Mark closest sample_row as used
        used_rows.emplace(closest_sample_row_idx);

        //Increment hit counter for label
        const auto& label = get_label(sample_model[closest_sample_row_idx]);
        ++counters[label];
    }

    auto counters_it = counters.begin(), counters_end = counters.end();
    if(counters_it != counters_end) {
        auto guess = (*counters_it++).first;
        size_t guess_freq = (*counters_it++).second;
        for (; counters_it != counters_end; ++counters_it) {
            const auto& c = (*counters_it);
            if(c.second > guess_freq) {
                guess = c.first;
                guess_freq = c.second;
            }
        }
        if(guess.empty()) {
            throw std::runtime_error("No labels matched");
        }
        return guess;
    } else {
        throw std::runtime_error("Prediction not possible");
    }
}


/**
 * Sample various knn values by splitting a input container into two parts and sampling the statistical success rate of it
 *
 * @param test_runs
 * @return
 */
template<size_t SkipLastN = 1, typename Container>
std::unordered_map<size_t, float> sample_knn_values(Container& cont, size_t test_runs = 1, float split_dataset = 0.75, size_t max_k_test = std::numeric_limits<size_t>::max()) {


    std::unordered_map<size_t, std::vector<float>> test_model_predictions;
    for(size_t i = 0; i < test_runs; ++i) {

        auto [sample_data, test_data] = split(cont, split_dataset);
        auto sample_data_size = sample_data.size();
        auto test_data_size = test_data.size();
        auto k_max = std::min(max_k_test, sample_data_size);

        //For every K
        for(size_t k = 1; k < k_max; k+=2) { //test only odd numbers

            size_t correct_labels = 0;
            //Test every row for the value K
            for(size_t test_idx = 0, test_count = test_data_size; test_idx < test_count; ++test_idx) {

                //Attempt to predict test_row with k nearest neighbours
                const auto& predicated_label = predict_by_knn(sample_data, test_data[test_idx], k);
                const auto& expected_label = get_label(test_data[test_idx]);

                if(predicated_label == expected_label) {
                    ++correct_labels;
                }

            }
            // Save the correctness of the training
            test_model_predictions[k].push_back(static_cast<float>(correct_labels) / static_cast<float>(test_data_size));
        }
    }

    std::unordered_map<size_t, float> ret;
    for(auto& res : test_model_predictions) {
        float mean = 0;
        for(const auto& a : res.second) {
             mean += a;
        }
        ret[res.first] = mean / res.second.size();
    }

    return ret;
}

/**
 * Maximizes the K-NN of sample test runs over the provided data set
 */
template<size_t SkipLastN = 1, typename Container>
auto maximize_knn (Container& cont, size_t test_runs = 1, float split_dataset = 0.75, size_t max_k_test = std::numeric_limits<size_t>::max()) {
    auto opt = sample_knn_values(cont, test_runs, split_dataset, max_k_test);
    auto res_it = std::max_element(opt.begin(), opt.end(), [](const auto& left, const auto& right) {
        return left.first > right.first;
    });
    return *res_it;
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_KNN_HPP */

