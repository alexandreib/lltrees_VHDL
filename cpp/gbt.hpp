#pragma once
#include <iomanip>
#include "tree.hpp"

class base_gbt {
public:
std::vector<double> residuals_average; 
virtual ~base_gbt()  =default;
virtual void predict(data& ts) =0;
virtual void fit(const data& tr, const data& va) =0;
};

template<class T> 
class Gbt : public base_gbt {
private:
std::vector<tree<T>*> trees;
public: 
~Gbt() {
    for (auto p : this->trees) {
        delete p;
    } 
    this->trees.clear();
}
void print_epoch_log(int& epoch, double& metric_tr, double& metric_va, double& mean_residuals );
void fit(const data& tr, const data& va);
void pred_and_add(const data& d, const tree<double>& tree, std::vector<double>& pred);
void predict(data& ts) override;

};
