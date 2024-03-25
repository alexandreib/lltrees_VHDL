#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <set> 
#include "tree.hpp"
#include "wrapper.hpp"

class base_gbt {
public:
std::vector<double> residuals_average; 
virtual ~base_gbt() = default;
virtual void predict(XY & ts) = 0;
virtual void fit(const XY & tr, const XY & va) = 0;
virtual void save() = 0;
virtual void load() = 0;
virtual void print() = 0;
virtual void predict_proba(XY & d) = 0;
void print_epoch_log(int & epoch, double & metric_tr, double & metric_va, double & mean_residuals );

};

template<class T> 
class Gbt : public base_gbt {
private:
std::vector<tree<T>*> trees;
std::set<int> classes; 
public: 
Gbt();
~Gbt();
void fit(const XY & tr, const XY & va) override;
void predict(XY & ts) override;
void save() override;
void load() override; 
void print() override;
void predict_proba(XY & d) override;
};
