#pragma once

struct configuration {
    int verbose;
    std::string mode;
    std::string criterion_name;
    std::string metric_name;
};

extern configuration conf_global; 

