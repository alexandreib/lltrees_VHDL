#pragma once
#include <memory>
class criterion {
public:
virtual double get(const std::vector<int>& Y);
virtual double get(const std::vector<double>& Y);
};

class criterion_variance : public criterion {
public:
virtual double get(const std::vector<double>& Y) override;
};

class criterion_absolute_error : public criterion {
public:
virtual double get(const std::vector<double>& Y) override;
};

class criterion_gini : public criterion {
public:
virtual double get(const std::vector<int>& Y) override;
};

std::shared_ptr<criterion> criterion_Factory(const std::string& type);
