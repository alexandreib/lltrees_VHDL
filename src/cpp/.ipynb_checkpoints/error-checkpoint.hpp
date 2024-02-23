#ifndef __ERROR_H_INCLUDED__
#define __ERROR_H_INCLUDED__

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>

class lltree_error{
private:
    std::string error_name;

    double _mae(const std::vector<double>& pred, const std::vector<double>& Y) {
        double error = 0;
        for (int i = 0; i < Y.size(); i++){
            error = error + std::abs(pred[i] - Y[i]);
        }
        error = error / Y.size();
        return error; 
    }

    double _mse(const std::vector<double>& pred, const std::vector<double>& Y){
        double error = 0;
        for (int i = 0; i < Y.size(); i++){
            error = error + pow(pred[i] - Y[i]);
        }
        error = error / Y.size();
        return error; 
    }

public:
    lltree_error() : error_name("mae") {};
    lltree_error(std::string error_name) : error_name(error_name) {};

    void set(std::string error_name) {
        this->error_name = error_name;
        std::cout<<"set error_name : " << this->error_name <<std::endl;
    }

    std::string get() {
        std::cout<<"get error_name : " << this->error_name <<std::endl;
        return this->error_name;
    }

    void print() {
        std::cout << "Error available for decisison tree : mae, mse." << std::endl;
    }

    double get(const std::vector<double>& Y){
        if (this->error_name == "mae") {
            return this->_mae(Y);
        }
        else if (this->error_name == "mse") {
            return this->_mse(Y);
        }
        else {
            assert(true && "error_name is not defined");
            return 0; // to avoid the warning during compile
        }
    }


};

#endif // __ERROR_H_INCLUDED__