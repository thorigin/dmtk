#define BOOST_TEST_MODULE dmtk_algo_hmm
#include <boost/test/unit_test.hpp>
#include <iomanip>

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
}

BOOST_AUTO_TEST_CASE( test_algo_hmm_probability_of_loaded_dice_logarithmic ) {

    std::cout << "Running: test_algo_hmm_probability_of_loaded_dice_logarithmic0.\n\n";
    
    auto res = hmm_probability_of<char, int, float>(
        {'F', 'F', 'F', 'F', 'F', 'L', 'L', 'L', 'L', 'F', 'F', 'F', 'F', 'F', 'F'},
        { 1,   2,   1,   5,   6,   2,   1,   6,   2,   4,   6,   2,   3,   6,   4 },
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
        },
        {}, // default starting probs
        hmm_logarithmic_scale_tag{}
    );

    BOOST_REQUIRE_CLOSE(-48.317516326904297, res, 0.01);
    std::cout << "The result is: " << res << "\n";
    std::cout << "\n\n";
}


BOOST_AUTO_TEST_CASE( test_algo_hmm_predict_dice_loaded_fair ) {

    std::cout << "Running: test_algo_hmm_predict_dice_loaded_fair\n\n";
    std::vector<int> throws = csv<int>("datasets/dice_rolls.txt", false);
    
    auto res = hmm_predict_sequence<char, int>(
       throws,      
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

    std::cout << "Most likely state sequence is: " << std::string(res.begin(), res.end()) << "\n";
    
    //switch start to loaded, see the change
    res[0] = 'L';
    
    auto prob = hmm_probability_of(
        res,
        throws,             
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
        },
        {},
        hmm_logarithmic_scale_tag{}
    );
    std::cout << "After changing the first state to L, the log of p(x | pi') is now: " << prob << "\n";
}

BOOST_AUTO_TEST_CASE( test_algo_hmm_predict_dice_loaded_fair_unknown_trans_and_emission_probs ) {

    std::cout << "Running: test_algo_hmm_predict_dice_loaded_fair_unknown_trans_and_emission_probs\n\n";
    std::vector<int> observations = csv<int>("datasets/dice_rolls2.txt", false);
    
    std::unordered_map<char, std::unordered_map<int, float_t>> emission_prob{
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
               {1, 2/8.0f},
               {2, 2/8.0f},
               {3, 1/8.0f},
               {4, 1/8.0f},
               {5, 1/8.0f},
               {6, 1/8.0f}   
           }
       }
    };
          
    std::unordered_map<std::tuple<char, char>, float_t> transition_prob{
        { {'F', 'L'}, 0.1},
        { {'L', 'F'}, 0.1},
        { {'L', 'L'}, 0.9},
        { {'F', 'F'}, 0.9}
    };
            
    /**
     * Predict the initial sequence for the default probabilities above
     */
    std::vector<char> initial_seq = hmm_predict_sequence(observations, emission_prob, transition_prob);
    
    /**
     * Predict the accuracy of the initial sequence from the initial set of inputs
     */
    auto initial_seq_accuracy = hmm_probability_of(initial_seq, observations, emission_prob, transition_prob, {}, hmm_logarithmic_scale_tag{});        
           
    /**
     * Perform viterbi training algorithm on the observations, given the emission and transition probabilities.
     */
    auto[ better_emission_prob, better_transition_prob] = hmm_viterbi_training(observations, emission_prob, transition_prob);
    
    /**
     * Get the most likely sequence after training has been done
     */
    std::vector<char> result_seq = hmm_predict_sequence(observations, better_emission_prob, better_transition_prob);
    
    auto resultl_seq_prob = hmm_probability_of(result_seq, observations, better_emission_prob, better_transition_prob, {}, hmm_logarithmic_scale_tag{});
        
    std::cout << "The results are: \n\tInitial: " << std::string(initial_seq.begin(), initial_seq.end()) <<
                                  "\n\tResult : " << std::string(result_seq.begin(), result_seq.end()) <<
                 "\nwith a probability of:\n\tInitial: " << initial_seq_accuracy <<
                                         "\n\tResult : " << resultl_seq_prob << "\n";
        
    std::cout << "Initial emission probabilities:\n";
    for(auto& kv : emission_prob) {
        for(auto& kv2 : kv.second) {
            std::cout << "\temission_prob[" << kv.first << "][" << kv2.first << "] = " << kv2.second << ";\n";
        }
    }
    std::cout << "Improved emission probabilities:\n";
    for(auto& kv : better_emission_prob) {
        for(auto& kv2 : kv.second) {
            std::cout << "\temission_prob[" << kv.first << "][" << kv2.first << "] = " << kv2.second << ";\n";
        }
    }
    std::cout << "Initial transition probabilities:\n";
    for(auto& kv : transition_prob) {
        std::cout << "\temission_prob[" << std::get<1>(kv.first) << "," << std::get<0>(kv.first) << "] = " << kv.second << ";\n";
    }
    std::cout << "Improved transition probabilities:\n";
    for(auto& kv : better_transition_prob) {
        std::cout << "\temission_prob[" << std::get<1>(kv.first) << "," << std::get<0>(kv.first) << "] = " << kv.second << ";\n";
    }
    std::cout << "result, original = " <<     resultl_seq_prob << ", " << initial_seq_accuracy << "\n";
    std::cout << "\n\n";
}