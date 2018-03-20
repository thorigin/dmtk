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

#include <iostream> // @TODO REMOVE

DMTK_NAMESPACE_BEGIN

/**
 * \brief Calculate the probability of the observed_states and the observed
 *        emissions given the emission probability map and transition maps.
 */
template<typename State, typename Emission, typename Prob = fp_type>
auto hmm_probability_of(
        std::vector<State> observed_states,
        std::vector<Emission> observations,
        std::unordered_map<State, std::unordered_map<Emission, Prob>> probs,
        std::unordered_map<std::tuple<State, State>, Prob> trans) {

    BOOST_ASSERT_MSG(observed_states.size() == observations.size(), "observed_states size is equal to observations size");

    size_t nr_of_states;
    //size_t nr_of_results;
    {
        std::set<State> uniqueStates(observed_states.begin(), observed_states.end());
        //std::set<Emission> uniqueResult(observations.begin(), observations.end());

        nr_of_states = uniqueStates.size();
        //nr_of_results = uniqueResult.size();
//
//        BOOST_ASSERT_MSG(
//            nr_of_states*nr_of_states == probs.size(),
//            "Number of possible probability equals the product of the number of observed_states and emissions"
//        );
    }

    /**
     * Initial probability is the selection between the starting states
     * given as 1 over the number of states, the probability of starting at
     * any one of the initial states.
     */
    fp_type res = 1.0 / nr_of_states;

    res *= probs[observed_states[0]][observations[0]];

    for(size_t p = 1, len = observations.size(); p < len; ++p) {
        auto& state_prev = observed_states[p-1];
        auto& state_next = observed_states[p];
        auto& result_next = observations[p];
        auto& prob_of = probs[state_next][result_next];
        auto& prob_of_trans = trans[{state_prev, state_next}];
        res *= prob_of * prob_of_trans;
    }

    return res;
}

/**
 * \brief Predict Hidden Markov Model (HMM) sequence of states by the observed
 *        emissions and the given emission probabilities and transition
 *        probabilities
 *
 * @return the predicted sequence of states for the emissions given
 */
template<typename State, typename Emission, typename Prob = fp_type>
std::vector<State> hmm_predict_sequence(
        std::vector<Emission> observations,
        std::unordered_map<State, std::unordered_map<Emission, Prob>> probs,
        std::unordered_map<std::tuple<State, State>, Prob> trans,
        std::unordered_map<State, Prob> start_probs) {

    BOOST_ASSERT_MSG(!observations.empty(), "observations input must not be empty");
    BOOST_ASSERT_MSG(!probs.empty(), "prob input must not be empty");
    BOOST_ASSERT_MSG(!trans.empty(), "trans input must not be empty");

    std::vector<State> state_values;
    state_values.reserve(probs.size());

    for(auto& kv : probs) {
        state_values.emplace_back(kv.first);
    }

    const size_t observations_count = observations.size();
    const size_t states_count = state_values.size();

    /* Probability not returned, therefore use log2 to optimize */
    detail::hmm_probability_map_to_log2(probs);
    detail::hmm_transition_map_to_log2(trans);

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
        for(auto& state : probs) {
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
                    return mat_prob(prev_state, t-1) + trans[{state_values[prev_state], state_values[st]}];
                },
                states_count
            );
            /* Populate matrix with the highest */
            for(size_t prev_st = 0; prev_st < states_count; ++prev_st) {
                if(mat_prob(prev_st, t-1) + trans[{state_values[prev_st], state_values[st]}] == max_product) {
                    /* Store the max_product we are maximizing and the probability of this */
                    mat_prob(st, t) = max_product + probs[state_values[st]][observations[t]];
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

    std::vector<State> predicted_seq(observations_count);

    /* Find the state matching the best_prob found earlier */
    size_t prev_state;
    for(size_t state = 0; state < states_count; ++state) {
        if(mat_prob(state, observations_count-1) == best_prob) {
            prev_state = state;
            predicted_seq[observations_count-1] = state_values[state];
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
 * \brief Predict Hidden Markov Model (HMM) sequence of states by the observed
 *        emissions and the given emission probabilities and transition
 *        probabilities
 *
 * Starting probabilities are automatically set to 1 / states
 *
 * @return the predicted sequence of states for the emissions given
 */
template<typename State, typename Emission, typename Prob = fp_type>
std::vector<State> hmm_predict_sequence(
        std::vector<Emission> observations,
        std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_probs,
        std::unordered_map<std::tuple<State, State>, Prob> trans) {
    return hmm_predict_sequence<State, Emission, Prob>(
        std::move(observations),
        std::move(emission_probs),
        std::move(trans),
        detail::hmm_default_starting_probs<State, Emission, Prob>(emission_probs)
    );
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
 * @return a tuple of the the new proposed emission, transition, and starting probabilities
 */
template<typename State, typename Emission, typename Prob = fp_type>
std::tuple<
    std::unordered_map<State, std::unordered_map<Emission, Prob>>,
    std::unordered_map<std::tuple<State, State>, Prob>
>
hmm_viterbi_training(   std::vector<Emission> observations,
                        std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_prob,
                        std::unordered_map<std::tuple<State, State>, Prob> transition_prob,
                        std::unordered_map<State, Prob> start_probs) {

    const size_t observation_count = observations.size(); // nr of emissions
    const size_t states_count = emission_prob.size(); // nr of states

    std::vector<State> prev_observed_states;
    bool converged = false;

    do {
        /* Get inital observed states given the input */
        auto observed_states = hmm_predict_sequence(
            observations,
            emission_prob,
            transition_prob,
            start_probs
        );

        /**
         * Evaluate the result and come up with new emission probabilities and
         * new transition probabilities
         */
        std::unordered_map<State, std::unordered_map<Emission, Prob>>   new_emission_prob(emission_prob.size());        
        std::unordered_map<std::tuple<State, State>, Prob>              new_transition_prob(transition_prob.size());
        std::unordered_map<State, Prob>                                 new_start_probs(start_probs.size());

        /* Set the previous state as the first state */
        State& prev_st = observed_states[0];

        /* Take first emission into account */
        new_emission_prob[observed_states[0]][observations[0]] += 1;
        new_start_probs[prev_st] += 1;

        /* Iterate over transitions 1 to observation_count */
        for(size_t i = 1; i < observation_count; ++i) {
            State& state = observed_states[i];
            Emission& emitted = observations[i];

            /* Count the emission for the current state */
            new_emission_prob[state][emitted] += 1;
            
            /* update count of transition from prev to curr state */
            new_transition_prob[{prev_st, state}] += 1;

            /* Update count of starting probabilities */
            //new_start_probs[]
            /* update previous */
            prev_st = state;
        }

        for(auto& ntp : new_transition_prob) {
            
        }

        /* Change counts into probabilities */
        /* For every state */
        for(auto& kv : new_emission_prob) {
            /* divide the number of states emitting value from previous state */
            for(auto& kv2 : kv.second)          { kv2.second /= new_emission_counts[kv.first]; }
        }
        /* divide transition counts by the number of states to get the */
        for(auto& ntp : new_transition_prob)    { ntp.second /= new_transition_prob_ct[std::get<0>(ntp.first)]; }
        /* divide new starting counts by the number of */
        for(auto& nsp : new_start_probs)        { nsp.second /= states_count; }

        /* update emission and transition probabilities */
        emission_prob = std::move(new_emission_prob);
        transition_prob = std::move(new_transition_prob);
        start_probs = std::move(new_start_probs);
        
        /**
         * Set should continue variable to true if previous and current states 
         * are not equal
         */
        converged = prev_observed_states == observed_states;
        prev_observed_states = std::move(observed_states);
        
    } while(!converged); /* Repeat this until training has converged */

    /**
     * Return a tuple of the new proposed emission and transition probabilities
     */
    return std::make_tuple(emission_prob, transition_prob);
}

/**
 * \brief Attempt to come up with higher accuracy emission and transition
 *        probabilities base don the initial emission probability and transition
 *        probabilities provided. The starting prob abilities do not change.
 *
 * @param observations
 * @param initial_probs
 * @param initial_probs
 * @return a tuple of the proposed emission probabilities and the transition
 *         probabilities
 */
template<typename State, typename Emission, typename Prob = fp_type>
std::tuple<
    std::unordered_map<State, std::unordered_map<Emission, Prob>>,
    std::unordered_map<std::tuple<State, State>, Prob>
>
hmm_viterbi_training(   std::vector<Emission> observations,
                        std::unordered_map<State, std::unordered_map<Emission, Prob>> emission_prob,
                        std::unordered_map<std::tuple<State, State>, Prob> transition_prob) {
    return hmm_viterbi_training(
        std::move(observations),
        std::move(emission_prob),
        std::move(transition_prob),
        detail::hmm_default_starting_probs(emission_prob)
    );
}


DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_HMM_MODEL_HPP */

