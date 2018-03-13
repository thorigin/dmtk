/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_POLYNOMIAL_HPP
#define DMTK_ALGORITHM_POLYNOMIAL_HPP

#include <tuple>
#include <type_traits>
#include <vector>

#include "../config.hpp"
#include "../util/types.hpp"

DMTK_NAMESPACE_BEGIN

template<typename T>
struct polynomial {

    polynomial(size_t degree) : values(degree, 0) {}

    polynomial(std::initializer_list<T>&& init_list)
    : values(std::move(init_list)) {}

    std::vector<T> values;

};

DMTK_NAMESPACE_END


#endif /* DMTK_ALGORITHM_POLYNOMIAL_HPP */

