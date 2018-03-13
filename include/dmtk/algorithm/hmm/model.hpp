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
#include <utility>
#include <iterator>
#include <vector>

DMTK_NAMESPACE_BEGIN


template<typename States, typename Result>
auto hmm_prediction(std::vector<States> states, std::vector<Result> results) {
    static_assert(states.size() == results.size(), "States size is equal to results size");
    
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_HMM_MODEL_HPP */

