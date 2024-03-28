#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>

class base_metrics
{
public:
virtual double get(std::vector<int> & pred, const int * target) {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual double get(std::vector<double> & pred, const double * target) {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
};

class mae: public base_metrics 
{
double get(std::vector<double> & pred, const double * target) override;
};

class accuracy: public base_metrics 
{
double get(std::vector<int> & pred, const int * target) override;
};

class mse: public base_metrics 
{
double get(std::vector<double> & pred, const double * target) override;
};
