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
template<typename TransititionMap>
void hmm_transition_map_to_log2(TransititionMap& trans) {
    std::for_each(trans.begin(), trans.end(), [](auto& pair) {
        pair.second = std::log2(pair.second);
    });
}

/**
 * \brief Convert transition map to log2 based
 */
template<typename StartingProbs>
void hmm_starting_probs_to_log2(StartingProbs& starting) {
    std::for_each(starting.begin(), starting.end(), [](auto& pair) {
        pair.second = std::log2(pair.second);
    });
}

/**
 * Construct the default starting probabilities vector
 */
template<typename State, typename Emission, typename Prob = double>
std::unordered_map<State, Prob> hmm_default_starting_probs(std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_probs) {
    const auto states_count = emission_probs.size();
    std::unordered_map<State, Prob> start_probs(emission_probs.size());
    for(auto& p : emission_probs) {
        start_probs.emplace(p.first, 1.0f / states_count);
    }
    return start_probs;
}


/**
 * Provides way to make pluggable log(p(x)) calculations
 */
template<bool Logarithmic>
struct hmm_probability_scale;

/**
 * \brief Partial specialization of hmm_probability_scale which defaults to
 *        default numeric scale
 */
template<>
struct hmm_probability_scale<false> {

    template<typename ProbabilityMap, typename TransitionMap, typename StartingProbs>
    hmm_probability_scale(ProbabilityMap&, TransitionMap&, StartingProbs&) {}

    template<typename LHS, typename RHS>
    inline auto mul(const LHS& lhs, const RHS& rhs) {
        return lhs * rhs;
    }

    template<typename T>
    inline auto p_of(const T& value) {
        return value;
    }
   
};

/**
 * \brief Partial specialization of hmm_probability_scale which treats
 *        multiplication as additions
 */
template<>
struct hmm_probability_scale<true> {

    template<typename ProbabilityMap, typename TransitionMap, typename StartingProbs>
    hmm_probability_scale(ProbabilityMap& pm, TransitionMap& tm, StartingProbs& sp) {
        hmm_probability_map_to_log2(pm);
        hmm_transition_map_to_log2(tm);
        hmm_starting_probs_to_log2(sp);
    }

    template<typename LHS, typename RHS>
    auto mul(const LHS& lhs, const RHS& rhs) {
        return lhs + rhs;
    }

    template<typename T>
    auto p_of(const T& value) {
        return value;
    }

};

/**
 * \brief Validates the size and shapes of parameters of hmm functions
 *
 */
template<typename EmissionProbMap, typename TransitionProbMap, typename Starting>
void hmm_validate_parameters(       EmissionProbMap& emission_probs,
                                    TransitionProbMap& transmissin_probs,
                                    Starting& starting_probs) {

    const auto nr_of_states = emission_probs.size();
    const auto nr_of_transitions = transmissin_probs.size();
    auto nr_of_emission_probs = 0;
    for(auto& kv : emission_probs) {
        nr_of_emission_probs += kv.second.size();
    }
    const auto nr_of_starting = starting_probs.size();

    BOOST_ASSERT_MSG(nr_of_emission_probs > 0, "Emission observations probability map input must not be empty");
    BOOST_ASSERT_MSG(nr_of_transitions > 0, "Transition probability map input must not be empty");
    BOOST_ASSERT_MSG(nr_of_starting > 0, "Starting probability vector input must not be empty");
    BOOST_ASSERT_MSG(nr_of_emission_probs > 0, "Emission probability map input must not be empty");

    BOOST_ASSERT_MSG(
        nr_of_states == nr_of_starting,
        "Number of starting possibilities is equal to the number of states"
    );
}

DMTK_DETAIL_NAMESPACE_END

#endif /* DMTK_ALGORITHM_HMM_MODEL_DETAIL_MODEL_HPP */

