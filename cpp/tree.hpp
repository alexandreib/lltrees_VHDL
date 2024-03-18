#pragma once
#include "node.hpp"
#include "wrapper.hpp"
#include "criterion.hpp"


template<class T> 
class tree {
private:
int id_node;
std::shared_ptr<criterion<T>> criterion_tree;
void deleteTree(node<T>* node);
int numbers_col ;

protected:
node<T>* node_0 = NULL;

public:
///////////////////////////////////////// Constructor / Destructor / set / get
tree() : id_node(0) {};
tree(std::shared_ptr<criterion<T>> criterion_tree) : id_node(0), criterion_tree(criterion_tree) {}
~tree();

///////////////////////////////////////// Fit Area
void fit(const data& base_tr, 
            const std::vector<T>& Y, 
            const std::vector<double>& weights);
    
void _grow(node<T> &pnode, 
            const data &d, 
            const std::vector<T>& Y, 
            const std::vector<double>& weights);
    
void _calculate_impurity(node<T>& pnode, 
                                const data& tr, 
                                const std::vector<T>& Y, 
                                const int thread_n, 
                                const std::vector<double>& weights);

///////////////////////////////////////// Predict Area
std::vector<T> predict(const data &d);
T predict_row(const double *row) const;
T _traverse(const node<T> &pnode, const double *row) const;
    
///////////////////////////////////////// Print Area
void print_node_0();
void printBT(const std::string& prefix, const node<T>* pnode, bool isLeft);
void printBT();

///////////////////////////////////////// Save/Load Area
void load(std::string line);
void load(node<T>*& pnode, std::string& line);

void save(std::ofstream& file);
void save(const node<T>* pnode, std::ofstream& file);

};

class tree_classification : public tree<int> {
public:
tree_classification() : tree () {}
tree_classification(std::shared_ptr<criterion<int>> criterion_tree) : tree (criterion_tree) {}

std::vector<std::unordered_map<int, double>> predict_proba(const data &d);
std::unordered_map<int, double> predict_proba_row(const double* row) const;
std::unordered_map<int, double> _traverse_proba(const node<int>& pnode, const double * row) const;
    
};