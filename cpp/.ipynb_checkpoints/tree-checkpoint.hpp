#pragma once
#include "node.hpp"
#include "wrapper.hpp"
#include "criterion.hpp"


template<class T> class tree {
private:
int id_node;
node<T>* node_0 = NULL;
std::shared_ptr<criterion> crit;
void deleteTree(node<T>* node);

public:
tree(std::shared_ptr<criterion> crit) : id_node(0), crit(crit) {}
~tree();
void fit(const data& base_tr, const std::vector<double>& Y);
void _grow(node<T>& pnode, const data& base_tr, const std::vector<double>& Y, std::vector<int> index);
void get_leaf_value(node<T>& pnode, const std::vector<double>& Y, std::vector<int> index); // Specialized template method
double predict_row(const double* row);
double _traverse(node<T>& pnode, const double * row);

void print();
void _print_tree(node<T>& node); 
void print_node_0();
};

