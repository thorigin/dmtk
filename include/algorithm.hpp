#ifndef NEWFILE_HPP
#define NEWFILE_HPP

#include <tuple>
#include <utility>
#include <cmath>
#include <type_traits>
#include <vector>
#include <tuple>
#include <memory>
#include <fstream>
#include <istream>
#include <numeric>
#include <algorithm>
#include <random>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>
#include <sstream>

/**
 * typedef for tuple
 */
template<typename ... T>
using node = std::tuple<T...>;

namespace detail {
    /**
     * Very small value (the smallest) of T
     */
    template<typename T>
    const T EPSILON = std::numeric_limits<T>::min();
}

/**
 * Shannon's Entropy (core)
 */
template<typename T>
auto entropy_part(T&& freq) {
    return freq * std::log2<T>(freq + detail::EPSILON<T>);
}

/**
 * Shannon's Entropy
 *
 * @param first
 * @param last
 */
template<typename Iterator>
auto entropy(Iterator first, Iterator last) {
    return std::accumulate(first, last, decltype(*first)(0), [&](const auto& sum, const auto& val) { return sum - entropy_part(val); });
}

/**
 * Shannon's Entropy
 *
 * @param first
 * @param last
 * @param op Unary operator
 */
template<typename Iterator, typename UnaryOperator>
auto entropy(Iterator first, Iterator last, UnaryOperator op) {
    return std::accumulate(first, last, decltype(op(*first))(0), [&](const auto& sum, const auto& val) { return sum -(entropy_part(op(val)));});
}

/**
 * Shannon's Entropy
 * @param cont A container
 */
template<typename Container>
auto entropy(Container&& cont) {
    return entropy(cont.begin(), cont.end());
}


/**
 * Shannon's Entropy gain
 */
//template<typename T>
//auto entropy_gain(T entropy_a, T entropy_d) {
//    return entropy(cont.begin(), cont.end());
//}


namespace detail {

    struct min_max_atom_tag {};

    struct min_max_atom_arithmetic_tag : min_max_atom_tag {};

    struct min_max_atom_skip_tag : min_max_atom_tag {};

    template<typename T>
    constexpr bool should_skip_atom_v = std::is_arithmetic<std::decay_t<T>>::value;

    template<typename T>
    using select_atom_tag_t = std::conditional_t<
        should_skip_atom_v<T>,
        min_max_atom_arithmetic_tag,
        min_max_atom_skip_tag
    >;

    template<typename T>
    void find_min_max_atom(const T& value, T& min, T& max) {
        find_min_max_atom(value, min, max, select_atom_tag_t<T>{});
    }

    template<typename T>
    void find_min_max_atom(const T& value, T& min, T& max, min_max_atom_arithmetic_tag) {
        if(value > max) {
            max = value;
        }
        if(value < min) {
            min = value;
        }
    }

    template<typename T>
    void find_min_max_atom(const T& value, T& min, T& max, min_max_atom_skip_tag) {}

    template<typename T>
    void find_min_max(const T& value, T& min, T& max) {
        find_min_max_atom(value, min, max);
    }

    template<typename ... T, size_t ... Indexes>
    void find_min_max_tuple(const std::tuple<T...>& value, std::tuple<T...>& min, std::tuple<T...>& max, std::index_sequence<Indexes...>) {
        (find_min_max_atom(std::get<Indexes>(value), std::get<Indexes>(min), std::get<Indexes>(max)), ...);
    }

    template<typename T, typename ... Rest>
    void find_min_max_vector(const std::vector<T, Rest...>& value, std::vector<T, Rest...>& min, std::vector<T, Rest...>& max) {
        for(    auto it = value.begin(), end = value.end(),
                min_it = min.begin(), min_end = min.end(),
                max_it = max.begin(), max_end = max.end();
                it != end && min_it != min_end && max_it != max_end;
                ++it,
                ++min_it,
                ++max_it
                ) {
            find_min_max_atom(*it, *min_it, *max_it);
        }
    }


    template<size_t skip_last, typename ... T>
    void find_min_max(const std::tuple<T...>& value, std::tuple<T...>& min, std::tuple<T...>& max) {
        find_min_max_tuple(value, min, max, std::make_index_sequence<sizeof...(T) - skip_last>{});
    }

    template<typename T>
    auto apply_normalization_atom(T x, const T& min, const T& max) {
        auto range = max-min;
        if(range != 0) {
            return (x - min) / range;
        } else if(max != 0) {
            return x - min / max;
        } else {
            return x;
        }
    }

    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max) {
        return apply_normalization(x, min, max, select_atom_tag_t<T>{});
    }

    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max, min_max_atom_arithmetic_tag) {
        return apply_normalization_atom(x, min, max);
    }
    
    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max, min_max_atom_skip_tag) {
        return x;
    }

    template<size_t SkipLast, typename T, typename ... Rest>
    auto& apply_normalization(std::vector<T, Rest...>& value, const T& min, const T& max) {
        for(auto it = value.begin(), end = value.end() - SkipLast; it != end; ++it) {
            *it = apply_normalization(*it, min, max);
        }
        return value;
    }

    template<typename ...T, size_t ... Indexes>
    auto& apply_normalization_helper(std::tuple<T...>& tuple, const std::tuple<T...>& min, const std::tuple<T...>& max, std::index_sequence<Indexes...>) {
        ((
            std::get<Indexes>(tuple) = apply_normalization(
                std::get<Indexes>(tuple),
                std::get<Indexes>(min),
                std::get<Indexes>(max)
            )
        ), ...);
        return tuple;
    }

    template<size_t SkipLastN, typename ...T>
    auto& apply_normalization(std::tuple<T...>& tuple, const std::tuple<T...>& min, const std::tuple<T...>& max) {
        return apply_normalization_helper(tuple, min, max, std::make_index_sequence<sizeof...(T)-SkipLastN>{});
    }

}

/**
 * Range based normalization of values
 * It is able to handle tuples, vector, and arithmetic value types
 * If element of type is not arithmetic, it is omitted.
 * If a custom type is defined, provide a specialization point
 * for {@code find_min_max} and {@code apply_normalization}
 *
 * @param cont
 * @tparam skip_last optional, if value type is a vector or a tuple, it skips the number of elements from the right
 * @return
 */
template<size_t SkipLastN = 0, typename Iterator>
void normalize(Iterator start, Iterator end) {
    if(start != end) {
        auto it = start;
        auto min = (*it++),
             max = (*it++);
        for(; it != end; ++it) {
            using namespace detail;
            find_min_max<SkipLastN>(*it, min, max);
        }

        std::transform(start, end, start, [&](auto& x) {
            using namespace detail;
            return apply_normalization<SkipLastN>(x, min, max);
        });
    }
}


/**
 * Range based normalization of values
 * It is able to handle tuples, vector
 * @param cont
 * @return
 */
template<size_t skip_last = 0, typename Container>
void normalize(Container& cont) {
    normalize<skip_last>(std::begin(cont), std::end(cont));
}

namespace detail {

    template<typename ... T, size_t ... Indexes>
    auto distance_euclidean_tuple(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2, std::index_sequence<Indexes...>) {
        return (0.0f + ... + ((std::get<Indexes>(tuple1) - std::get<Indexes>(tuple2)) * (std::get<Indexes>(tuple1) - std::get<Indexes>(tuple2))));
    }
}

/**
 * Euclidean distance function that calculates the distance of values in a two
 * tuples. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @param skip_last Skip the last number of elements in the tuple
 * @return the distance
 */
template<size_t skip_last = 0, typename ...T>
auto distance_euclidean(const std::tuple<T...>& tuple1, const std::tuple<T...>& tuple2){
    return detail::distance_euclidean_tuple(tuple1, tuple2, std::make_index_sequence<sizeof...(T) - skip_last>{});
}

/**
 * Euclidean distance function that calculates the distance of values in a two
 * vectors. The behavior for vectors of unequal length is undefined.
 *
 * @param cont1 the container of the first values
 * @param cont2 the container of the second values
 * @return the distance
 */
template<size_t skip_last = 0, typename Container>
auto distance_euclidean(const Container& cont1, const Container& cont2){
    typename Container::value_type sum = 0;
    for(auto
            it_1 = std::begin(cont1),
            it_1_end = std::end(cont1),
            it_2 = std::begin(cont2);
            it_1  != it_1_end;
            ++it_1, ++it_2) {
        sum += (*it_1 - *it_2) * (*it_1 - *it_2);
    }
    return sqrt(sum);
}
/**
 * Split container into two parts
 * @param cont
 * @param fraction
 * @return
 */
template<typename Container, typename Random = std::mt19937>
std::pair<Container, Container> split(const Container& cont, float split) {

    auto rows = cont.size();
    auto split_first_size = std::floor(rows * split);
    auto split_second_size = std::ceil(rows * (1.0 - split));

    std::pair<Container, Container> res;

    res.first.reserve(split_first_size);
    res.second.reserve(split_second_size);

    std::set<size_t> used;

    //Random selection
    std::random_device rd;
    Random mt(rd());
    std::uniform_int_distribution<size_t> dist(0, rows-1);

    size_t next_idx;
    for(size_t i = 0, len = split_first_size; i < len; ++i) {        
        do {
            next_idx = dist(mt);
        } while(used.find(next_idx) != used.end());
        res.first.emplace_back(cont[next_idx]);
    }

    for(size_t i = 0, len = split_second_size; i < len; ++i) {
        do {
            next_idx = dist(mt);
        } while(used.find(next_idx) != used.end());
        res.second.emplace_back(cont[next_idx]);
    }

    return res;
}

/**
 * By convention, the label is always the last value in a tuple
 * @param tuple
 * @return a reference to the label
 */
template<typename ...T>
auto& get_label(std::tuple<T...>& tuple) {
    return std::get<sizeof...(T)-1>(tuple);
}

/**
 * By convention, the label is always the last value in a tuple
 * @param tuple
 * @return a reference to the label
 */
template<typename ...T>
const auto& get_label(const std::tuple<T...>& tuple) {
    return std::get<sizeof...(T)-1>(tuple);
}

/**
 * By convention, the label is always the last value in a Container
 * @param cont the input container
 * @return a reference to the label
 */
template<typename Container>
auto& get_label(Container& cont) {
    return *(cont.begin() + cont.size()-1);
}


/**
 * By convention, the label is always the last value in a Container
 * @param cont the input container
 * @return a reference to the label
 */
template<typename Container>
const auto& get_label(const Container& cont) {
    return *(cont.begin() + cont.size()-1);
}

/**
 * Predict the label of the row by using k samples from the model provided
 *
 * @param model
 * @param row
 * @param k
 * @return
 */
template<size_t SkipLastN = 0, typename Container, typename Element>
auto predict_by_knn(const Container& sample_model, const Element& test_element, const size_t& k) -> std::decay_t<decltype(get_label(test_element))> {

   std::set<size_t> used_rows;

    using label_type = std::decay_t<decltype(get_label(test_element))>;
    std::map<label_type, size_t> counters;

    //Test every row for every other row, at current K
    //Check if k closest rows are above the threshold
    //First, find the k closest
    auto sample_size = sample_model.size();

    //Find K closest points and store accuracy
    for (size_t k_sample_idx = 0; k_sample_idx < k; ++k_sample_idx) {

        /**
         * The closest sample row index and distance that is unused
         */
        auto closest_sample_row_dist = distance_euclidean<SkipLastN>(test_element, sample_model[0]);
        size_t closest_sample_row_idx = 0;

        for (size_t sample_row_idx = 1; sample_row_idx < sample_size; ++sample_row_idx) {

            //Check if row has not already been sampled
            if(used_rows.find(sample_row_idx) == used_rows.end()) {

                auto temp_sample_dist = distance_euclidean<SkipLastN>(test_element, sample_model[sample_row_idx]);
                
                if(closest_sample_row_dist > temp_sample_dist) {
                    closest_sample_row_dist = temp_sample_dist;
                    closest_sample_row_idx = sample_row_idx;
                }
            }
        }

        //Mark closet sample_row as used
        used_rows.emplace(closest_sample_row_idx);

        //Increment hit counter for label
        const auto& label = get_label(sample_model[closest_sample_row_idx]);
        ++counters[label];
    }

    auto counters_it = counters.begin(), counters_end = counters.end();
    if(counters_it != counters_end) {
        auto guess = (*counters_it++).first;
        size_t guess_freq = (*counters_it++).second;
        for (; counters_it != counters_end; ++counters_it) {
            const auto& c = (*counters_it);
            if(c.second > guess_freq) {
                guess = c.first;
                guess_freq = c.second;
            }
        }
        if(guess.empty()) {
            throw std::runtime_error("No labels matched");
        }
        return guess;
    } else {
        throw std::runtime_error("Prediction not possible");
    }
}

/**
 * Tests various knn values by splitting a input container into two parts and sampling the statistical success rate of it
 * 
 * @param test_runs
 * @return
 */
template<size_t SkipLastN = 1, typename Container>
std::map<size_t, float> test_knn_values(Container& cont, size_t test_runs = 1, float split_dataset = 0.75, size_t max_k_test = std::numeric_limits<size_t>::max()) {


    std::map<size_t, std::vector<float>> test_model_predictions;
    for(size_t i = 0; i < test_runs; ++i) {

        auto [sample_data, test_data] = split(cont, split_dataset);
        auto sample_data_size = sample_data.size();
        auto test_data_size = test_data.size();
        auto k_max = std::min(max_k_test, sample_data_size);

        //For every K
        for(size_t k = 1; k < k_max; k+=2) { //test only odd numbers

            size_t correct_labels = 0;
            //Test every row for the value K
            for(size_t test_idx = 0, test_count = test_data_size; test_idx < test_count; ++test_idx) {

                //Attempt to predict test_row with k nearest neighbours
                const auto& predicated_label = predict_by_knn<SkipLastN>(sample_data, test_data[test_idx], k);
                const auto& expected_label = get_label(test_data[test_idx]);

                if(predicated_label == expected_label) {
                    ++correct_labels;
                }

            }
            // Save the correctness of the training
            test_model_predictions[k].push_back(static_cast<float>(correct_labels) / static_cast<float>(test_data_size));
        }
    }

    std::map<size_t, float> ret;
    for(auto& res : test_model_predictions) {
        float mean = 0;
        for(const auto& a : res.second) {
             mean += a;
        }
        ret[res.first] = mean / res.second.size();
    }

    return ret;
}


namespace detail {

    template<typename InputStream, typename ...T, size_t ... Indexes>
    auto& csv_read_tuple_indexes(std::tuple<T...>& tuple, InputStream& is, std::index_sequence<Indexes...>) {
        return (is >> ... >> std::get<Indexes>(tuple));
    }

    template<typename InputStream, typename ...T>
    auto& csv_read_tuple(std::tuple<T...>& tuple, InputStream& is) {
        return csv_read_tuple_indexes(tuple, is, std::index_sequence_for<T...>{});
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


#endif /* NEWFILE_HPP */

