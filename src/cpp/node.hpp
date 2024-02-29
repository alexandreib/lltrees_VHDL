#ifndef __NODE_H_INCLUDED__ 
#define __NODE_H_INCLUDED__

// // #include <iostream>
// #include <vector>
// #include <numeric>
// #include <algorithm>
// #include <limits>

class node{
private:
    node* l_node;
    node* r_node;

public:
    // node() : isleaf(true), level(0), id_node(0), l_size(0), r_size(0),  size(0), index_col (0), 
    // loss(std::numeric_limits<int>::max()), threshold(std::numeric_limits<int>::max()), leaf_value(std::numeric_limits<int>::max()) {}
    node(int size) : isleaf(true), level(0), id_node(0), l_size(0), r_size(0),  size(size), index_col (0), 
    loss(std::numeric_limits<int>::max()), threshold(std::numeric_limits<int>::max()), leaf_value(std::numeric_limits<double>::quiet_NaN()) {}
    node(int level, int id_node, int size, double loss) : 
    isleaf(true), level(level), id_node(id_node), l_size(0), r_size(0), size(size), index_col(0), loss(loss), threshold(0) {}
    
    bool isleaf;
    int level, id_node, l_size, r_size, size, index_col;
    double loss, threshold ,leaf_value ;

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

#endif // __NODE_H_INCLUDED__