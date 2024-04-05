#include <chrono>
#include <mutex>
#include <thread> 
#include <iostream>
#include <fstream>
#include "tree.hpp"
#include "conf.hpp"

// Constructor / Destructor
template<class T>
tree<T>::tree()
{ 
} 

template<class T>
tree<T>::~tree()
{ 
    this->deleteTree(this->node_0);
    this->node_0 = NULL;
} 

template<class T> 
void tree<T>::deleteTree(node<T>* node)
{ 
    if (node == NULL) return; 
    this->deleteTree(node->l_node); 
    this->deleteTree(node->r_node); 
    delete node;
} 

// Fit Area
template<class T>
void tree<T>::fit(const XY & tr, 
                const std::vector<T> & Y,
                const std::vector<double> & W) 
{   
    std::vector<int> index(tr.number_of_rows);
    std::iota(index.begin(), index.end(), 0);
    
    double impurity = criterion_tree->get(Y, index, W);
    this->node_0 = new node<T>(tr.number_of_rows, impurity);
    
    this->fit(*this->node_0, tr, Y, index, W);    
}

template<class T> 
void tree<T>::fit(node<T> & pnode,
                const XY & tr, 
                const std::vector<T> & Y, 
                const std::vector<int> & index, 
                const std::vector<double> & W) 
{      
    std::vector<int> l_index, r_index;
    double l_impurity, r_impurity;
    if (pnode.level < conf::tree::max_depth) 
    {
        double current_impurity = pnode.impurity;
        for (int index_col = 0; index_col < tr.number_of_cols; index_col++) 
        {
            std::function<void()> bound_split = std::bind(&tree::split, this, 
                                                        std::ref(pnode), 
                                                        std::ref(tr), 
                                                        std::ref(Y), 
                                                        std::ref(index), 
                                                        std::ref(W), 
                                                        index_col,  
                                                        std::ref(current_impurity),
                                                        std::ref(l_index), 
                                                        std::ref(r_index), 
                                                        std::ref(l_impurity), 
                                                        std::ref(r_impurity)); 
            this->pool->enqueue(bound_split);
        }
        this->pool->wait_finish();
    }

    if (pnode.isleaf == false) 
    {
        node<T>* l_node = new node<T>(pnode.level + 1, pnode.id_node * 2 , l_index.size(), l_impurity);
        node<T>* r_node = new node<T>(pnode.level + 1, pnode.id_node * 2 + 1, r_index.size(), r_impurity);
        pnode.set_children(l_node, r_node);
        this->fit(*l_node, tr, Y, l_index, W);
        this->fit(*r_node, tr, Y, r_index, W);    
    }
    else
    {
        pnode.set_leaf_value(Y, index);
    }
}

std::mutex mutex_impurity;

template<class T> 
void tree<T>::split(node<T> & pnode, 
                const XY & tr, 
                const std::vector<T> & Y, 
                const std::vector<int> & index,
                const std::vector<double> & W,
                const int index_col,
                double & current_impurity,
                std::vector<int> & l_index, 
                std::vector<int> & r_index,
                double & l_impurity, 
                double & r_impurity) 
{
    std::vector<double> unique_sorted;
    for(auto const &index_row : index) 
    {
        unique_sorted.push_back(tr.x[index_row * tr.number_of_cols + index_col]); 
    }
    std::sort(unique_sorted.begin(), unique_sorted.end());
    unique_sorted.erase(std::unique(unique_sorted.begin(), unique_sorted.end()), unique_sorted.end());
    
    if (unique_sorted.size() !=1 ) 
    {
        for(long unsigned int idx = 0; idx < unique_sorted.size() -1 ; idx++) 
        {
            double threshold = (unique_sorted[idx] + unique_sorted[idx+1])/2;
            std::vector<int> local_l_index, local_r_index;
            for(auto const &index_row : index) 
            {
                if (tr.x[index_row * tr.number_of_cols + index_col] <= threshold) 
                {
                    local_l_index.push_back( index_row ); 
                }
                else 
                { 
                    local_r_index.push_back( index_row ); 
                }
            }
            double local_l_impurity = criterion_tree->get(Y, local_l_index, W);
            double local_r_impurity = criterion_tree->get(Y, local_r_index, W);
            double new_impurity = (local_l_index.size() / (double) pnode.size) * local_l_impurity + (local_r_index.size() / (double) pnode.size) * local_r_impurity;
            
            const std::lock_guard<std::mutex> lock(mutex_impurity);
            if (new_impurity < current_impurity && !isnan(new_impurity) && local_r_index.size() > conf::tree::min_leaf_size && local_l_index.size() > conf::tree::min_leaf_size)
            {     
                current_impurity = new_impurity;
                l_impurity = local_l_impurity;
                r_impurity = local_r_impurity;
                l_index = local_l_index;
                r_index = local_r_index;
                pnode.threshold = threshold;
                pnode.index_col = index_col; 
                pnode.isleaf = false;  
            }
        }  
    }
}

// Predict Area
template<class T> 
template<class U> 
std::vector<U> tree<T>::predict(const XY & d) const
{  
    std::vector<U> pred;
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++)
    {
        pred.push_back(this->predict_row<U>(d.x + index_row * d.number_of_cols));
    }
    return pred;
}

template<class T> 
template<class U> 
U tree<T>::predict_row(const double * row) const 
{  
    return this->predict_row<U>(*this->node_0, row);
}

template<class T> 
template<class U> 
U tree<T>::predict_row(const node<T> & pnode, const double * row) const 
{
    if (pnode.isleaf)
    { 
        return pnode.template get_leaf_value<U>();
    }
    if (*(row + pnode.index_col) <= pnode.threshold)
    {
        return this->predict_row<U>(pnode.get_l_children(), row);
    } 
    else 
    {
        return this->predict_row<U>(pnode.get_r_children(), row);
    }
}
template std::vector<int> tree<int>::predict(const XY & d) const; 
template std::vector<std::unordered_map<int, double>> tree<int>::predict(const XY & d) const; 
template std::vector<double> tree<double>::predict(const XY & d) const; 

template int tree<int>::predict_row(const double * row) const;
template std::unordered_map<int, double> tree<int>::predict_row(const double * row) const;
template double tree<double>::predict_row(const double * row) const;

template int tree<int>::predict_row(const node<int> & pnode, const double * row) const;
template std::unordered_map<int, double> tree<int>::predict_row(const node<int> & pnode, const double * row) const;
template double tree<double>::predict_row(const node<double> & pnode, const double * row) const; 

template<> 
void tree<double>::pred_and_add(const XY & d, std::vector<double> & pred) 
{
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++)
    {
        pred[index_row] += conf::gbt::learning_rate * this->predict_row<double>(d.x + index_row * d.number_of_cols);
    }
}
template<> 
void tree<int>::pred_and_add(const XY & d, std::vector<double> & pred) 
{
    __builtin_unreachable();
}

template<> 
void tree<int>::pred_and_add(const XY & d, std::vector<std::unordered_map<int, double>> & pred, const double & model_weight) 
{
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++)
    {
        std::unordered_map<int, double> tree_pred = this->predict_row<std::unordered_map<int, double>>(d.x + index_row * d.number_of_cols);
        for (auto & map_row_pred : pred[index_row]) 
        {
            pred[index_row][map_row_pred.first] += model_weight * tree_pred[map_row_pred.first];
        }
    }
    
}

template<> 
void tree<double>::pred_and_add(const XY & d, std::vector<std::unordered_map<int, double>> & pred, const double & model_weight)  
{
    __builtin_unreachable();
}

// Print Area
template<class T> 
void tree<T>::print_node_0() 
{
    this->node_0->print();
}

template<class T> 
void tree<T>::printBT(const std::string & prefix, const node<T> * pnode, bool isLeft)
{
    if( pnode != nullptr )
    {
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──" );
        if (pnode->isleaf) 
        {
            std::cout << pnode->id_node << ", criterion: " << pnode->impurity << ", leaf_value: " << pnode->leaf_value << ", size: " << pnode->size << std::endl;
        } 
        else 
        {
            std::cout << pnode->id_node << ", criterion: " << pnode->impurity << ", size: " << pnode->size << std::endl;
        }
        printBT( prefix + (isLeft ? "│   " : "    "), pnode->l_node, true);
        printBT( prefix + (isLeft ? "│   " : "    "), pnode->r_node, false);
    }
}

template<class T> 
void tree<T>::printBT() 
{
    printBT("", this->node_0, false);    
}

// Save/Load Area
template<class T> 
void tree<T>::load(node<T> *& pnode, std::string & line) 
{
    std::string delimiter = ":";
    std::string token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + 1);
    if (token == "#") 
    {
        return;
    }    
    int id_node = std::stoi(token);
    
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    bool isleaf = (bool)std::stoi(token);

    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    int level = std::stoi(token);
       
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    int size = std::stoi(token);
    
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    int index_col = std::stoi(token);

    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    double impurity = std::stod(token);

    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    double threshold = std::stod(token);
    
    token = line.substr(0, line.find(","));
    line.erase(0, token.size() + 1);
    double leaf_value = std::stod(token);

    pnode = new node<T>(size, impurity);
    pnode->id_node = id_node;
    pnode->isleaf = isleaf;
    pnode->level = level;
    pnode->index_col = index_col;
    pnode->threshold = threshold;
    pnode->leaf_value = leaf_value;
    
    this->load(pnode->l_node, line );
    this->load(pnode->r_node, line );
}

template<class T> 
void tree<T>::load(std::string line)
{
    this->load(this->node_0, line );
}

template<class T> 
void tree<T>::save(std::ofstream & file) 
{
    save(this->node_0, file);  
}

template<class T> 
void tree<T>::save(const node<T> * pnode, std::ofstream & file) 
{
    if (pnode == NULL) { file << "#:";  return ;}    
    {
        file << pnode->id_node << ":" << pnode->isleaf << ":" << pnode->level << ":" << pnode->size << ":" 
            << pnode->index_col << ":" << pnode->impurity << ":" << pnode->threshold << ":" << pnode->leaf_value
            << ",";
    }
    this->save(pnode->l_node, file);
    this->save(pnode->r_node, file);
}

template class tree<int>;  // Explicit instantiation
template class tree<double>;  // Explicit instantiation
