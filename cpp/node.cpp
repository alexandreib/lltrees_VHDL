#include "node.hpp"

template <class T> void node<T>::set_children(node<T>* left, node<T>* right) 
{
    this->l_node = left;
    this->r_node = right;
}

template <class T> node<T>& node<T>::get_l_children() const 
{
    return *l_node;
}

template <class T> node<T>& node<T>::get_r_children() const 
{
    return *r_node;
}

template <class T> void node<T>::print() 
{
    std::cout << "**********" << std::endl;
    std::cout << "Node id : " << this->id_node << std::endl;
    std::cout << "Node level : " << level << std::endl;
    std::cout << "Node impurity : " << impurity << std::endl;
    std::cout << "Node index_col : " << index_col << std::endl;
    std::cout << "Node threshold : " << threshold << std::endl;
    std::cout << "Node isleaf : " << std::boolalpha << isleaf << std::endl;
    std::cout << "Node leaf_value : " << leaf_value << std::endl;
    std::cout << "Node size : " << size << std::endl;  
    for (const auto& pair : this->probas) 
    { 
        std::cout<< pair.first <<" : " << pair.second << std::endl;  
    }  
    std::cout << "**********" << std::endl;
}
template<class T> 
T node<T>::get_leaf_value() 
{
    return this->leaf_value;
}

template<> 
void node<double>::set_leaf_value(const std::vector<double>& Y, const std::vector<int>& index) 
{
    double average = 0;
    for(auto const &index_row : index)
    {
        average = average + Y[index_row];
    }                
     this->leaf_value = average / index.size(); 
}

template<> 
void node<int>::set_leaf_value(const std::vector<int>& Y, const std::vector<int>& index) 
{
    // std::cout << "set_leaf_value" <<std::endl;
    for (long unsigned int idx : index) 
    { 
        if (this->probas.find(Y[idx]) == this->probas.end())
        {
            this->probas[Y[idx]] = 1 ;
        }
        else
        {
            this->probas[Y[idx]] += 1 ;
        }
    }
    for (auto const &prob : this->probas) 
    {
        this->probas[prob.first] = (double) prob.second /  (double) index.size();
        // std::cout << this->id_node << " " << prob.first <<" "<< prob.second << " " <<  this->size << std::endl;
        // std::cout << "prob: " << this->probas[prob.first] <<std::endl;
    }
    double max_proba = 0;
    for (const auto& pair : this->probas) 
    { 
        if (pair.second > max_proba) 
        { 
            max_proba = pair.second;
            this->leaf_value = pair.first; 
        } 
    }  
    // std::cout << "this->leaf_value " << this->leaf_value <<std::endl;
}

template class node<int>;  // Explicit instantiation
template class node<double>;  // Explicit instantiation