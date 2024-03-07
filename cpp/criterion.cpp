#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include "criterion.hpp"

double variance::get(const std::vector<double>& Y) {
    double average = 0, sum = 0;
    for(auto const & value : Y){average = average + value;}
    average = average / Y.size();
    for(auto const & value : Y) {sum += pow(average - value,2);} 
    return sum/ Y.size();  
}

double absolute_error::get(const std::vector<double>& Y)  {
        double average = 0, sum = 0;
        for(auto const & value : Y){average = average + value;}
        average = average / Y.size();
        for(auto const & value : Y) {sum += std::abs(average - value);} 
        return sum/ Y.size();  
    }

double gini::get(const std::vector<int>& Y) {
    double G = 0;
    double p = 0;
    std::vector col = Y;
    std::sort(col.begin(), col.end());
    col.erase(std::unique(col.begin(), col.end()), col.end());
    for (auto const &y : col){
        p = std::count(Y.begin(), Y.end(), y);
        G += pow(p / Y.size(), 2);
        // p  = p / Y.size();
        // G += p * (1-p);
    }
    // return G;
    return 1 - G;
}


//     double _entropy(const std::vector<T>& Y) {
//         double H = 0;
//         double p = 0;
//         std::vector<T> col = Y;
//         std::sort(col.begin(), col.end());
//         col.erase(std::unique(col.begin(), col.end()), col.end());
//         for (auto &y : col){
//             p = std::count(Y.begin(), Y.end(), y);
//             H -= p*p; //np.log2(p)
//         }
//         return H;
//     }