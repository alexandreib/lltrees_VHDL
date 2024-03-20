#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include "criterion.hpp"

double variance::get(const std::vector<double>& Y) 
{
    double average = 0, sum = 0;
    for(auto const & value : Y)
    {
        average = average + value;
    }
    average = average / Y.size();
    for(auto const & value : Y) 
    {
        sum += pow(average - value,2);
    } 
    return sum/ Y.size();  
}

double absolute_error::get(const std::vector<double>& Y)
{
    double average = 0, sum = 0;
    for(auto const & value : Y)
    {
        average = average + value;
    }
    average = average / Y.size();
    for(auto const & value : Y) 
    {
        sum += std::abs(value - average);
    } 
    return sum / Y.size();  
}

double gini::get(const std::vector<int>& Y) {
    double G =0, p=0;
    std::unordered_map<int, double> probas; 
    for (int y : Y) 
    { 
        if (probas.find(y) == probas.end())
        {
            probas[y] = 1;
        }
        else
        {
            probas[y] += 1;
        }
    }
    for (const auto& pair : probas) 
    {       
        p = pair.second / (double) Y.size();
        G += p*(1-p);//pow(pair.second / (double) Y.size(), 2);
    } 
    return G;
}

double entropy::get(const std::vector<int>& Y) {
    double G =0, p=0;
    std::unordered_map<int, double> probas; 
    for (int y : Y) 
    { 
        if (probas.find(y) == probas.end())
        {
            probas[y] = 1;
        }
        else
        {
            probas[y] += 1;
        }
    }
    for (const auto& pair : probas) 
    {       
        p = pair.second / (double) Y.size();
        G += p * std::log(p);
    } 
    return -G;
}
