#ifndef __CRITERIONE_H_INCLUDED__
#define __CRITERIONE_H_INCLUDED__

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>

class criterion{
private:
    std::string criterion_name;

    double _variance(const std::vector<double>& Y) {
        double average = 0, sum = 0;
        for(double value : Y){average = average + value;}
        average = average / Y.size();
        for(double value : Y) {sum += pow(average - value,2);} 
        return sum/ Y.size();  
    }

    double _absolute_error(const std::vector<double>& Y) {
        double average = 0, sum = 0;
        for(double value : Y){average = average + value;}
        average = average / Y.size();
        for(double value : Y) {sum += std::abs(average - value);} 
        return sum/ Y.size();  
    }

    double _gini(const std::vector<double>& Y) {
        double G = 0;
        double p = 0;
        std::vector<double> col = Y;
        std::sort(col.begin(), col.end());
        col.erase(std::unique(col.begin(), col.end()), col.end());

        for (auto &y : col){
            p = std::count(Y.begin(), Y.end(), y);
            std::cout<<y<< " " << p << std::endl;
            G += p*p;
        }
        G = 1 - G;
        return G;
    }
public:
    criterion() : criterion_name("variance") {};
    criterion(std::string criterion_name) : criterion_name(criterion_name) {};

    void set_criterion(std::string criterion_name) {
        this->criterion_name = criterion_name;
    }

    std::string get_criterion() {
        return this->criterion_name;
    }

    void print() {
        std::cout << "Critetion available for decisison tree : variance." << std::endl;
    }

    double get(const std::vector<double>& Y){
        if (this->criterion_name == "variance") {
            return this->_variance(Y);
        }
        else if (this->criterion_name == "absolute_error") {
            return this->_absolute_error(Y);
        }
        else if (this->criterion_name == "gini") {
            return this->_gini(Y);
        }
        else {
            assert(true && "criterion_name is not defined");
            return 0; // to avoid the warning during compile
        }
    }


};

#endif // __CRITERIONE_H_INCLUDED__