/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_HMM_MODEL_DETAIL_MODEL_HPP
#define DMTK_ALGORITHM_HMM_MODEL_DETAIL_MODEL_HPP

#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <functional>


DMTK_DETAIL_NAMESPACE_BEGIN

/**
 * \brief Convert probability map to log2 based
 */
template<typename State, typename Emission, typename Prob>
void hmm_probability_map_to_log2(std::unordered_map<State, std::unordered_map<Emission, Prob>>& probs) {
    for(auto& v : probs) {
        std::for_each(v.second.begin(), v.second.end(), [](auto& pair) {
            pair.second = std::log2(pair.second);
        });
    }
}

/**
 * \brief Convert transition map to log2 based
 */
template<typename State, typename Prob>
void hmm_transition_map_to_log2(std::unordered_map<std::tuple<State, State>, Prob>& trans) {
    std::for_each(trans.begin(), trans.end(), [](auto& pair) {
        pair.second = std::log2(pair.second);
    });
}

/**
 * Construct the default starting probabilities vector
 */
template<typename State, typename Emission, typename Prob = fp_type>
std::unordered_map<State, Prob> hmm_default_starting_probs(std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_probs) {
    const auto states_count = emission_probs.size();
    std::unordered_map<State, Prob> start_probs(emission_probs.size());
    for(auto& p : emission_probs) {
        start_probs.emplace(p.first, 1.0f / states_count);
    }
    return start_probs;
}

DMTK_DETAIL_NAMESPACE_END

#endif /* DMTK_ALGORITHM_HMM_MODEL_DETAIL_MODEL_HPP */

