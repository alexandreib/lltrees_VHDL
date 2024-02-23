#ifndef __NODE_H_INCLUDED__ 
#define __NODE_H_INCLUDED__

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

class node{
private:
    node* l_node;
    node* r_node;

public:
    node() : isleaf(true), tree_level(0) {}//std::cout << "Creating Node 0." << std::endl; }
    node(int tree_level) : isleaf(true), tree_level(tree_level) {}
    bool isleaf;
    int tree_level;
    int l_size;
    int r_size;
    int saved_col_index = std::numeric_limits<int>::max();
    double saved_threshold = std::numeric_limits<double>::max();
    double saved_loss = std::numeric_limits<double>::max();
    double leaf_value = std::numeric_limits<double>::max();

    void set_children(node* left, node* right) {
         l_node = left;
         r_node = right;
     }

    node* get_l_children() {
         return l_node;
     }
    
    node* get_r_children() {
         return r_node;
     }

    void print() {
        std::cout << "**********" << std::endl;
        std::cout << "Node level : " << tree_level << std::endl;
        std::cout << "Node saved_loss : " << saved_loss << std::endl;
        std::cout << "Node saved_col_index : " << saved_col_index << std::endl;
        std::cout << "Node saved_threshold : " << saved_threshold << std::endl;
        std::cout << "Node isleaf : " << std::boolalpha << isleaf << std::endl;
        std::cout << "Node leaf_value : " << leaf_value << std::endl;
        std::cout << "Node l_size : " << l_size << std::endl;
        std::cout << "Node r_size : " << r_size << std::endl;
        std::cout << "**********" << std::endl;
    }
};

#endif // __NODE_H_INCLUDED__