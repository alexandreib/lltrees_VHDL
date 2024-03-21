#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "tree.hpp"
#include "wrapper.hpp"

class base_gbt {
public:
std::vector<double> residuals_average; 
virtual ~base_gbt() = default;
virtual void predict(data& ts) = 0;
virtual void fit(const data& tr, const data& va) = 0;
virtual void save() = 0;
virtual void load() = 0;
virtual void print() = 0;
void print_epoch_log(int& epoch, double& metric_tr, double& metric_va, double& mean_residuals );
};

template<class T> 
class Gbt : public base_gbt {
private:
std::vector<tree<T>*> trees;
public: 
Gbt() {};
~Gbt();
void fit(const data& tr, const data& va) override;
// void pred_and_add(const data& d, const tree<double>& tree, std::vector<double>& pred);
void predict(data& ts) override;
void save() override;
void load() override; 
void print() override;
};
