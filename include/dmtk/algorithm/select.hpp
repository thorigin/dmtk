/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_SELECT_HPP
#define DMTK_ALGORITHM_SELECT_HPP

#include <random>
#include <vector>
#include <unordered_set>
#include <tuple>
#include <memory>


DMTK_NAMESPACE_BEGIN


/**
 * Selects N number of samples from the container
 *
 * @param cont the container to select samples from
 * @param n the number of samples to select
 * @param not_in (Optional) container container possibly containing
 * samples from cont which should be excluded from selection
 * @return a container of tuples containing the selected element and their indexes in the FromContainer
 */
template<typename FromContainer, typename NotInContainer, typename Random = std::mt19937>
auto select_n_random(FromContainer& cont, size_t n, const NotInContainer& not_index, Random&& rand = Random(std::random_device{}())) {

    std::vector<std::tuple<typename FromContainer::value_type, typename FromContainer::size_type>> selected;
    selected.reserve(n);

    const auto rows = cont.size();

    std::unordered_set<size_t> used;

    //Random selection
    std::uniform_int_distribution<size_t> dist(0, rows-1);

    size_t next_idx;
    for(size_t i = 0; i < n; ++i) {
        do {
            next_idx = dist(rand);
        } while(
            used.find(next_idx) != used.end() &&
            not_index.find(next_idx) != not_index.end()
        );
        used.insert(next_idx);
        selected.emplace_back(std::forward_as_tuple(cont[next_idx], next_idx));
    }

    return selected;
}


/**
 * Selects N number of samples from the container
 *
 * @param cont the container to select samples from
 * @param n the number of samples to select
 * @return a container of tuples containing the selected element and their indexes in the FromContainer
 */
template<typename FromContainer, typename Random = std::mt19937>
auto select_n_random(FromContainer& cont, size_t n, Random&& rand = Random(std::random_device{}())) {
    std::unordered_set<typename FromContainer::size_type> not_index;
    return select_n_random(cont, n, not_index, std::forward<Random>(rand));
}

/**
 * Selects N number of samples from the container into another container
 *
 * The algorithm will verify that an element does not already exist in the into container
 *
 * @param cont the container to select samples from
 * @param n the number of samples to select
 * @return a tuple containing the element selected and the index of the element in the FromContainer
 */
template<typename FromContainer, typename NotInContainer, typename Random = std::mt19937>
auto select_1_random(FromContainer& cont, const NotInContainer& not_index, Random&& rand = Random(std::random_device{}())) {

    const auto rows = cont.size();
    //Random selection
    std::uniform_int_distribution<size_t> dist(0, rows-1);

    size_t next_idx;
    do {
        next_idx = dist(rand);
    } while(not_index.find(next_idx) != not_index.end());

    return std::forward_as_tuple(cont[next_idx], next_idx);
}

/**
 * Selects N number of samples from the container into another container
 *
 * The algorithm will verify that an element does not already exist in the into container
 *
 * @param cont the container to select samples from
 * @param n the number of samples to select
 * @return a tuple containing the element selected and the index of the element
 */
template<typename FromContainer, typename Random = std::mt19937>
auto& select_1_random(FromContainer& cont, Random&& rand = Random(std::random_device{})) {
    std::unordered_set<typename FromContainer::size_type> not_index;
    return select_1_random(cont, not_index, std::forward<Random>(rand));
}


/**
 * Selects N number of samples from the container into another container
 *
 * The algorithm will verify that an element does not already exist in the into container
 *
 * @param cont the container to select samples from
 * @param n the number of samples to select
 * @return the resulting selection container
 */
template<typename FromContainer, typename IntoContainer, typename Random = std::mt19937>
void select_n_random_into(FromContainer& cont, size_t n, IntoContainer& into, Random&& rand = Random(std::random_device{}())) {
    into.reserve(into.size() + n);

    auto rows = cont.size();

    //Fast lookup of used values
    std::unordered_set<size_t> used;

    //Random selection
    std::uniform_int_distribution<size_t> dist(0, rows-1);

    size_t next_idx;
    for(size_t i = 0; i < n; ++i) {
        do {
            if(used)
            next_idx = dist(rand);
        } while(
            used.find(next_idx) != used.end() &&
            into.find(cont[next_idx]) != into.end()
        );
        used.insert(next_idx);
        into.emplace_back(cont[next_idx]);
    }
}

DMTK_NAMESPACE_END

#endif /* DMTK_ALGORITHM_SELECT_HPP */

