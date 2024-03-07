#include "metrics.hpp"


double mae::get(std::vector<double>& pred,const double* target) {
    double error = 0;
    for (long unsigned int i = 0; i < pred.size(); i++){
        error = error + std::abs(pred[i] - target[i]);
    }
    std::cout << "mae::get "<< pred[100] << " " << target[100]<< " " << error << std::endl;
    error = error / pred.size();
    return error;
}

double accuracy::get(std::vector<int>& pred,const int* target) {
    return 0.0; 
}
