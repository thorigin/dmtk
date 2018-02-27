/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_PARTITION_HPP
#define DMTK_ALGORITHM_PARTITION_HPP

#include <random>
#include <unordered_set>

DMTK_NAMESPACE_BEGIN

/**
 * Split container into two parts
 * @param cont the container to split
 * @param fraction
 * @return
 */
template<typename Container, typename Random = std::mt19937>
std::pair<Container, Container> split(const Container& cont, float split, Random&& rand = Random(std::random_device{}())) {

    auto rows = cont.size();
    auto split_first_size = std::floor(rows * split);
    auto split_second_size = std::ceil(rows * (1.0 - split));

    std::pair<Container, Container> res;

    res.first.reserve(split_first_size);
    res.second.reserve(split_second_size);

    std::unordered_set<size_t> used;

    //Random selection
    std::uniform_int_distribution<size_t> dist(0, rows-1);

    size_t next_idx;
    for(size_t i = 0, len = split_first_size; i < len; ++i) {
        do {
                next_idx = dist(rand);
        } while(used.find(next_idx) != used.end());
        used.insert(next_idx);
        res.first.emplace_back(cont[next_idx]);
    }

    for(size_t i = 0, len = split_second_size; i < len; ++i) {
        do {
            next_idx = dist(rand);
        } while(used.find(next_idx) != used.end());
        used.insert(next_idx);
        res.second.emplace_back(cont[next_idx]);
    }

    return res;
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_PARTITION_HPP */

