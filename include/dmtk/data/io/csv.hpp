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

    struct read_into_container_tag {};
    struct read_tuple_into_container_tag : read_into_container_tag{};
    struct read_value_into_container_tag : read_into_container_tag{};
    
    template<typename Container>
    void csv_read_into_container(std::istream& fs, Container& res, read_tuple_into_container_tag) {
        using value_type = typename Container::value_type;
        std::string line;
        while(std::getline(fs, line)) {
            std::istringstream iss(line);
            value_type value;
            if(!detail::csv_read_tuple(value, iss)) {
                throw std::runtime_error("Failing reading into tuple");
            }
            res.emplace_back(std::move(value));
        }
    }

    template<typename Container>
    void csv_read_into_container(std::istream& fs, Container& res, read_value_into_container_tag) {
        using value_type = typename Container::value_type;
        std::string line;
        while(std::getline(fs, line)) {
            std::istringstream iss(line);
            value_type value;
            if(!(iss >> value)) {
                throw std::runtime_error("Failing reading value");
            }
            res.emplace_back(std::move(value));
        }
    }
    
    template<typename Container>
    void csv_read_into_container(std::istream& fs, Container& res) {
        return csv_read_into_container(
            fs,
            res,
            std::conditional_t<
                is_tuple_v<typename Container::value_type>,
                read_tuple_into_container_tag,
                read_value_into_container_tag
            >{}
        );
    }
}

/**
 * @brief Write container out as CSV
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

/**
 * @brief Write from CSV file into the provided vector
 */
template<typename Container>
auto csv_into(const std::string& file, Container& container, bool first_line_header = true) {
    std::fstream fs(file, std::ios::in);
    if(fs.is_open() && fs.good()) {                     
        std::string line;
        if(first_line_header && !std::getline(fs, line)) {
            throw std::runtime_error("Failed reading header");
        }
        using value_type = typename Container::value_type;
        std::vector<value_type> res;;
        detail::csv_read_into_container(fs, container);
    } else {
        throw new std::runtime_error("Error occurred opening file");
    }
}

template<typename ... Types>
auto csv(const std::string& file, bool first_line_header = true) {
    
    static_assert(sizeof...(Types) > 0, "Provide one or more types");
    
    std::fstream fs(file, std::ios::in);
    if(fs.is_open() && fs.good()) {
       
        std::string line;

        if(first_line_header && !std::getline(fs, line)) {
            throw std::runtime_error("Failed reading header");
        }

        /**
         * If a single type is specified, there's no need to wrap it ina tuple
         * unwrap and read the values
         */
        using value_type = std::conditional_t<
            sizeof...(Types) == 1,
            std::tuple_element_t<0, std::tuple<Types...>>,
            node<Types...>
        >;
        std::vector<value_type> res;
        detail::csv_read_into_container(fs, res);
        return res;
    } else {
        throw new std::runtime_error("Error occurred opening file");
    }
}

DMTK_NAMESPACE_END

#endif /* DMTK_DATA_IO_CSV_HPP */

