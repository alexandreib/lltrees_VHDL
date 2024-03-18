#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <map>


template <typename T> 
class tree; // for friend class

template<class T> 
class node {
private:
node<T>* l_node;
node<T>* r_node;

public:

node() :  l_node(NULL), r_node(NULL), isleaf(true), level(0), id_node(0), size(index.size()), index_col(0), impurity(std::numeric_limits<T>::max()), threshold(0), leaf_value(0) {}

node(std::vector<int> index) :  l_node(NULL), r_node(NULL), isleaf(true), level(0), id_node(0), size(index.size()), index_col(0), impurity(std::numeric_limits<T>::max()), threshold(0), leaf_value(0), index(index) {}

node(int level, int id_node, double impurity, std::vector<int> index) : l_node(NULL), r_node(NULL), isleaf(true), level(level), id_node(id_node), size(index.size()), index_col(0), impurity(impurity), threshold(0), leaf_value(0), index(index) {}
friend class tree<T>;   

bool isleaf;
int level, id_node, size, index_col;
double impurity, threshold;
T  leaf_value;
std::vector<int> index;

void set_children(node<T>* left, node<T>* right);
node& get_l_children() const;
node& get_r_children() const;
void print();
void write() ;

std::unordered_map<int, double> probabilities_of_each_class; // unused if T = int, to be rework.
void set_leaf_value(const std::vector<T>& Y);
};
