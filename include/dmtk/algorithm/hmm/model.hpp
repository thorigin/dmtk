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

#include <cmath>
#include <utility>
#include <iterator>
#include <vector>
#include <initializer_list>
#include <set>

#include "../cache/pairwise.hpp"

DMTK_NAMESPACE_BEGIN


template<typename State, typename Result, typename Prob = fp_type>
auto hmm_probability_of(
        std::vector<State>&& states,
        std::vector<Result>&& results,
        std::unordered_map<std::tuple<State, Result>, Prob>&& probs,
        std::unordered_map<std::tuple<State, State>, Prob>&& trans) {

    BOOST_ASSERT_MSG(states.size() == results.size(), "States size is equal to results size");


    size_t nr_of_states;
    size_t nr_of_results;
    {
        std::set<State> uniqueStates(states.begin(), states.end());
        std::set<Result> uniqueResult(results.begin(), results.end());

        nr_of_states = uniqueStates.size();
        nr_of_results = uniqueResult.size();

//        BOOST_ASSERT_MSG(
//            trans.size() == std::factb,
//            "States size is equal to results size");
        BOOST_ASSERT_MSG(
            nr_of_states*nr_of_results == probs.size(),
            "Number of listed probability equals the product of the number of unique states and results"
        );
    }

    /**
     * Initial probability is the selection between the starting states
     * given as 1 over the number of states, the probability of starting at
     * any one of the initial states.
     */
    fp_type res = 1.0 / nr_of_states;

    //auto cache = make_pairwise_cache<size_t, Prob>();

    res *= probs[{states[0], results[0]}];

    for(size_t p = 1, len = results.size(); p < len; ++p) {


        auto& state_prev = states[p-1];
        auto& state_next = states[p];

        auto& result_next = results[p];
        auto& result_prev = results[p-1];

        auto& prob_of = probs[{state_next, result_next}];
        auto& prob_of_trans = trans[{state_prev, state_next}];

        res *= prob_of * prob_of_trans;

    }

    return res;
}

template<typename State, typename Result, typename Prob = fp_type>
auto hmm_probability_of(
        std::initializer_list<State> states,
        std::initializer_list<Result> results,
        std::initializer_list<std::tuple<State, Result, Prob>> probs,
        std::initializer_list<std::tuple<State, State, Prob>> trans) {
    std::unordered_map<std::tuple<State, Result>, Prob> map_probs;
    std::unordered_map<std::tuple<State, State>, Prob> map_trans;
    for(auto& [i, j, k] : probs) {
        map_probs.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(i, j),
            std::forward_as_tuple(k)
        );
    }
    for(auto& [i, j, k] : trans) {
        map_trans.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(i, j),
            std::forward_as_tuple(k)
        );
    }
    return hmm_probability_of(
        std::vector<State>(states),
        std::vector<Result>(results),
        std::move(map_probs),
        std::move(map_trans)
    );
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_HMM_MODEL_HPP */

