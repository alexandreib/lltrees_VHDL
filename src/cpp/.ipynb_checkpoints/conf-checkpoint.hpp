#pragma once
#include <string>

struct configuration {
    int verbose;
    std::string mode;
    std::string criterion_name;
    std::string metric_name;
    int epochs;
    int learning_rate;
};

extern configuration conf_global; 

