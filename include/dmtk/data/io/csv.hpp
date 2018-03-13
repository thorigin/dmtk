/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_DATA_IO_CSV_HPP
#define DMTK_DATA_IO_CSV_HPP

#include "dmtk/element/element.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

DMTK_NAMESPACE_BEGIN

namespace detail {

    template<typename InputStream, typename ...T, size_t ... Indexes>
    auto& csv_read_tuple_indexes(std::tuple<T...>& tuple, InputStream& is, std::index_sequence<Indexes...>) {
        return (is >> ... >> std::get<Indexes>(tuple));
    }

    template<typename InputStream, typename ...T>
    auto& csv_read_tuple(std::tuple<T...>& tuple, InputStream& is) {
        return csv_read_tuple_indexes(tuple, is, std::index_sequence_for<T...>{});
    }

    template<typename ...T, size_t FirstIndex, size_t... RestIndexes>
    void tuple_to_csv_line_helper_2(std::ostream& os, std::tuple<T...>& tuple, std::index_sequence<FirstIndex, RestIndexes...>) {
        os << std::get<FirstIndex>(tuple);
        ((os << ", " << std::get<RestIndexes>(tuple)), ...);
        os << '\n';
    }

    template<typename ...T>
    void tuple_to_csv_line_helper(std::ostream& os, std::tuple<T...>& tuple) {
        tuple_to_csv_line_helper_2(os, tuple, std::index_sequence_for<T...>{});
    }

}

/**
 * Write container out as CSV
 * @param out
 * @param cont
 * @return
 */
template<typename Container>
void csv_write(std::ostream& os, Container& cont) {
    for(auto& v : cont) {
        detail::tuple_to_csv_line_helper(os, v);
    }
}

template<typename ... Types>
auto csv(const std::string& file, bool first_line_header = true) {
    std::fstream fs(file, std::ios::in);
    if(fs.is_open() && fs.good()) {

        using node_type = node<Types...>;

        std::vector<node_type> res;
        std::string line;

        if(first_line_header && !std::getline(fs, line)) {
            throw std::runtime_error("Failed reading header");
        }

        //read rows
        while(std::getline(fs, line)) {
            std::istringstream iss(line);
            node_type new_tuple;
            if(!detail::csv_read_tuple(new_tuple, iss)) {
                throw std::runtime_error("Failing reading into tuple");
            }
            res.emplace_back(std::move(new_tuple));
        }
        return res;
    } else {
        throw new std::runtime_error("Error occurred opening file");
    }
}

DMTK_NAMESPACE_END

#endif /* DMTK_DATA_IO_CSV_HPP */
