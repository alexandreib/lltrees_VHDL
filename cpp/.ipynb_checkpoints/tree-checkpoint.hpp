#pragma once
#include "node.hpp"
#include "wrapper.hpp"
#include "criterion.hpp"

template<class T> 
class tree {
private:
int id_node;
std::shared_ptr<criterion<T>> criterion_tree;
void deleteTree(node<T> * node);
int numbers_col ;
node<T>* node_0 = NULL;

public:
///////////////////////////////////////// Constructor / Destructor / set / get
tree() : id_node(0) {};
tree(std::shared_ptr<criterion<T>> criterion_tree) : id_node(0), criterion_tree(criterion_tree) {};
~tree();

///////////////////////////////////////// Fit Area
void fit(const XY & base_tr, const std::vector<T> & Y);
void fit(node<T> &pnode, const XY & d, const std::vector<T> & Y, const std::vector<int> & index);
void _calculate_impurity(node<T>& pnode, 
                                const XY& tr, 
                                const std::vector<T>& Y, 
                                const std::vector<int>& index, 
                                double &impurity,
                                const int thread_n);
T get_leaf_value(const std::vector<T>& Y, const std::vector<int>& index);
    
///////////////////////////////////////// Predict Area
    
template<class U> 
std::vector<U> predict(const XY & d) const;
    
template<class U> 
U predict_row(const double * row) const;
    
template<class U> 
U predict_row(const node<T> & pnode, const double * row) const;

void pred_and_add(const XY & d, std::vector<double> & pred);
    
///////////////////////////////////////// Print Area
void print_node_0();
void printBT(const std::string & prefix, const node<T> * pnode, bool isLeft);
void printBT();

///////////////////////////////////////// Save/Load Area
void load(std::string line);
void load(node<T> *& pnode, std::string & line);

void save(std::ofstream & file);
void save(const node<T> * pnode, std::ofstream & file);

};