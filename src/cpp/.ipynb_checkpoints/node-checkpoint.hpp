#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

template <typename T> class tree;

template<class T> class node{
private:
    node<T>* l_node;
    node<T>* r_node;

public:
    node(int level, int id_node, int size, double loss) :  l_node(NULL), r_node(NULL), isleaf(true), level(level), id_node(id_node), l_size(0), r_size(0), size(size), index_col(0), loss(loss), threshold(std::numeric_limits<T>::quiet_NaN()), leaf_value(std::numeric_limits<T>::quiet_NaN()) {}
    friend class tree<T>;   

    bool isleaf;
    int level, id_node, l_size, r_size, size, index_col;
    double loss, threshold;
    T  leaf_value;

    void set_children(node<T>* left, node<T>* right) {
         this->l_node = left;
         this->r_node = right;
     }

    node& get_l_children() {
         return *l_node;
     }
    
    node& get_r_children() {
         return *r_node;
     }

    void print() {
        std::cout << "**********" << std::endl;
        std::cout << "Node id : " << this->id_node << std::endl;
        std::cout << "Node level : " << level << std::endl;
        std::cout << "Node loss : " << loss << std::endl;
        std::cout << "Node index_col : " << index_col << std::endl;
        std::cout << "Node threshold : " << threshold << std::endl;
        std::cout << "Node isleaf : " << std::boolalpha << isleaf << std::endl;
        std::cout << "Node leaf_value : " << leaf_value << std::endl;
        std::cout << "Node l_size : " << l_size << std::endl;
        std::cout << "Node r_size : " << r_size << std::endl;
        std::cout << "Node size : " << size << std::endl;
        std::cout << "**********" << std::endl;
    }

    void write() {
        std::cout << "**********" << std::endl;
    }
};

template class node<int>;  // Explicit instantiation
template class node<double>;  // Explicit instantiation