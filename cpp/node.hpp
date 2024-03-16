#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

template <typename T> class tree; // for friend class

template<class T> class node{
private:
node<T>* l_node;
node<T>* r_node;

public:
node() :  l_node(NULL), r_node(NULL), isleaf(true), level(0), id_node(0), l_size(0), r_size(0), size(0), index_col(0), impurity(std::numeric_limits<T>::max()), threshold(0), leaf_value(0) {}

node(int size) :  l_node(NULL), r_node(NULL), isleaf(true), level(0), id_node(0), l_size(0), r_size(0), size(size), index_col(0), impurity(std::numeric_limits<T>::max()), threshold(0), leaf_value(0) {}

node(int level, int id_node, int size, double impurity) :  l_node(NULL), r_node(NULL), isleaf(true), level(level), id_node(id_node), l_size(0), r_size(0),  size(size), index_col(0), impurity(impurity), threshold(0), leaf_value(0) {}
friend class tree<T>;   

bool isleaf;
int level, id_node, l_size, r_size, size, index_col;
double impurity, threshold;
T  leaf_value;

void set_children(node<T>* left, node<T>* right);
node& get_l_children() const;
node& get_r_children() const;
void print();
void write() ;
};
