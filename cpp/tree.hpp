#pragma once
#include "node.hpp"
#include "wrapper.hpp"
#include "criterion.hpp"


template<class T> class tree {
private:
int id_node;
node<T>* node_0 = NULL;
std::shared_ptr<criterion<T>> criterion_tree;
void deleteTree(node<T>* node);

public:
tree(std::shared_ptr<criterion<T>> criterion_tree) : id_node(0), criterion_tree(criterion_tree) {}
~tree();
void fit(const data& base_tr, const T* Y);
void _grow(node<T> &pnode, const data &base_tr, const T* Y, std::vector<int> index);
void get_leaf_value(node<T> &pnode, const T* Y, std::vector<int> index); // Specialized template method
double predict_row(const double *row);
double _traverse(node<T> &pnode, const double *row);

void print();
void _print_tree(node<T>& node); 
void print_node_0();
};

