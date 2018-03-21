/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_HMM_MODEL_HPP
#define DMTK_ALGORITHM_HMM_MODEL_HPP

#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>
#include <set>
#include <iterator>
#include <utility>
#include <cmath>

#include <boost/assert.hpp>

#include "detail/model.hpp"
#include "../optimize.hpp"
#include "../cache/pairwise.hpp"
#include "dmtk/util/preprocessor.hpp"

#include <iostream> // @TODO REMOVE

DMTK_NAMESPACE_BEGIN

/**
 * \brief Tag to specify which mode is preferred, logarithmic or nonlogarithmic
 * probability scale
 *
 */
struct hmm_logarithmic_scale_tag {};
struct hmm_nonlogarithmic_scale_tag {};

/**
 * \brief Calculate the probability of the observed_states and the observed
 *        emissions given the emission probability map and transition maps.
 *
 * @param start_probs (optionally empty, will be automatically generated)
 * 
 */
template<typename State, typename Emission, typename Prob = double, typename ScaleTag = hmm_nonlogarithmic_scale_tag>
auto hmm_probability_of(
        std::vector<State> observed_states,
        std::vector<Emission> observations,
        std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_probs,
        std::unordered_map<std::tuple<State, State>, Prob> transition_probs,
        std::unordered_map<State, Prob> start_probs = {},
        ScaleTag scale_tag = ScaleTag()) {

    DMTK_UNUSED(scale_tag);
    
    if(start_probs.empty()) {
        start_probs = detail::hmm_default_starting_probs(emission_probs);
    }

    detail::hmm_validate_parameters(emission_probs, transition_probs, start_probs);

    /**
     * Provides pluggable operations for handling logarithmic ops equivalency
     */
    constexpr bool is_logarithmic = std::is_same_v<ScaleTag, hmm_logarithmic_scale_tag>;
    detail::hmm_probability_scale<is_logarithmic> scale(emission_probs, transition_probs, start_probs);

    /**
     * Initial probability is the selection between the starting states
     * given as 1 over the number of states, the probability of starting at
     * any one of the initial states.
     */
    fp_type res = scale.p_of(start_probs[observed_states[0]]);
    
    res = scale.mul(res, emission_probs[observed_states[0]][observations[0]]);

    for(size_t p = 1, len = observations.size(); p < len; ++p) {
        auto& state_prev = observed_states[p-1];
        auto& state_next = observed_states[p];
        auto& result_next = observations[p];
        auto& prob_of = emission_probs[state_next][result_next];
        auto& prob_of_trans = transition_probs[{state_prev, state_next}];
        res = scale.mul(res, scale.mul(prob_of, prob_of_trans));
    }

    return res;
}

/**
 * \brief Predict Hidden Markov Model (HMM) sequence of states by the observed
 *        emissions and the given emission probabilities and transition
 *        probabilities
 *
 * @param start_probs (optionally empty, will be automatically generated)
 *
 * @return the predicted sequence of states for the emissions given
 */
template<typename State, typename Emission, typename Prob = double>
std::vector<State> hmm_predict_sequence(
        std::vector<Emission> observations,
        std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_probs,
        std::unordered_map<std::tuple<State, State>, Prob> transmission_probs,
        std::unordered_map<State, Prob> start_probs = {}) {

    if(start_probs.empty()) {
        start_probs = detail::hmm_default_starting_probs(emission_probs);
    }
    
    detail::hmm_validate_parameters(emission_probs, transmission_probs, start_probs);

    std::vector<State> state_values;
    state_values.reserve(emission_probs.size());

    for(auto& kv : emission_probs) {
        state_values.emplace_back(kv.first);
    }

    const size_t observations_count = observations.size();
    const size_t states_count = state_values.size();

    /* Probability not returned, therefore use log2 to optimize */
    detail::hmm_probability_map_to_log2(emission_probs);
    detail::hmm_transition_map_to_log2(transmission_probs);

    /**
     * Two matrices,
     * - Previous state
     * - Probability
     */
    std::vector<State> mat_states_data(observations_count * states_count);
    std::vector<Prob>  mat_prob_data(observations_count * states_count);

    /**
     * Lambda accessors to mat_prev_data and mat_prob_data
     *
     * Rows -> Observations
     * Columns -> States
     */
    auto mat_states = [&](const size_t& i, const size_t& j) -> State& { return mat_states_data[i * observations_count + j]; };
    auto mat_prob = [&](const size_t& i, const size_t& j) -> Prob&    { return mat_prob_data  [i * observations_count + j]; };

    {   /* Starting probabilities */
        size_t state_idx = 0;
        for(auto& state : emission_probs) {
            mat_prob(state_idx, 0) = start_probs[state.first] + state.second[observations[0]];
            ++state_idx;
        }
    }

    /* For each observation (1..n) */
    for(size_t t = 1; t < observations_count; ++t) {
        /* For every state */
        for(size_t st = 0; st < states_count; ++st) {
            /**
             * Find the maximum product - Calculate the product of the
             * transition from prev->state and the previous state at state_ix
             * current state
             */
            Prob max_product;
            std::tie(max_product, std::ignore) = maximize(
                [&](const size_t& prev_state) {
                    return mat_prob(prev_state, t-1) + transmission_probs[{state_values[prev_state], state_values[st]}];
                },
                states_count
            );
            /* Populate matrix with the highest */
            for(size_t prev_st = 0; prev_st < states_count; ++prev_st) {
                if(mat_prob(prev_st, t-1) + transmission_probs[{state_values[prev_st], state_values[st]}] == max_product) {
                    /* Store the max_product we are maximizing and the probability of this */
                    mat_prob(st, t) = max_product + emission_probs[state_values[st]][observations[t]];
                    /* Store the previous state of the max value for later use in back tracking */
                    mat_states(st, t) = prev_st;
                    break;
                }
            }
        }
    }

    /**
     * Perform back tracking. First find the best probability based on
     * the last outcome
     */
    Prob best_prob;
    std::tie(best_prob, std::ignore) = maximize([&](const size_t& state_idx) {
        return mat_prob(state_idx, observations_count-1);
    }, states_count);
    
    std::cout << "Best path probability: " << best_prob << "\n";
    std::vector<State> predicted_seq(observations_count);

    /* Find the state matching the best_prob found earlier */
    size_t prev_state;
    for(size_t state = 0; state < states_count; ++state) {
        if(mat_prob(state, observations_count-1) == best_prob) {
            prev_state = state;
            predicted_seq[observations_count-1] = state_values[state];
            break;
        }
    }
    /* Actual backtracking loop */
    for(size_t t = observations_count-2; t != static_cast<size_t>(-1); --t) {
        /* Lookup mapped state value */
        predicted_seq[t] = state_values[mat_states(prev_state, t+1)];
        prev_state = mat_states(prev_state, t+1);
    }

    return predicted_seq;
}

/**
 * \brief Viterbri training algoirthm. Takes in emission, transition, and
 *        starting probabilities and iteraitvely improved it until convergence
 *        is reached.
 *
 *
 * @param observations
 * @param initial_probs
 * @param initial_probs
 * @param start_probs
 * @param max_iterations (optional) maximum iteration parameter, default to 100
 * @return a tuple of the the new proposed emission, transition, and starting probabilities
 */
template<typename State, typename Emission, typename Prob = double>
std::tuple<
    std::unordered_map<State, std::unordered_map<Emission, Prob>>,
    std::unordered_map<std::tuple<State, State>, Prob>
>
hmm_viterbi_training(   std::vector<Emission> observations,
                        std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_prob,
                        std::unordered_map<std::tuple<State, State>, Prob> transition_prob,
                        std::unordered_map<State, Prob> start_probs = {},
                        size_t max_iterations = 100) {

    if(start_probs.empty()) {
        start_probs = detail::hmm_default_starting_probs(emission_prob);
    }

    detail::hmm_validate_parameters(emission_prob, transition_prob, start_probs);

    const size_t observation_count = observations.size(); // nr of emissions

    std::vector<State> prev_observed_states;
    bool converged = false;
    size_t iterations = 0;
    
    do {
        /* Get inital observed states given the input */
        auto observed_states = hmm_predict_sequence(
            observations,
            emission_prob,
            transition_prob,
            start_probs
        );

        //std::cout << "new observation: \n" << std::string(observed_states.begin(), observed_states.end()) << "\n";

        /**
         * Evaluate the result and come up with new emission probabilities and
         * new transition probabilities
         */
        std::unordered_map<State, std::unordered_map<Emission, Prob>>   emission_count(emission_prob.size());
        std::unordered_map<std::tuple<State, State>, Prob>              transition_count(transition_prob.size());

        std::unordered_map<State, std::unordered_map<Emission, Prob>>   new_emission_prob(emission_prob.size());        
        std::unordered_map<std::tuple<State, State>, Prob>              new_transition_prob(transition_prob.size());
        //std::unordered_map<State, Prob>                                 new_start_probs(start_probs.size());

        /* Set the previous state as the first state */
        State& prev_st = observed_states[0];

        /* Take first emission into account */
        new_emission_prob[observed_states[0]][observations[0]] += 1;
        //new_start_probs[prev_st] += 1;

        /* Iterate over transitions 1 to observation_count */
        for(size_t i = 1; i < observation_count; ++i) {
            State& state = observed_states[i];
            Emission& emitted = observations[i];

            /* Count the emission for the current state */
            emission_count[state][emitted] += 1;
            
            /* Increment count of transition from prev to curr state */
            transition_count[{prev_st, state}] += 1;

            /* Update count of starting probabilities */
            //new_start_probs[]
            /* update previous */
            prev_st = state;
        }

        /**
         * Lambda that calculates the marginal transitional probability of the
         * current state (sum of all p(curr_st | x), where x in States)
         */
        auto sum_of_prob_transition_to = [&](const State& curr_st) {
            float total = 0;
            for(auto& ntp2 : transition_count) {
                auto [from_st, to_st] = ntp2.first;
                if(curr_st == to_st) {
                    total += transition_count[{from_st, curr_st}];
                }
            }
            return total;
        };

        /**
         * Calculate the new_transition_prob by iterating over every transition
         * count and adding every transition i->j as p(j | i)
         */
        for(auto& ntp : transition_count) {
            auto& [from_st, to_st] = ntp.first;
            auto total = sum_of_prob_transition_to(to_st);
            new_transition_prob[ntp.first] = ntp.second / total;
        }
        /**
         * Calculate the new emission probability by iterating over
         * every emission count and adding every (e,s) as p( e | s)
         */
        for(auto& ec : emission_count) {
            auto total = sum_of_prob_transition_to(ec.first);
            for(auto& em : ec.second) {
                new_emission_prob[ec.first][em.first] = em.second / total;
            }
        }

//        std::cout << "Previous emission probabilities:\n";
//        for(auto& kv : emission_prob) {
//            for(auto& kv2 : kv.second) {
//                std::cout << "\temission_prob[" << kv.first << "][" << kv2.first << "] = " << kv2.second << ";\n";
//            }
//        }
//        std::cout << "New emission probabilities:\n";
//        for(auto& kv : new_emission_prob) {
//            for(auto& kv2 : kv.second) {
//                std::cout << "\temission_prob[" << kv.first << "][" << kv2.first << "] = " << kv2.second << ";\n";
//            }
//        }
//        std::cout << "Previous transition probabilities:\n";
//        for(auto& kv : transition_prob) {
//            std::cout << "\temission_prob[" << std::get<1>(kv.first) << "," << std::get<0>(kv.first) << "] = " << kv.second << ";\n";
//        }
//        std::cout << "New transition probabilities:\n";
//        for(auto& kv : new_transition_prob) {
//            std::cout << "\temission_prob[" << std::get<1>(kv.first) << "," << std::get<0>(kv.first) << "] = " << kv.second << ";\n";
//        }
//        std::cout << "\n\n";
//
        emission_prob = std::move(new_emission_prob);
        transition_prob = std::move(new_transition_prob);
        //start_probs = std::move(new_start_probs);
        
        /**
         * Set should continue variable to true if previous and current states 
         * are not equal
         */
        converged = prev_observed_states == observed_states;
        prev_observed_states = std::move(observed_states);
        
        ++iterations;
        /* Repeat this until training has converged or max iterations elapsed */
    } while(!converged && iterations < max_iterations);

    std::cout << "Trained Viterbi model over " << iterations << " iterations\n";
    /**
     * Return a tuple of the new proposed emission and transition probabilities
     */
    return std::make_tuple(emission_prob, transition_prob);
}


DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_HMM_MODEL_HPP */

