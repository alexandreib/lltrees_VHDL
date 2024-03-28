#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>

class base_metrics
{
public:
virtual double get(std::vector<int> & pred, const int * target) {return 0;};
virtual double get(std::vector<double> & pred, const double * target) {return 0;};
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
