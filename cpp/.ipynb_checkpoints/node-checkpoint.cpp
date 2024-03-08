#include "node.hpp"

template <class T> void node<T>::set_children(node<T>* left, node<T>* right) {
        this->l_node = left;
        this->r_node = right;
    }

template <class T> node<T>& node<T>::get_l_children() const {
        return *l_node;
    }

template <class T> node<T>& node<T>::get_r_children() const {
        return *r_node;
    }

template <class T> void node<T>::print() {
    std::cout << "**********" << std::endl;
    std::cout << "Node id : " << this->id_node << std::endl;
    std::cout << "Node level : " << level << std::endl;
    std::cout << "Node impurity : " << impurity << std::endl;
    std::cout << "Node index_col : " << index_col << std::endl;
    std::cout << "Node threshold : " << threshold << std::endl;
    std::cout << "Node isleaf : " << std::boolalpha << isleaf << std::endl;
    std::cout << "Node leaf_value : " << leaf_value << std::endl;
    std::cout << "Node l_size : " << l_size << std::endl;
    std::cout << "Node r_size : " << r_size << std::endl;
    std::cout << "Node size : " << size << std::endl;
    std::cout << "**********" << std::endl;
}

template <class T> void node<T>::write() {
    std::cout << "**********" << std::endl;
}

template class node<int>;  // Explicit instantiation
template class node<double>;  // Explicit instantiation