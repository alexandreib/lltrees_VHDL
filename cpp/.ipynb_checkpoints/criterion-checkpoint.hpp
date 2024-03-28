#pragma once
#include <memory>
#include <map>

class base_criterion 
{
protected:
base_criterion() {};

double get_average(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W = {});

std::unordered_map<int, double> get_proba(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W = {});


public:
virtual double get(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W)  {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual double get(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W) {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
};

class variance : public base_criterion 
{
public:
double get(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W)override;
};

class absolute_error : public base_criterion 
{
public:
double get(const std::vector<double>& Y, const std::vector<int> & index, const std::vector<double> & W) override;
};

class gini : public base_criterion 
{
public:
double get(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W) override;
};

class entropy : public base_criterion
{
public:
double get(const std::vector<int>& Y, const std::vector<int> & index, const std::vector<double> & W) override;
};

