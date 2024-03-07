#pragma once
#include <memory>
#include <map>

class criterion {
public:
virtual double get(const std::vector<int>& Y);
virtual double get(const std::vector<double>& Y);
};

class variance : public criterion {
public:
virtual double get(const std::vector<double>& Y) override;
};

class absolute_error : public criterion {
public:
virtual double get(const std::vector<double>& Y) override;
};

class gini : public criterion {
public:
virtual double get(const std::vector<int>& Y) override;
};
