#pragma once
#include "node.hpp"
#include "wrapper.hpp"
#include "criterion.hpp"
#include "threadpool.hpp"

template<class T> 
class tree {
private:
std::shared_ptr<base_criterion> criterion_tree;
void deleteTree(node<T> * node);
node<T> * node_0 = NULL;
ThreadPool * pool;


public:
// Constructor / Destructor 
tree();
tree(std::shared_ptr<base_criterion> criterion_tree, ThreadPool * pool) : criterion_tree(criterion_tree), pool(pool){};
~tree();

// Fit Area
void fit(const XY & base_tr, 
        const std::vector<T> & Y, 
        const std::vector<double> & W = {});
    
void fit(node<T> & pnode, 
        const XY & base_tr,
        const std::vector<T> & Y, 
        const std::vector<int> & index, 
        const std::vector<double> & W);
    
void split(node<T>& pnode, 
        const XY & tr, 
        const std::vector<T> & Y, 
        const std::vector<int> & index, 
        const std::vector<double> & W,
        const int index_col,
        double & impurity,
        std::vector<int> & l_index, 
        std::vector<int> & r_index,
        double & l_impurity, 
        double & r_impurity);

// Predict Area
template<class U> 
std::vector<U> predict(const XY & d) const;
    
template<class U> 
U predict_row(const double * row) const;
    
template<class U> 
U predict_row(const node<T> & pnode, const double * row) const;

void pred_and_add(const XY & d, std::vector<double> & pred);
void pred_and_add(const XY & d, std::vector<std::unordered_map<int, double>> & pred, const double & model_weight);
    
// Print Area
void print_node_0();
void printBT(const std::string & prefix, const node<T> * pnode, bool isLeft);
void printBT();

// Save/Load Area
void load(std::string line);
void load(node<T> *& pnode, std::string & line);

void save(std::ofstream & file);
void save(const node<T> * pnode, std::ofstream & file);
    
};