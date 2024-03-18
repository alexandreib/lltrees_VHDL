#pragma once
#include <memory>
#include <map>

template<class T> class criterion {
public:
virtual double get(const std::vector<T>& Y, const std::vector<int>& index, const std::vector<double>& weights) = 0;
};

class variance : public criterion<double> {
public:
double get(const std::vector<double>& Y, const std::vector<int>& index, const std::vector<double>& weights) override;
};

class absolute_error : public criterion<double> {
public:
double get(const std::vector<double>& Y, const std::vector<int>& index, const std::vector<double>& weights) override;
};

class gini : public criterion<int> {
public:
double get(const std::vector<int>& Y, const std::vector<int>& index, const std::vector<double>& weights) override;
};

