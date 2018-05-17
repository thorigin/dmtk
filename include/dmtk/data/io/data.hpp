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
#include <boost/algorithm/string/predicate.hpp>

DMTK_NAMESPACE_BEGIN

/**
 * \file Provides rudimentary data importation and export
 *
 * \TODO handle locales better, more data type, abstract better
 */


enum file_type {
    csv_type,
    tsv_type,
    detect_type
};

namespace detail {

    file_type detect_file_type(const std::string& file, const file_type& suggested = file_type::detect_type) {
        file_type use;
        if(suggested == detect_type) {
            /**
             * Simple non-pluggable support for now
             * \TODO support pluggable handling of format
             */
            if(boost::algorithm::ends_with(file, ".csv")) {
                use = csv_type;
            } else  if(boost::algorithm::ends_with(file, ".tsv")) {
                use = tsv_type;
            } else {
                throw std::runtime_error("Unable to infer file type from file name");
            }
        } else {
            use = suggested;
        }
        return use;
    }

    auto file_type_to_separator(const file_type& type) {
        switch(type) {
            case tsv_type:
                return '\t';
            case csv_type:
            default:
                return ',';
        }
    }

    template<typename Element, typename InputStream>
    void load_tuple_indexes_by_separator(Element& element, InputStream& is, const char& separator, std::string& buffer) {
        std::getline(is, buffer, separator);
        std::istringstream buffer_iss(buffer);
        if(!(buffer_iss >> element)) {
            throw std::runtime_error("Failed loading row");
        }
    }

    template<typename InputStream, typename ...T, size_t ... Indexes>
    auto& load_tuple_indexes(std::tuple<T...>& tuple, InputStream& is, std::index_sequence<Indexes...>, const char& separator) {
        std::string buffer;
        (load_tuple_indexes_by_separator(std::get<Indexes>(tuple), is, separator, buffer), ...);
        return is;
    }

    template<typename InputStream, typename ...T>
    auto& load_tuple(std::tuple<T...>& tuple, InputStream& is, const char& separator) {
        return load_tuple_indexes(tuple, is, std::index_sequence_for<T...>{}, separator);
    }

    template<typename ...T, size_t FirstIndex, size_t... RestIndexes>
    void tuple_to_line_helper_2(std::ostream& os, std::tuple<T...>& tuple, std::index_sequence<FirstIndex, RestIndexes...>, const char& separator) {
        os << std::get<FirstIndex>(tuple);
        ((os << separator << std::get<RestIndexes>(tuple)), ...);
        os << '\n';
    }

    template<typename ...T>
    void tuple_to_line_helper(std::ostream& os, std::tuple<T...>& tuple, const file_type& type) {
        auto separator = file_type_to_separator(type);
        tuple_to_line_helper_2(os, tuple, std::index_sequence_for<T...>{}, separator);
    }

    struct load_into_container_tag {};
    struct load_tuple_into_container_tag : load_into_container_tag{};
    struct load_value_into_container_tag : load_into_container_tag{};

    template<typename Container>
    void load_into_container(std::istream& fs, Container& res, const file_type& type, load_tuple_into_container_tag) {
        using value_type = typename Container::value_type;
        std::string line;
        auto separator = file_type_to_separator(type);
        while(std::getline(fs, line)) {
            std::istringstream iss(line);
            value_type value;
            if(!detail::load_tuple(value, iss, separator)) {
                throw std::runtime_error("Failing reading into tuple");
            }
            res.emplace_back(std::move(value));
        }
    }

    template<typename Container>
    void load_into_container(std::istream& fs, Container& res, const file_type& type, load_value_into_container_tag) {
        using value_type = typename Container::value_type;
        auto separator = file_type_to_separator(type);
        std::string line;
        while(std::getline(fs, line)) {
            std::istringstream iss(line);
            value_type value;

//            while(std::getline(is, buffer, separator)) {
//                std::istringstream buffer_iss(buffer);
//                if(!(buffer_iss >> element)) {
//                    throw std::runtime_error("Failed loading row");
//                }
//                if(!(iss >> value)) {
//                    throw std::runtime_error("Failing reading value");
//                }
//                res.emplace_back(std::move(value));
//            }
        }
    }

    template<typename Container>
    void load_into_container(std::istream& fs, Container& res, const file_type& type) {
        return load_into_container(
            fs,
            res,
            type,
            std::conditional_t<
                is_tuple_v<typename Container::value_type>,
                load_tuple_into_container_tag,
                load_value_into_container_tag
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
void save(std::ostream& os, Container& cont) {
    for(auto& v : cont) {
        detail::tuple_to_line_helper(os, v);
    }
}

/**
 * @brief Load data from a file into the provided vector
 */
template<typename Container>
void load_into(const std::string& file, Container& container, bool first_line_header = true, const file_type& type = detect_type) {
    file_type use_type = detail::detect_file_type(file, type);
    std::fstream fs(file, std::ios::in);
    if(fs.is_open() && fs.good()) {
        std::string line;
        if(first_line_header && !std::getline(fs, line)) {
            throw std::runtime_error("Failed reading header");
        }
        using value_type = typename Container::value_type;
        std::vector<value_type> res;;
        detail::load_into_container(fs, container, use_type);
    } else {
        throw new std::runtime_error("Error occurred opening file");
    }
}


/**
 * @brief Load csv data from a file into the provided vector
 */
template<typename Container>
void csv_into(const std::string& file, Container& container, bool first_line_header = true) {
    load_into(file, container, first_line_header, csv_type);
}

/**
 * @brief Load tsv data from a file into the provided vector
 */
template<typename Container>
void tsv_into(const std::string& file, Container& container, bool first_line_header = true) {
    load_into(file, container, first_line_header, tsv_type);
}

template<typename ... Types>
auto load(const std::string& file, bool first_line_header = true, const file_type& type = file_type::detect_type) {

    static_assert(sizeof...(Types) > 0, "Provide one or more types");

    file_type use_type = detail::detect_file_type(file, type);

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
        detail::load_into_container(fs, res, use_type);
        return res;
    } else {
        throw new std::runtime_error("Error occurred opening file");
    }
}

/**
 * \brief Shorthand for load(..., ..,  csv)
 */
template<typename ... Types>
auto csv(const std::string& file, bool first_line_header = true) {
    return load<Types...>(file, first_line_header, file_type::csv_type);
}

/**
 * \brief Shorthand for load(..., ..,  tsv)
 */
template<typename ... Types>
auto tsv(const std::string& file, bool first_line_header = true) {
    return load<Types...>(file, first_line_header, file_type::tsv_type);
}


DMTK_NAMESPACE_END

#endif /* DMTK_DATA_IO_CSV_HPP */

