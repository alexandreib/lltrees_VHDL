#ifndef __METRIC_H_INCLUDED__
#define __METRIC_H_INCLUDED__

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>

class metrics{
private:
    std::string metric_name;

    double _mae(const std::vector<double>& pred, const std::vector<double>& Y) {
        double error = 0;
        for (long unsigned int i = 0; i < Y.size(); i++){
            error = error + std::abs(pred[i] - Y[i]);
        }
        error = error / Y.size();
        return error; 
    }

    double _mse(const std::vector<double>& pred, const std::vector<double>& Y){
        double error = 0;
        for (long unsigned int i = 0; i < Y.size(); i++){
            error = error + pow(pred[i] - Y[i], 2);
        }
        error = error / Y.size();
        return error; 
    }

public:
    metrics() : metric_name("mae") {};
    metrics(std::string metric_name) : metric_name(metric_name) {};

    void set_name(std::string metric_name) {
        this->metric_name = metric_name;
    }

    std::string get_name() {
        return this->metric_name;
    }

    void print() {
        std::cout << "Metrics available for decisison tree : mae, mse." << std::endl;
    }

    double get(const std::vector<double>& pred, const std::vector<double>& Y){
        if (this->metric_name == "mae") {
            return this->_mae(pred, Y);
        }
        else if (this->metric_name == "mse") {
            return this->_mse(pred, Y);
        }
        else {
            assert(true && "metric_name is not defined");
            return 0; // to avoid the warning during compile
        }
    }


};

#endif // __METRIC_H_INCLUDED__