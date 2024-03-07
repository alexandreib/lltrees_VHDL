#include "metrics.hpp"

double metrics::get(std::vector<double>& pred,const std::vector<double>& target) {
    std::cout << "End in Base class metric." << std::endl;
    return 0.0;
    }

double metrics::get(std::vector<int>& pred,const std::vector<int>& target) {
    std::cout << "End in Base class metric." << std::endl;
    return 0.0;
    }

double mae::get(std::vector<double>& pred,const std::vector<double>& target) {
    double error = 0;
    for (long unsigned int i = 0; i < pred.size(); i++){
        error = error + std::abs(pred[i] - target[i]);
    }
    std::cout << "mae::get "<< pred[100] << " " << target[100]<< " " << error << std::endl;
    error = error / pred.size();
    return error;
}

double accuracy::get(std::vector<int>& pred,const std::vector<int>& target) {
    return 0.0; 
}
