#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <map>
#include "criterion.hpp"

double variance::get(const std::vector<double>& Y, const std::vector<int>& index, const std::vector<double>& weights) 
{
    double average = 0, sum = 0, sum_weights = 0;
    for(auto const & idx : index) 
    {
        sum_weights +=  weights[idx];
        average = average + (Y[idx] * weights[idx]);
    }
    average = average / sum_weights;
    for(auto const & idx : index) 
    {
        sum += pow(average - Y[idx] ,2) * weights[idx];
    } 
    return sum / sum_weights;  
}

double absolute_error::get(const std::vector<double>& Y, const std::vector<int>& index, const std::vector<double>& weights)  
{
    double average = 0, sum = 0, sum_weights = 0;
    for(auto const & idx : index) 
    {
        sum_weights +=  weights[idx];
        average = average + (Y[idx] * weights[idx]);
    }
    average = average / sum_weights;
    for(auto const & idx : index) 
    {
        sum += std::abs(average - Y[idx] ) * weights[idx];
    } 
    return sum / sum_weights;  
}

double gini::get(const std::vector<int>& Y, const std::vector<int>& index, const std::vector<double>& weights){
    double G = 0, p=0;
    // std::vector<int> col = Y;
    // std::sort(col.begin(), col.end());
    // col.erase(std::unique(col.begin(), col.end()), col.end());

    double weighted_sum = 0;
    std::unordered_map<int, double> my_map;
    for(auto const & idx : index)
    {
        if (my_map.find(Y[idx]) == my_map.end())
        {
            my_map[Y[idx]] = weights[idx] / (double) index.size();
        }
        else
        {
            my_map[Y[idx]] += weights[idx] / (double) index.size();   
        }
        weighted_sum += weights[idx] / (double) index.size();   
    } 
    for ( const auto &val : my_map ) 
    {
        p = val.second / weighted_sum;
        G += p* (1-p);//pow(val.second / weighted_sum  , 2);
        // std::cout << "eeee" << val.first << " " << val.second << " " << weighted_sum << " " <<  G<< std::endl;
    } 
    // std::cout<<"----------------" <<std::endl;
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
