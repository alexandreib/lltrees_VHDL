struct gbt_configuration {
    int verbose;
    std::string mode;
    std::string criterion_name;
    std::string metric_name;
    int epochs;
    double learning_rate;
    int number_of_threads;
    void reset() {
        verbose = 0;
        mode = "regression";
        criterion_name = "variance";
        metric_name = "mae";
        epochs = 1;
        learning_rate = 1;
        number_of_threads = 4; // to create a thread pool
    }
};

struct tree_configuration {
    int max_depth;
    long unsigned int min_leaf_size;
    void reset() {
        max_depth = 8;
        min_leaf_size = 1;
    }
};

extern gbt_configuration conf_gbt; 
extern tree_configuration conf_trees;
