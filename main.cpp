/* Lab03: KNN
 * Omar Thor
 * Aurora Hernandez
 *
 */



#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <map>
#include <cmath>
#include <cctype>
#include <random>
#include <set>
#include <limits>



using namespace std;

const double EPSILON = 0.00000000000001;

/**
 * Shannon's Entropy (core)
 */
template<typename T>
auto entropy_part(T&& freq) {
    return (freq) * std::log2(freq + EPSILON);
}

/**
 * Shannon's Entropy
 * - Summation
 */
template<typename Container>
auto entropy(Container&& container) {
    using val_type = typename Container::value_type;
    return std::accumulate(container.begin(), container.end(), val_type(0), [&](const auto& sum, const auto& val) { return sum - entropy_part(val); });
}

/**
 * Shannon's Entropy
 * - Summation
 */
template<typename Iterator>
auto entropy(Iterator first, Iterator last) {
    return std::accumulate(first, last, decltype(*first)(0), [&](const auto& sum, const auto& val) { return sum - entropy_part(val); });
}

/**
 * Shannon's Entropy
 */
template<typename Iterator, typename UnaryOperator>
auto entropy(Iterator first, Iterator last, UnaryOperator op) {
    return std::accumulate(first, last, decltype(op(*first))(0), [&](const auto& sum, const auto& val) { return sum -(entropy_part(op(val)));});
}

/**
 * Range based normalization of values
 * @param cont
 * @return
 */
template<typename Container>
auto normalize_simple(Container& cont) {
    typename Container::value_type min, max;
    for(auto it = std::begin(cont), end = std::end(cont); it != end; ++it) {
        if(*it > max) {
            max = *it;
        }
        if(*it < min) {
            min = *it;
        }
    }
    auto range = max-min;
    std::transform(std::begin(cont), std::end(cont), std::begin(cont), [&](const auto& x) {
        return (x - min) / range;
    });
}

/**
 * Arbitray distance function of two vector of computable values
 * @param cont1
 * @param cont2
 * @return
 */
template<typename Container>
typename Container::value_type distace_calc (const Container& cont1, const Container& cont2){
    typename Container::value_type sum = 0;
    for(auto
            it_1 = std::begin(cont1),
            it_1_end = std::end(cont1),
            it_2 = std::begin(cont2);
            //it_2_end = std::end(cont2);
            it_1 != it_1_end;
            ++it_1, ++it_2) {
        sum += (*it_1) * (*it_1) - (*it_2) * (*it_2);
    }
    return sqrt(sum);
}

template<typename T>
struct attribute;

using string_attribute = attribute<std::string>;
using float_attribute = attribute<float>;
using double_attribute = attribute<double>;
using label_attribute = string_attribute;


/**
 * Attribute of a model
 */
template<typename T>
struct attribute {

    using value_type = T;

    using value_container_type = std::vector<value_type>;
    using iterator = typename value_container_type::iterator;
    using size_type = size_t;
    using difference_type = typename value_container_type::difference_type;
    using const_reference = typename value_container_type::const_reference;
    using reference = typename value_container_type::reference;

    attribute()
        : number(), attr_name(), data() {}

    attribute(const attribute& other)
        : number(other.number), attr_name(other.attr_name), data(other.data) {}

    attribute(size_t number, const std::string& name)
        : number(number), attr_name(name), data() {}

    attribute& operator=(const attribute& right) {
        if (this == &right) {
            return *this;
        }
        this->number = right.number;
        this->attr_name = right.attr_name;
        this->data = right.data;
        return *this;
    }


    void set_ref(label_attribute* label) {
        this->data = data;
        //this->label_attr = label;
    }

    double entropy_a(double threshold, std::string correct_label) {
        double  total = 0,
                below = 0,
                below_correct = 0,
                below_incorrect = 0,
                above = 0,
                above_correct = 0,
                above_incorrect = 0;

        auto& attr_values = *data;

        auto attr_it = attr_values.begin(), attr_end = attr_values.end();
//        auto label_it = labels->begin(), label_end = labels->end();
//
//        for(;attr_it != attr_end && label_it != label_end; ++attr_it, ++label_it) {
//            const auto& val = *attr_it;
//            const auto& label = *label_it;
//            ++total;
//            if(val <= threshold) {
//                ++below;
//                if(label == correct_label) {
//                    ++below_correct;
//                } else {
//                    ++below_incorrect;
//                }
//            } else {
//                ++above;
//                if(label == correct_label) {
//                    ++above_correct;
//                } else {
//                    ++above_incorrect;
//                }
//            }
//        }

//        auto entropy_d_above = entropy(std::vector<double>{
//            above_correct / (above_correct + above_incorrect),
//            above_incorrect / (above_correct + above_incorrect),
//        });
//        auto entropy_d_below = entropy(std::vector<double>{
//            below_correct / (below_correct + below_correct),
//            below_incorrect / (below_correct + below_correct),
//        });
//        auto res = (above / total) * entropy_d_above + (below / total) * entropy_d_below;
        //return res;
        return 0;
    }

    /**
     * Returns the frequency map of the values in the attribute.
     * @warning can be very large map
     * @return the frequency map
     */
    auto freq_map() {
        using freq_map = std::map<std::string, int>;
        freq_map freq;
        auto total = 0;
        for(auto& val : *data) {
            ++freq[val];
            ++total;
        }
        return freq;
    }

    std::pair<double, double> maxize_entropy_threshold(std::string correct_label, double step = 0.1) {
        auto& values = *data;
        if(!values.empty()) {
            auto min = *std::min_element(values.begin(), values.end());
            auto max = *std::max_element(values.begin(), values.end());
            auto threshold_value = std::numeric_limits<double>::max();
            auto min_entropy = std::numeric_limits<double>::max();
            for(auto current_thres = min, end = max; current_thres <= end; current_thres += step) {
                auto entropy_calc = entropy_a(current_thres, correct_label);
                if(min_entropy > entropy_calc) {
                    min_entropy = entropy_calc;
                    threshold_value = current_thres;
                }
            }
            return {threshold_value, min_entropy};
        } else {
            return {std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity()};
        }
    }

    bool operator>(const attribute& right) const {
        return this->number > right.number;
    }

    size_t num() const {
        return number;
    }

    size_t num() {
        return number;
    }

    std::string name() const {
        return attr_name;
    }

    std::string name() {
        return attr_name;
    }

    template<typename ... Arguments>
    void emplace_back(Arguments&& ... values) {
        data.emplace_back(std::move(values)...);
    }

    void push_back(const value_type& value) {
        data.push_back(value);
    }

    void push_back(value_type&& value) {
        data.emplace_back(std::move(value));
    }

    void resize(const size_t& resize_to) {
        this->data.resize(resize_to);
    }

    reference operator[](const size_t& index) {
        return this->data[index];
    }

    const_reference operator[](const size_t& index) const {
        return this->data[index];
    }

    size_t size() const {
        return this->data.size();
    }

    size_t size() {
        return this->data.size();
    }

    iterator begin() {
        return this->data.begin();
    }

    iterator end() {
        return this->data.end();
    }

protected:
    size_t number;
    std::string attr_name;
    value_container_type data;
};



/**
 * @brief Model
 * Contains attributes and data entries
 *
 * @param file
 * @param first_header
 */
template<typename ValueAttribute = float_attribute>
struct model {

    using value_attribute = ValueAttribute;

    using label_value_type = typename label_attribute::value_type;

    model()
        : file(), value_attrs(), label_attr(), rows() {
    }

    model(const model&) = delete;
    model& operator=(const model&) = delete;

    model(model&& other)
        :   file(std::move(other.file)), value_attrs(std::move(other.value_attrs)),
            label_attr(std::move(other.label_attr)), rows(std::move(other.rows)){
    }

    model(const std::string& file, bool first_header = true)
        : file(file), value_attrs(), label_attr(), rows() {
        open(file, first_header);
    }

    void open(const std::string& file, bool first_header = true) {
        this->file = file;
        std::fstream fs(file.c_str());
        if(fs.good()) {
            std::string line;
            std::string value;
            if(!std::getline(fs, line)) {
                throw std::runtime_error("Failed reading header");
            }
            size_t attr_count = 0;
            if(first_header) {
                std::istringstream iss(line);
                std::string attr_name;
                while(std::getline(iss, attr_name, '\t')) {
                    for(auto it = attr_name.rbegin(), end = attr_name.rend(); it != end; ++it) {
                        if(std::isalnum(*it)) {
                            attr_name.erase(it.base(), attr_name.end());
                            break;
                        }
                    }
                    if(iss.eof()) {
                        label_attr = {attr_count+1, std::move(attr_name)};
                    } else {
                        value_attrs.emplace_back(attr_count+1, std::move(attr_name));
                    }

                    ++attr_count;
                }
                if(value_attrs.empty()) {
                    throw std::runtime_error("No attributes found");
                }
//                for(auto& attr: value_attrs) {
//                    attr.set
//                }
            } else {
                throw "Unsupported no header provided";
            }
            //read rows
            while(std::getline(fs, line)) {
                std::istringstream iss(line);
                for(size_t row_idx = 0; row_idx < attr_count; ++row_idx) {
                    if(iss >> value) {
                        value.erase(std::remove_if(value.begin(), value.end(), [](const auto& v) { return !std::isalnum(v) && v != '.'; }), value.end());
                        if(row_idx == attr_count-1) {
                            this->label_attr.push_back(value);
                        } else {
                            this->value_attrs[row_idx].push_back(std::atof(value.c_str()));
                        }
                    }
                }
                ++rows;
            }
        } else {
            throw new std::runtime_error("Error occurred opening file");
        }
    }

    std::vector<value_attribute>& value_attributes() {
        return value_attrs;
    }

    size_t size() {
        return this->rows;
    }

    std::string source() {
        return file;
    }

    label_attribute& label() {
        return this->label_attr;
    }

    double entropy_d() {
        std::map<std::string, int> freq;
        size_t total = 0;
        for(auto& val : label_attr) {
            ++freq[val];
            ++total;
        }
        double res = entropy(
            freq.begin(),
            freq.end(),
            [&](const auto& val) {
                return val.second / static_cast<double>(total);
            }
        );
        return res;
    }

    void test() {
//        std::vector<
//            std::pair<
//                std::vector<double>::iterator,
//                std::vector<double>::iterator
//            >
//        > iterators;
//        auto attr1 = *(attrs.begin()+1);
//        auto attr2 = *(attrs.begin()+2);
//        auto attr2 = *(attrs.begin()+3);
//        auto attr1_values = attr1.double_value();
//        auto attr1_values = attr1.begin();
//        auto attr1_values = attr1.begin();
////        for(auto end != attr1.end(); it1 != end; ++it1, ++it2, ++it3) {
////            if((*it1).
////        }
    }

    /**
     * Predict the label of the row by using k samples from the model provided
     * 
     * @param model
     * @param row
     * @param k
     * @return 
     */
    std::string_view predict_by_knn(model& model, size_t row, size_t k) {
        using val_type = typename value_attribute::value_type;
        using val_vec = std::vector<val_type>;

        val_vec pos_testing, pos_sample;
        //Read row as vector
        pos_testing.clear();
        for(auto& attr : value_attrs) {
            pos_testing.emplace_back(attr[row]);
        }

        val_type min_dist = std::numeric_limits<val_type>::max();

        std::set<size_t> used_rows;

        std::map<label_value_type, size_t> counters;

        //Test every row for every other row, at current K
        //Check if k closest rows are above the threshold
        //First, find the k closest
        //O(n^2). Could be better?

        //Find K closest points and store accuracy
        for(size_t k_sample = 0; k_sample < k; ++k_sample) {
            size_t min_row = 0;
            for(size_t row_sample = 0; row_sample < rows; ++row_sample) {

                //Don't compare against itself
                if(row_sample != row) {
                    //Check if row has already been sampled
                    if(used_rows.find(row_sample) == used_rows.end()) {

                        pos_sample.clear();
                        for(auto& attr : value_attrs) {
                            pos_sample.emplace_back(attr[row]);
                        }

                        auto res = distace_calc(pos_testing, pos_sample);

                        if(min_dist > res) {
                            min_dist = res;
                            min_row = row_sample;
                        }

                    }
                }
            }
            used_rows.emplace(min_row);
            ++counters[model.label_attr[min_row]];
        }

        auto counter_it = counters.begin(), end = counters.end();
        label_value_type guess = (*counter_it++).first;
        size_t guess_freq = (*counter_it).second;
        for(; counter_it != end; ++counter_it) {
            if((*counter_it).second > guess_freq) {
                guess = (*counter_it).first;
                guess_freq = (*counter_it).second;
            }
        }
        
        return guess;
    }

    /**
     * Find the map optimality for k
     * @param test_runs
     */
    std::map<size_t, float> find_optimal_knn(size_t test_runs) {

        std::map<size_t, std::vector<float>> k_result;
        for(size_t i = 0; i < test_runs; ++i) {

            std::pair<model, model> ms = split(0.75);

            std::cout << "Test run #" << (i+1) << "model split to (train = " << ms.first.size() << ", test = " << ms.second.size() << ")\n";

            //For every K
            for(size_t k = 1, k_max = ms.first.rows; k < k_max; k+=2) { //test only odd numbers

                //std::cout << "Testing K = " << k << ", with N = " << ms.first.rows << "\n";
                
                size_t correct_labels = 0;
                //Test every row for the value K
                for(size_t row = 0, row_max = ms.first.rows; row < row_max; ++row) {
                    if(predict_by_knn(ms.first, row, k) == ms.first.label_attr[row]) {
                        ++correct_labels;
                    }                    
                }
                // Save the correctness of the training                    
                k_result[k].push_back(static_cast<float>(correct_labels) / static_cast<float>(ms.first.rows));
            }

            for(auto& res : k_result) {
                std::cout << "K = " << res.first << " (expected) accuracy: ";
                for(auto& a : res.second) {
                    std::cout << a << "% ";
                }
                std::cout << "\n";
            }
        }
        std::map<size_t, float> ret;
        for(auto& res : k_result) {
            std::cout << "K = " << res.first << " accuracy: ";
            float mean = 0;
            for(auto& a : res.second) {
                 std::cout << a << "% ";
                 mean += a;
            }
            ret[res.first] = mean / res.second.size();
            std::cout << '\n';
        }
        return ret;
    }

    //Copies
    void emplace_back_from(const model& other, size_t index) {
        if(other.value_attrs.size() != this->value_attrs.size()) {
            this->value_attrs.clear();
            for(auto& attr : other.value_attrs) {
                this->value_attrs.emplace_back(attr.num(), attr.name());
            }
            this->label_attr = label_attribute(other.label_attr.num(), other.label_attr.name());
        }
        for(size_t attr_idx = 0, attr_idx_len = other.value_attrs.size(); attr_idx < attr_idx_len; ++attr_idx) {
            value_attrs[attr_idx].emplace_back(other.value_attrs[attr_idx][index]);
        }

        label_attr.emplace_back(other.label_attr[index]);
        ++this->rows;
    }

    std::pair<model, model> split(float fraction) {
        auto part1_size = std::floor(this->rows * fraction);
        auto part2_size = std::ceil(this->rows * (1.0 - fraction));

        std::pair<model, model> res;

        std::set<size_t> used;
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<size_t> dist(0, rows-1);

        for(size_t i = 0, len = part1_size; i < len; ++i) {
            size_t next_idx;
            do {
                next_idx = dist(mt);
            } while(used.find(next_idx) != used.end());
            res.first.emplace_back_from(*this, next_idx);
        }

        for(size_t i = 0, len = part2_size; i < len; ++i) {
            size_t next_idx;
            do {
                next_idx = dist(mt);
            } while(used.find(next_idx) != used.end());
            res.second.emplace_back_from(*this, next_idx);
        }

        return res;
    }

protected:
    std::string file;
    std::vector<value_attribute> value_attrs;
    label_attribute label_attr;
    size_t rows;
};



int main(int argc, char** argv) {

    model<> test("Lab2known.txt", true);
    for(auto& attr : test.value_attributes()) {
        std::cout << attr.size() << std::endl;
    }

    test.find_optimal_knn(1);

//    std::cout << "Random sample is: " << m1.size() << "\n";
//    std::cout << "Random sample is: " << m2.size() << "\n";


    return 0;
}

