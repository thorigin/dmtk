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
#include <c++/6/bits/uniform_int_dist.h>



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
        
    }
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
    
    attribute(int number, std::string&& name)
        : number(number), attr_name(std::move(name)), data() {}

    void set_ref(label_attribute* label) {
        this->data = data;
        this->label_attr = label;
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
    
    size_t num() {
        return number;
    }
    
    std::string name() {
        return attr_name;
    }
    
    template<typename ... Arguments>
    void emplace_back(Arguments&& ... values) {
        data.emplace_back(std::move(values)...);
    }

    reference operator[](const size_t& index) {
        return this->data[index];
    }
    
    const_reference operator[](const size_t& index) const {
        return this->data[index];
    }
    
    auto begin() {
        return this->data.begin();
    }
    
    auto end() {
        return this->data.end();
    }     
    
protected:
    int number;
    std::string attr_name;
    value_container_type data;
    label_attribute* label_attr;
};


struct node {
    node* parent() {
        return parent_node;
    }

    std::vector<node*> children() {
        return childs;
    }
    
private:
    node* parent_node;
    std::vector<node*> childs;
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
    
    model()
        : file(), value_attrs(), label_attr(), rows() {        
    }
    
    model(const std::string& file, bool first_header = true)
        : file(file), value_attrs(), label_attr(), rows() {  
        open(file, first_header);
    }
    
    void open(const std::string& file, bool first_header = true) {
        this->file = file;
        std::fstream fs(file.c_str());
        int attr_count = 1;
        if(fs.good()) {
            std::string line;
            std::string value;
            std::getline(fs, line);
            auto attr_count = 0;
            if(first_header) {
                std::istringstream iss(line);
                std::string attr_name;
                while(std::getline(iss, attr_name, '\t')) {
                    size_t crop_end = attr_name.size()-1;
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
            } else {
                throw "Unsupported no header provided";
            }
            //read rows
            std::vector<std::string>* last = nullptr;
            while(std::getline(fs, line)) {                
                std::istringstream iss(line);                                               
                for(size_t row_idx = 0; row_idx < attr_count; ++row_idx) {                      
                    if(iss >> value) {                     
                        value.erase(std::remove_if(value.begin(), value.end(), [](const auto& v) { return !std::isalnum(v) && v != '.'; }), value.end());
                        if(row_idx == attr_count-1) {
                            this->label_attr.emplace_back(std::move(value));
                        } else {                            
                            this->value_attrs[row_idx].emplace_back(std::atof(value.c_str()));
                        }                        
                    }
                   ++rows;
                }
                
            }
            //Set reference of attribute to each values
//            for(auto it = values.begin(), end = values.end(); it != end; ++it) {
//                (*it).first->set_ref(&(*it).second, last);
//            }
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
    
    void find_optimal_knn(size_t test_runs) {
        auto correct = 0;
        auto incorrect = 0;
        for(size_t i = 0; i < test_runs; ++i) {
            for(size_t k = 1, k_max = rows; k < k_max; ++k) {                
                for(size_t row = 0, row_max = rows; row < row_max; ++row) {
                    for(auto& attr : value_attrs) {
                        
                    }
                }
            }
        }
    }
    
    //Copies
    void copy(model& other, size_t index) {
        if(other.value_attrs.size() != this->value_attrs.size()) {
            this->value_attrs.clear();
            for(auto& attr : other.value_attrs) {
                this->value_attrs.emplace_back(attr.num(), attr.name());
            }
            this->label_attr = other.label_attr;
        }
        for(size_t i = 0, i_size = rows; i < i_size; ++i) {            
            value_attrs[i][index] = other.value_attrs[i][index];
        }
        label_attr[index] = other.label_attr[index];
    }
    
    std::pair<model, model> split(float fraction) {
        auto first = std::floor(this->rows * fraction);
        auto second = std::ceil(this->rows * (1.0 - fraction));
        
        std::pair<model, model> res;
        
        std::set<size_t> used;
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<size_t> dist(0, rows);
        
        for(size_t i = 0, i_size = first; i < i_size; ++i) {            
            size_t next_idx;
            do {
                next_idx = dist(mt); 
            } while(used.find(next_idx) != used.end());
            res.first.copy(*this, next_idx);            
        }
        for(size_t i = first, i_size = second; i < i_size; ++i) {            
            size_t next_idx;
            do {
                next_idx = dist(mt); 
            } while(used.find(next_idx) != used.end());
            res.second.copy(*this, next_idx);
        }
        
        return res;
    }

protected:
    std::string file;
    std::vector<value_attribute> value_attrs;   
    label_attribute label_attr;
    size_t rows;
};

/*
 * Distance between two points
 * 
 */
//double distance (const point & apoint) const {
  //  const double x1_coordinate, y1_coordinate, x2_
    
//}

struct points
{
    float x, y;
    points (float p1 = 0, float p1 = 0) : x(p1), y(p1) { }
    
};

//typedef std:: vector<points> pointVector;
//typedef std:: map<int, pointVector> mapPointvector;


float distace_calc (const points& p1, const points& p2){
    float diff_Y = p1.y - p2.y;
    float diff_X = p1.x - p2.x;
    return sqrt((diff_Y*diff_X) + (diff_X * diff_X));
}

//void distance_display ( const mapPointvector& val, int val_1, int val_2) {
    
//}


int main(int argc, char** argv) {

    model<> m("Lab1trainingData.txt", true);
//    std::cout << std::setprecision(9);
//    std::string correct_label = "I.versicolor";
//    std::cout << "Created model from: " << m.source() << std::endl;
//    std::cout << "Attribute count: " << m.value_attributes().size() << std::endl;
//    std::cout << "Row count: " << m.size() << std::endl;
//    std::cout << "Entropies of labels (D): " << m.entropy_d() << std::endl;
//    std::cout << "Entropy of attributes: \n";
//    std::pair<attribute*, double> attr_choice{nullptr,0.0};
//    for(    auto    it = m.value_attributes().begin(),
//                    end = m.value_attributes().end();
//            it != end;
//            ++it) {
//        auto a = *it;
//        if(std::next(it) != end) {
//            auto ent_calc = a.maxize_entropy_threshold(correct_label);
//            auto gain = m.entropy_d() - ent_calc.second;
//            std::cout <<    "\tAttribute: \"" << a.name() <<
//                            "\"\n\t\tpivot point = " << ent_calc.first <<
//                            "\n\t\tentropy = " << ent_calc.second <<
//                            "\n\t\tgain(d, a) = " << gain << '\n';
//
//            if(attr_choice.first) {
//                if(attr_choice.second < ent_calc.second) {
//                    attr_choice.first = &a;
//                    attr_choice.second = ent_calc.second;
//                }
//            } else {
//                attr_choice.first = &a;
//                attr_choice.second = ent_calc.second;
//            }
//        }
//    }

    //Construct decision tree
    //std::cout << "\n\tBased on results found, attribute: " << attr_choice.first->name() << " is a better choice\n";

    model<float_attribute> test("Lab1testingData.txt", true);
    test.split(test.size() / 2);


    return 0;
}

