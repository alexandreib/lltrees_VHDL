#include <string>

namespace conf
{
    extern int number_of_threads;
    extern int verbose;
    extern std::string mode;

    namespace tree 
    {
        extern int max_depth;
        extern long unsigned int min_leaf_size;
        extern std::string criterion_name;
    };
    
    namespace gbt
    {
        extern std::string metric_name;
        extern int epochs;
        extern double learning_rate;
    };
}