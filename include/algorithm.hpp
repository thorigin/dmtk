#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "element.hpp"
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
#include <unordered_set>
#include <iostream>
#include <sstream>

#define UNUSED(x) (void)x;

/**
 *@file Provides operations on collections of Element (see element.hpp)
 * which provide the ability for the user to use custom data types
 */

//The following is included for hash functionality out of the box
//see https://stackoverflow.com/questions/1250599/how-to-unordered-settupleint-int
namespace std{
    namespace
    {

        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     https://stackoverflow.com/questions/4948780

        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            hash_combine(seed, get<0>(tuple));
          }
        };
    }

    template <typename ... TT>
    struct hash<std::tuple<TT...>>
    {
        size_t
        operator()(std::tuple<TT...> const& tt) const
        {
            size_t seed = 0;
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
            return seed;
        }

    };
}

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
    return entropy(std::begin(cont), std::end(cont));
}


/**
 * Shannon's Entropy gain
 */
//template<typename T>
//auto entropy_gain(T entropy_a, T entropy_d) {
//    return entropy(cont.begin(), cont.end());
//}


namespace detail {

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
    auto apply_normalization(T& x, const T& min, const T& max, arithmetic_atom_tag) {
        return apply_normalization_atom(x, min, max);
    }

    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max, skip_atom_tag) {
        return x;
    }

    template<typename T>
    auto apply_normalization(T& x, const T& min, const T& max) {
        return apply_normalization(x, min, max, select_atom_tag_t<T>{});
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
        used.insert(next_idx);
        res.first.emplace_back(cont[next_idx]);
    }

    for(size_t i = 0, len = split_second_size; i < len; ++i) {
        do {
            next_idx = dist(mt);
        } while(used.find(next_idx) != used.end());
        used.insert(next_idx);
        res.second.emplace_back(cont[next_idx]);
    }

    return res;
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
 * Sample various knn values by splitting a input container into two parts and sampling the statistical success rate of it
 *
 * @param test_runs
 * @return
 */
template<size_t SkipLastN = 1, typename Container>
std::map<size_t, float> sample_knn_values(Container& cont, size_t test_runs = 1, float split_dataset = 0.75, size_t max_k_test = std::numeric_limits<size_t>::max()) {


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

/**
 * Maximizes the K-NN of sample test runs over the provided data set
 */
template<size_t SkipLastN = 1, typename Container>
auto maximize_knn (Container& cont, size_t test_runs = 1, float split_dataset = 0.75, size_t max_k_test = std::numeric_limits<size_t>::max()) {
    auto opt = sample_knn_values<SkipLastN>(cont, test_runs, split_dataset, max_k_test);
    auto res_it = std::max_element(opt.begin(), opt.end(), [](const auto& left, const auto& right) {
        return left.first > right.first;
    });
    return *res_it;
}

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

    std::set<size_t> used;

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
    std::set<size_t> used;

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

/**
 *
 * @param cont the container to calculate the centroid for
 * @return
 */
template<size_t SkipLastN = 0, typename Container>
auto centroid(Container& cont) {
    using value_type = typename Container::value_type;
    value_type center;
    auto size = std::distance(std::begin(cont), std::end(cont));
    using namespace detail;
    for(auto& val : cont) {
        element_add<SkipLastN>(center, val);
    }
    element_div<SkipLastN>(center, size);
    return center;
}

/**
 * Calculates the average distance (euclidean) between all the points in the cointainer
 * and the element provided
 *
 * If the element provided exists in cont, its weight will be considered
 * @param cont
 * @param element
 * @return the average
 */
template<size_t SkipLastN = 0, typename Container, typename Result = float>
Result average_distance_euclidean(const Container& cont, const typename Container::value_type& element) {
    auto it = std::begin(cont),
         end = std::end(cont);
    if(it != end) {
        Result sum = 0;
        for(; it != end; ++it) {
            sum += distance_euclidean<SkipLastN>(*it, element);
        }
        return static_cast<Result>(sum) / static_cast<Result>(cont.size());
    } else {
        return 0;
    }
}


/**
 * Constructs k clusters from selection into a tuple of cluster map a d cluster centroids from the container cont
 *
 * @param cont
 * @param selection The initially selected element that are the seeds to clustering
 * @param k
 * @param cluster_map
 */
template<size_t SkipLastN = 0, typename FromContainer, typename SelectionContainer, typename Random = std::mt19937>
auto create_k_means_clusters_from_selection(FromContainer& cont, SelectionContainer&& selection, Random&& rand = Random(std::random_device{}())) {

    const auto samples_count = cont.size();
    const auto k_clusters = selection.size();
    using value_type = typename FromContainer::value_type;
    using vec_type = std::vector<value_type>;

    /**
     * Initialize centroids, and used_samples containers
     */
    std::unordered_map<size_t, vec_type> cluster_map(k_clusters);
    std::unordered_map<size_t, value_type> cluster_centroid_map(k_clusters);
    std::unordered_set<size_t> used_samples(samples_count);

    if(selection.empty()) {
        throw std::runtime_error("Invalid cluster initialization");
    }

    /**
     * Select random n initial seeds and add them into the k_clusters map
     */
    size_t k_cluster_idx = 0;
    for(auto [origin, origin_idx] : selection) {
       vec_type vec;
       /**
        * Estimate average cluster size as samples * 0.75 / k
        */
       vec.reserve(samples_count * 0.75f / k_clusters);
       /**
        * Move the initial element (the seed) into it own cluster
        */
       vec.emplace_back(origin);
       /**
        * Initialize cluster centroid
        */
       cluster_centroid_map[k_cluster_idx] = centroid<SkipLastN>(vec);
       /**
        * Move temp vec to cluster_map after initialization
        */
       cluster_map.emplace(k_cluster_idx++, std::move(vec));
       used_samples.emplace(origin_idx);
    }

    /**
     * Consider samples_count - k elements and for every element left, find it's closest group
     */
    for(size_t remaining_samples_idx = 0; remaining_samples_idx < samples_count-k_clusters; ++remaining_samples_idx) {

       /**
        * Select 1 random sample from cont that is not in used_samples
        */
       auto [sample, sample_idx] = select_1_random(cont, used_samples, rand);
       size_t closest_cluster_idx = 0;
       auto closest_cluster_dist = distance_euclidean<SkipLastN>(cluster_centroid_map[closest_cluster_idx], sample);
       for(size_t k_check_idx = 1; k_check_idx < k_clusters; ++k_check_idx) {
           auto& cluster_centroid = cluster_centroid_map[k_check_idx];

           /**
            * Calculate the euclidean distance between the cluster and the sample
            */
           auto temp_dist = distance_euclidean<SkipLastN>(cluster_centroid, sample);
           if(temp_dist < closest_cluster_dist) {
               closest_cluster_dist = temp_dist;
               closest_cluster_idx = k_check_idx;
           }
       }
       /**
        * Add element to the closest cluster
        */
       used_samples.emplace(sample_idx);
       auto& cluster_container = cluster_map[closest_cluster_idx];
       cluster_container.emplace_back(sample);
       /**
        * Recalculate cluster centroid
        */
       cluster_centroid_map[closest_cluster_idx] = centroid<SkipLastN>(cluster_container);
    }

    return std::make_tuple(std::move(cluster_map), std::move(cluster_centroid_map));
}

/**
 * Constructs k clusters into cluster_map from the container cont
 * @param cont
 * @param k
 * @param cluster_map
 */
template<size_t SkipLastN = 0, typename FromContainer, typename Random = std::mt19937>
auto create_k_means_clusters(FromContainer& cont, const size_t& k_clusters, Random&& rand = Random(std::random_device{}())) {
    return create_k_means_clusters_from_selection<SkipLastN>(cont, select_n_random(cont, k_clusters, rand), rand);
}

namespace detail {
    /**
     * Optimize given an algorithm over n runs and find the optimal n argument
     * @tparam Algorithm a functor accepting
     * @tparam Comparator the comparison operator for
     */
    template<typename Algorithm, typename Comparator>
    struct optimizer_helper {

        using algorithm_type = Algorithm;
        using comparator_type = Comparator;

        optimizer_helper(algorithm_type && algorithm, comparator_type&& comp, size_t from_n, size_t to_n)
            : algorithm(std::forward<algorithm_type>(algorithm)),
              comparator(std::forward<comparator_type>(comp)),
              from_n(from_n),
              to_n(to_n) {}

        /**
         * Apply (optional) arguments to the algorithm
         * @param arguments
         * @return the optimized result over n
         */
        auto operator()() {
            size_t min_idx = from_n;
            auto min_val = algorithm(from_n);
            for(size_t i = from_n+1, len = to_n; i < len; ++i) {
                auto temp_val = algorithm(i);
                if(comparator(temp_val, min_val)) {
                    min_idx = i;
                    min_val = temp_val;
                }
            }
            return std::make_tuple(std::move(min_val), min_idx);
        }

        algorithm_type algorithm;
        comparator_type comparator;
        size_t from_n, to_n;
    };
}

template<size_t SkipFirstN, typename Container, typename UnaryOp>
auto average(const Container& cont, UnaryOp&& op) {
    using res_type = decltype(op(*cont.begin()));

    res_type sum = 0;
    for(auto& v : cont) {
        sum += op(v);
    }
    return sum / cont.size();
}

/**
 * Optimize the input algorithm based on the comparator (comp) given, over the range [from_n, to_n]
 *
 * @param algorithm the algorithm to optimize
 * @param comp the comparison of the result
 * @param from_n the starting range of n
 * @param to_n the ending range of n (exclusive)
 * @return the most optimal value
 */
template<typename Algorithm, typename Comparator>
auto optimizer(Algorithm&& algorithm, Comparator&& comp, size_t from_n, size_t to_n) {
    return detail::optimizer_helper<Algorithm, Comparator>(
        std::forward<Algorithm>(algorithm),
        std::forward<Comparator>(comp),
        from_n,
        to_n
    )();
}

/**
 * Tests various kmeans values by splitting a input container into two parts and sampling the statistical success rate of it
 *
 * @param test_runs
 * @param max_k_test the maximum number of clusters to test (defaults to the number of data points in the container provided
 * @return the result map
 */
template<size_t SkipLastN = 0, typename Container, typename Random = std::mt19937>
auto sample_kmeans(Container& cont, size_t test_runs = 1, size_t max_k_test = std::numeric_limits<size_t>::max(), Random&& rand = Random(std::random_device{}())) {

    using value_type = typename Container::value_type;
    const auto samples_count = cont.size();
    const auto k_clusters_max = std::min(max_k_test, samples_count+1);

    /**
     * distance from each element to its cluster centroid
     * i.e. what is being minimized (the average)
     */
    //std::map<size_t, std::vector<float>> test_results;

    /**
     * tuple with key that is the elements, and value of the average
     */
    using selection_info = std::tuple<std::vector<value_type>, float>;
    /**
     * list of tuples {k = elements vector, v = average}
     */
    using selection_vector = std::vector<selection_info>;

    /**
     * map of {k = cluster count, v = vector of selection_info}
     */
    std::unordered_map<size_t, selection_vector> selection_proximity(k_clusters_max);

    /**
     * For every k clusters, starting with k = 1 clusters, and increment and measure
     */
    for(size_t k_clusters = 1; k_clusters < k_clusters_max; ++k_clusters) {

        std::cout << "Gathering kmeans samples for K=" << k_clusters << " (samples = " << test_runs << ")\n";
        for(size_t i = 0; i < test_runs; ++i) {

            auto selection = select_n_random(cont, k_clusters, rand);
            std::vector<value_type> selection_values;
            selection_values.reserve(k_clusters);
            for(auto [k, v] : selection) {
                UNUSED(v);
                selection_values.emplace_back(k);
            }

            auto [clusters, centroids] = create_k_means_clusters_from_selection<SkipLastN>(cont, selection, rand);

            auto avg = average<SkipLastN>(clusters, [&](const auto& pair) {
                auto [idx, vec] = pair;
                return average_distance_euclidean<SkipLastN>(vec, centroids[idx]);
            });
//            float sum = 0;
//            for(auto [idx, vec] : clusters) {
//                sum +=
//            }
//            float avg = sum / k_clusters;
            //test_results[k_clusters].emplace_back(avg);

            selection_proximity[k_clusters].emplace_back(selection_values, avg);
        }
    }

    return selection_proximity;
}


/**
 * Tests various kmeans values by splitting a input container into two parts and sampling the statistical success rate of it
 *
 * @param test_runs
 * @param max_k_test the maximum number of clusters to test (defaults to the number of data points in the container provided
 * @return the result map
 */
template<size_t SkipLastN = 0, typename Container, typename Random = std::mt19937>
auto minimize_kmeans(Container& cont, size_t test_runs = 1, size_t max_k_test = std::numeric_limits<size_t>::max()) {
    auto res = sample_kmeans(cont, test_runs,  max_k_test);
    auto res_it = std::max_element(res.begin(), res.end(), [](const auto& left, const auto& right) {
        return std::get<1>(left) > std::get<1>(right);
    });
    return *res_it;
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

#endif /* ALGORITHM_HPP */

