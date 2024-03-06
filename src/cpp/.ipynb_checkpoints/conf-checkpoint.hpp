struct gbt_configuration {
    int verbose;
    std::string mode;
    std::string criterion_name;
    std::string metric_name;
    int epochs;
    int learning_rate;
    gbt_configuration() : verbose(0), mode("regression"), criterion_name("variance"), metric_name("mae"), epochs(1) , learning_rate(1) {}
    void reset() {
        verbose = 0;
        mode = "regression";
        criterion_name = "variance";
        metric_name = "mae";
        epochs = 1;
        learning_rate = 100;
    }
};

struct tree_configuration {
    int max_depth;
    long unsigned int min_leaf_size;
    tree_configuration(): max_depth(8), min_leaf_size(1) {}
    void reset() {
        max_depth = 8;
        min_leaf_size = 1;
    }
};

extern gbt_configuration conf_gbt; 
extern tree_configuration conf_trees;