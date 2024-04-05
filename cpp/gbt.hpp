#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <set> 
#include <vector>
#include "wrapper.hpp"
#include "tree.hpp"


class base_gbt 
{
public:
std::vector<double> residuals_average; 

virtual ~base_gbt() = default;
virtual void print() = 0;
virtual void save() = 0;
virtual void load() = 0;
virtual void fit(const XY & tr, const XY & va) = 0;
virtual void predict(XY & ts) = 0;

// Classification Virtual :
virtual std::vector<int> extract_pred_from_proba(const std::vector<std::unordered_map<int, double>> probas) const {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual std::vector<std::unordered_map<int, double>>  get_proba(const XY & d, const std::vector<double> models_weights) const {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual std::vector<std::unordered_map<int, double>>  get_proba(const XY & d) const {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual void predict_proba(XY & d)  {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual std::unordered_map<int, double> init_map_from_clases() const  {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual std::vector<int> get_predict(const XY & d, const std::vector<double> models_weights) const  {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };
virtual std::vector<int> get_predict(const XY & d) const  {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); };

void print_epoch_log(int & epoch, double & metric_tr, double & metric_va, double & mean_residuals );
};

template<class T> 
class gbt : public base_gbt 
{
protected:
std::vector<tree<T>*> trees;
public: 
gbt();
~gbt();
void print() override;
void save() override;
void load() override; 
};

class regression : public gbt<double>
{
private:
public: 
void fit(const XY & tr, const XY & va) override;
void predict(XY & ts) override;
};

class classification : public gbt<int>
{
private:
std::set<int> classes; 
std::vector<double> models_weights;
public: 
void fit(const XY & tr, const XY & va) override;
void predict(XY & ts) override;

std::vector<int> extract_pred_from_proba(const std::vector<std::unordered_map<int, double>> probas) const override;
std::vector<std::unordered_map<int, double>>  get_proba(const XY & d, const std::vector<double> models_weights) const override;
std::vector<std::unordered_map<int, double>>  get_proba(const XY & d) const override;
void predict_proba(XY & d) override;
std::unordered_map<int, double> init_map_from_clases() const override;
std::vector<int> get_predict(const XY & d, const std::vector<double> models_weights) const override;
std::vector<int> get_predict(const XY & d) const override;
};

class classic_classification : public classification
{
private:
public: 
};