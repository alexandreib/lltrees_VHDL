#pragma once
#include <memory>
#include <map>

class base_criterion 
{
protected:
base_criterion() { };

public:
virtual double get(const std::vector<int> & Y) {return 0;};
virtual double get(const std::vector<double> & Y) {return 0;};
};

class variance : public base_criterion 
{
public:
double get(const std::vector<double> & Y) override;
};

class absolute_error : public base_criterion 
{
public:
double get(const std::vector<double> & Y) override;
};

class gini : public base_criterion 
{
public:
double get(const std::vector<int> & Y) override;
};

class entropy : public base_criterion
{
public:
double get(const std::vector<int> & Y) override;
};

