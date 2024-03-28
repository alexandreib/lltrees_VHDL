#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include "criterion.hpp"

inline double base_criterion::get_average(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W)
{
    double average = 0;
    for(auto const & idx : index)
    {
        average = average + Y[idx];
    }
    return average / index.size();
}

inline double variance::get(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W) 
{
    double sum = 0;
    double average = this->get_average(Y, index, W);
    for(auto const & idx : index)
    {
        sum += pow(average - Y[idx],2);
    } 
    return sum/ index.size();  
}

inline double absolute_error::get(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W)
{
    double sum = 0;
    double average = this->get_average(Y, index, W);
    for(auto const & idx : index)
    {
        sum += std::abs(Y[idx] - average);
    } 
    return sum / index.size();  
}

inline std::unordered_map<int, double> base_criterion::get_proba(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W)
{
    std::unordered_map<int, double> probas; 
    double sum_weights = 0;
    for(auto const & idx : index)
    { 
        if (probas.find(Y[idx]) == probas.end())
        {
            probas[Y[idx]] = W[idx];
        }
        else
        {
            probas[Y[idx]] += W[idx];
        }
        sum_weights += W[idx];
    }
    for (auto & pair : probas) 
    {
        probas[pair.first] = pair.second / sum_weights;
    }
    return probas;
}

inline double gini::get(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W) 
{
    double G = 1;
    std::unordered_map<int, double> probas = this->get_proba(Y, index, W); 
    for (const auto & pair : probas) 
    {       
        // G += p*(1-p);
        G -= pow(pair.second, 2);
    } 
    return G;
}

inline double entropy::get(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W) 
{
    double G =0;
    std::unordered_map<int, double> probas = this->get_proba(Y, index, W); 
    for (const auto & pair : probas) 
    {       
        G += pair.second * std::log(pair.second);
    } 
    return -G;
}