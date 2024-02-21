#ifndef CRITERION_H
#define CRITERION_H

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>

class criterion{
private:
    std::string criterion_name;

    double _mae(const std::vector<double>& Y) {
        double average = 0, sum = 0;
        for(double value : Y){average = average + value;}
        average = average / Y.size();
        for(double value : Y) {sum += std::abs(average - value);} 
        return sum / Y.size(); 
    }

    double _mse(const std::vector<double>& Y) {
        double average = 0, sum = 0;
        for(double value : Y){average = average + value;}
        average = average / Y.size();
        for(double value : Y) {sum += pow(average - value,2);} 
        return sum / Y.size();  
    }

public:
    criterion() : criterion_name("mae") {};
    criterion(std::string criterion_name) : criterion_name(criterion_name) {};

    void set(std::string criterion_name) {
        this->criterion_name = criterion_name;
        std::cout<<"set criterion_name : " << this->criterion_name <<std::endl;
    }

    std::string get() {
        std::cout<<"get criterion_name : " << this->criterion_name <<std::endl;
        return this->criterion_name;
    }

    void print() {
        std::cout << "Critetion available for decisison tree : mae, mse." << std::endl;
    }

    double get(const std::vector<double>& Y){
        if (this->criterion_name == "mae") {
            return this->_mae(Y);
        }
        else if (this->criterion_name == "mse") {
            return this->_mse(Y);
        }
        else {
            assert(True && "criterion_name is not defined");
            return 0; // to avoid the warning during compile
        }
    }


};

#endif