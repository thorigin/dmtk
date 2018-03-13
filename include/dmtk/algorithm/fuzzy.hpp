/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_ALGORITHM_FUZZY_HPP
#define DMTK_ALGORITHM_FUZZY_HPP

#include <tuple>
#include <type_traits>
#include <vector>
#include <cmath>

#include "../config.hpp"
#include "../types.hpp"

DMTK_NAMESPACE_BEGIN


//template<auto ... XYZ>
//struct fuzzy_membership {
//
//
//    std::tuple<XYZ...> values;
//};
//template<typename ... Memberships>
//struct fuzzy_box {
//
//
//
//    std::tuple<Memberships...> values;
//};

//
//struct fuzzy_intersection_product {
//    template<typename ... T>
//    auto operator()(const T& ... values ) {
//
//    }
//};
//
//struct fuzzy_intersection_min {
//    template<typename ... T>
//    auto operator()(const T& ... values ) {
//        return std::min({values...});
//    }
//};
//
//struct fuzzy_union_sum {
//    template<typename ... T>
//    auto operator()(const T& ... values ) {
//
//    }
//};
//
//struct fuzzy_union_max {
//    template<typename ... T>
//    auto operator()(const T& ... values ) {
//        return std::max({values...});
//    }
//};
//
//
//template<auto ... XYZ>
//auto and_min(const fuzzy_membershi<XYZ>p& left, const fuzzy_membershi<XYZ>& right) {
//    return fuzzy_intersection_product()(left, right);
//}
//
//template<auto ... XYZ>
//auto and_prod(const fuzzy_membershi<XYZ>p& membership, const fuzzy_membershi<XYZ>& membership) {
//    return fuzzy_intersection_min
//}

//struct fuzzy_monotonic_selection {
//
//    auto operator()() {
//
//    }
//};

struct fuzzification {

};

struct defuzzification {

};
DMTK_NAMESPACE_END


#endif /* DMTK_ALGORITHM_FUZZY_HPP */

