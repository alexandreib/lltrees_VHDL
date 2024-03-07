#pragma once
#include <iomanip>
#include "tree.hpp"

class Gbt {
public:
std::vector<double> residuals_average; 
virtual ~Gbt() {}
virtual void fit(const data& tr, const data& va) = 0;
virtual std::vector<double> predict(const data& ts) = 0;
void print_epoch_log(int& epoch, double & metric_tr, double & metric_va, double residuals_average);
};

class classification : public Gbt {
private :
std::vector<tree<int>*> trees; 

public: 
~classification() {
    for (auto p : this->trees) {
        delete p;
    } 
    this->trees.clear();
    // std::cout<<"~classification"<<std::endl;
}
virtual void fit(const data& tr, const data& va) override;
virtual std::vector<double> predict(const data& ts) override;
void pred_and_add(const data& ts, tree<int>& tree, std::vector<double>& results);
};

class regression : public Gbt {
private :
std::vector<tree<double>*> trees; 

public:
~regression() {
    for (auto p : this->trees) {
        delete p;
    } 
    this->trees.clear();
    // std::cout<<"~regression"<<std::endl;
}
virtual void fit(const data& tr, const data& va) override;
virtual std::vector<double> predict(const data& ts) override;
void pred_and_add(const data& ts, tree<double>& tree, std::vector<double>& results);
};
