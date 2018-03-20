#define BOOST_TEST_MODULE dmtk_algo_entropy
#include <boost/test/unit_test.hpp>

#include "dmtk.hpp"

using namespace dmtk;

BOOST_AUTO_TEST_CASE( test_algo_hmm_probability_of_loaded_dice ) {

    std::cout << "Running: test_algo_hmm_probability_of_loaded_dice\n\n";
    
    auto res = hmm_probability_of<char, int, float>(
        {'F', 'F', 'F', 'F', 'F', 'L', 'L', 'L', 'L', 'F', 'F', 'F', 'F', 'F', 'F'},
        { 1, 2, 1, 5, 6, 2, 1, 6, 2, 4, 6, 2, 3, 6, 4 },
        {
            {
                'F', {
                    {1, 1/6.0f},
                    {2, 1/6.0f},
                    {3, 1/6.0f},
                    {4, 1/6.0f},
                    {5, 1/6.0f},
                    {6, 1/6.0f}
                }
            },
            {
                'L', {
                    {1, 1/8.0f},
                    {2, 1/8.0f},
                    {3, 1/8.0f},
                    {4, 1/8.0f},
                    {5, 1/8.0f},
                    {6, 3/8.0f}   
                }
            }
        },
        {
            { {'F', 'L'}, 0.1},
            { {'L', 'F'}, 0.1},
            { {'L', 'L'}, 0.9},
            { {'F', 'F'}, 0.9}
        }
    );

    BOOST_REQUIRE_CLOSE(2.85087e-15, res, 0.03);
    std::cout << "The result is: " << res << "\n";
    std::cout << "\n\n";
}


BOOST_AUTO_TEST_CASE( test_algo_hmm_predict_dice_loaded_fair ) {

    std::cout << "Running: test_algo_hmm_predict_dice_loaded_fair\n\n";
    std::vector<int> throws = csv<int>("datasets/dice_rolls.txt", false);
    
    auto res = hmm_predict_sequence<char, int, float>(
        //throws,
       { 1, 2, 3, 4, 5, 3, 4, 5, 1, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1},
        {
            {
                'F', {
                    {1, 1/6.0f},
                    {2, 1/6.0f},
                    {3, 1/6.0f},
                    {4, 1/6.0f},
                    {5, 1/6.0f},
                    {6, 1/6.0f}
                }
            },
            {
                'L', {
                    {1, 1/8.0f},
                    {2, 1/8.0f},
                    {3, 1/8.0f},
                    {4, 1/8.0f},
                    {5, 1/8.0f},
                    {6, 3/8.0f}   
                }
            }
        },
        {
            { {'F', 'L'}, 0.1},
            { {'L', 'F'}, 0.1},
            { {'L', 'L'}, 0.9},
            { {'F', 'F'}, 0.9}
        }
    );

    std::cout << "The result is: \n";
    for(auto& v : res) {
        std::cout << v;
    }
    std::cout << "\n\n";    
}

BOOST_AUTO_TEST_CASE( test_algo_hmm_predict_dice_loaded_fair_unknown_trans_and_emission_probs ) {

    std::cout << "Running: test_algo_hmm_predict_dice_loaded_fair_unknown_trans_and_emission_probs\n\n";
    std::vector<int> observations = csv<int>("datasets/dice_rolls.txt", false);
    
//    std::vector<int> observations = {  1, 2, 3, 4, 5, 3, 4, 5, 1, 4, 6, 6, 6, 6, 
//                                        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 
//                                        1, 1, 1, 1, 1};
    
    std::unordered_map<char, std::unordered_map<int, float>> emission_prob{
            {
                'F', {
                    {1, 1/6.0f},
                    {2, 1/6.0f},
                    {3, 1/6.0f},
                    {4, 1/6.0f},
                    {5, 1/6.0f},
                    {6, 1/6.0f}
                }
            },
            {
                'L', {
                    {1, 1/8.0f},
                    {2, 1/8.0f},
                    {3, 1/8.0f},
                    {4, 1/8.0f},
                    {5, 1/8.0f},
                    {6, 3/8.0f}   
                }
            }
    };
          
    std::unordered_map<std::tuple<char, char>, float> transition_prob{
        { {'F', 'L'}, 0.50},
        { {'L', 'F'}, 0.50},
        { {'L', 'L'}, 0.50},
        { {'F', 'F'}, 0.50}
    };
            
    /**
     * Predict the initial sequence for the default probabilities above
     */
    std::vector<char> initial_seq = hmm_predict_sequence(observations, emission_prob, transition_prob);
    
    /**
     * Predict the accuracy of the initial sequence from the initial set of inputs
     */
    auto initial_seq_accuracy = hmm_probability_of(initial_seq, observations, emission_prob, transition_prob);        
    
       
    auto[ better_emission_prob, better_transition_prob] = hmm_viterbi_training(observations, emission_prob, transition_prob);
    
    std::vector<char> result_seq = hmm_predict_sequence(observations, better_emission_prob, better_transition_prob);
    
    auto resultl_seq_prob = hmm_probability_of(result_seq, observations, better_emission_prob, better_transition_prob);
        
    //cout the result
    std::cout << "The results are: \n\tInitial: " << std::string(initial_seq.begin(), initial_seq.end()) <<
                                  "\n\tResult : " << std::string(result_seq.begin(), result_seq.end()) <<
                 "\nwith a probability of:\n\tInitial: " << initial_seq_accuracy <<
                                         "\n\tResult : " << resultl_seq_prob << "\n";
        
    std::cout << "Initial emission probaiblities:\n";
    for(auto& kv : emission_prob) {
        for(auto& kv2 : kv.second) {
            std::cout << "emission_prob[" << kv.first << "][" << kv2.first << "] = " << kv2.second << ";\n";
        }
    }
    std::cout << "Improved emission probaiblities:\n";
    for(auto& kv : better_emission_prob) {
        for(auto& kv2 : kv.second) {
            std::cout << "emission_prob[" << kv.first << "][" << kv2.first << "] = " << kv2.second << ";\n";
        }
    }
    std::cout << "Initial transition probaiblities:\n";
    for(auto& kv : transition_prob) {
        std::cout << "emission_prob[" << std::get<0>(kv.first) << "," << std::get<1>(kv.first) << "] = " << kv.second << ";\n";
    }
    std::cout << "Improved transition probaiblities:\n";
    for(auto& kv : better_transition_prob) {
        std::cout << "emission_prob[" << std::get<0>(kv.first) << "," << std::get<1>(kv.first) << "] = " << kv.second << ";\n";
    }
    std::cout << "\n\n";
}