#pragma once
#include <memory>
#include <map>

template<class T> class criterion {
public:
virtual double get(const std::vector<T>& Y) = 0;
};

class variance : public criterion<double> {
public:
double get(const std::vector<double>& Y) override;
};

class absolute_error : public criterion<double> {
public:
double get(const std::vector<double>& Y) override;
};

class gini : public criterion<int> {
public:
double get(const std::vector<int>& Y) override;
};

class entropy : public criterion<int> {
public:
double get(const std::vector<int>& Y) override;
};

