#include "conf.hpp"

namespace conf
{
    int number_of_threads = 4;
    int verbose = 0;
    std::string mode = "regression";

    namespace tree 
    {
        int max_depth = 8;
        long unsigned int min_leaf_size = 1;
        std::string criterion_name = "variance";
    };
    
    namespace gbt
    {
        std::string metric_name = "mae";
        int epochs = 1;
        double learning_rate = 1;
    };
}