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

public:
node<T>* node_0 = NULL;
tree(std::shared_ptr<criterion<T>> criterion_tree) : id_node(0), criterion_tree(criterion_tree) {}
~tree();
void fit(const data& base_tr, const std::vector<T>& Y);
void _grow(node<T> &pnode, const data &d, const std::vector<T>& Y, const std::vector<int>& index);
void _calculate_impurity(node<T>& pnode, 
                                const data& tr, 
                                const std::vector<T>& Y, 
                                const std::vector<int>& index, 
                                const int start_col,
                                const int end_col);

// void _calculate_impurity_old(node<T>& pnode, 
//                                 const data& tr, 
//                                 const std::vector<T>& Y, 
//                                 const std::vector<int>& index, 
//                                 const int& index_col);

T get_leaf_value(const std::vector<T>& Y, const std::vector<int>& index); // Specialized template method

std::vector<T> predict(const data &d);

T predict_row(const double *row) const;
T _traverse(const node<T> &pnode, const double *row) const;

void print();
void _print_tree(node<T>& node); 
void print_node_0();
};

