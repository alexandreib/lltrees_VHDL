#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <iomanip>
#include "wrapper.hpp"
#include "tree.hpp"


class Gbt {
public:
int epochs;
std::vector<double> residuals; 
double learning_rate; 
Gbt() : epochs(16) {};
virtual void fit(const data& tr, const data& va) = 0;
virtual std::vector<double> predict(const data& ts) = 0;
};

class classification : public Gbt {
public:
private :
std::vector<tree<int>*> trees; 
virtual void fit(const data& tr, const data& va) override;
virtual std::vector<double> predict(const data& ts) override;
void pred_and_add(const data& ts, tree<int>& tree, std::vector<double>& results);
};

class regression : public Gbt {
public:
private :
std::vector<tree<double>*> trees; 
virtual void fit(const data& tr, const data& va) override;
virtual std::vector<double> predict(const data& ts) override;
void pred_and_add(const data& ts, tree<double>& tree, std::vector<double>& results);
};

Gbt* gbt_Factory(const std::string& type);