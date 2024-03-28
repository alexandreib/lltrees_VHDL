#include "metrics.hpp"

double mae::get(std::vector<double>& pred, const double* target) {
    double error = 0;
    for (long unsigned int i = 0; i < pred.size(); i++){
        error = error + std::abs(pred[i] - target[i]);
    }
    return  error / pred.size();;
}

double mse::get(std::vector<double>& pred, const double* target) {
    double error = 0;
    for (long unsigned int i = 0; i < pred.size(); i++){
        error = error + pow(pred[i] - target[i], 2);
    }
    return  error / pred.size();;
}

double accuracy::get(std::vector<int>& pred, const int* target) {
    double accuracy = 0;
    for (long unsigned int i = 0; i < pred.size(); i++){
        if (pred[i] == target[i]) {
            accuracy+=1;
        }
    }
    return accuracy / pred.size(); 
}
