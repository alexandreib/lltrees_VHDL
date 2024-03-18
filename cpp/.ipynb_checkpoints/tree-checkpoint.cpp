#include <chrono>
#include <mutex>
#include <thread> 
#include <iostream>
#include <fstream>
#include "tree.hpp"
#include "conf.hpp"
#include "factories.hpp"
#include "threadpool.hpp"


///////////////////////////////////////// Constructor / Destructor / set / get
template<class T>
tree<T>::~tree(){ 
    deleteTree(this->node_0);
    this->node_0 = NULL;
} 

template<class T> 
void tree<T>::deleteTree(node<T>* node){ 
    if (node == NULL) return; 
    this->deleteTree(node->l_node); 
    this->deleteTree(node->r_node); 
    delete node;
} 

///////////////////////////////////////// Fit Area
template<class T>
void tree<T>::fit(const data& tr, 
                    const std::vector<T>& target, 
                    const std::vector<double>& weights) 
{
    this->numbers_col = tr.number_of_cols / conf_gbt.number_of_threads;
    std::vector<int> index(tr.number_of_rows);
    std::iota(index.begin(), index.end(), 0);
    
    this->node_0 = new node<T>(index);
    this->_grow(*this->node_0, tr, target, weights);
}

std::mutex mutex_impurity;

template<class T> 
void tree<T>::_calculate_impurity(node<T>& pnode, 
                                const data& tr, 
                                const std::vector<T>& Y, 
                                const int thread_n, 
                                const std::vector<double>& weights) 
{
    int start_col  = numbers_col * thread_n;
    int end_col = numbers_col * (thread_n+1);
    if (thread_n == conf_gbt.number_of_threads -1)
    {
        if (tr.number_of_cols % conf_gbt.number_of_threads != 0 ) 
        {
            ++end_col;
        }
    }
    for (int index_col = start_col; index_col < end_col; index_col++) 
    {
        // std::cout << "start_col "<< start_col << "end_col " << end_col << "index_col " << index_col <<std::endl;
        std::vector<double> unique_sorted;
        for(auto const &index_row : pnode.index) 
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
                std::vector<int> l_index, r_index;
                for(auto const &index_row : pnode.index) 
                {
                    if (tr.x[index_row * tr.number_of_cols + index_col] <= threshold) 
                    {
                        l_index.push_back( index_row); 
                    }
                    else 
                    { 
                        r_index.push_back( index_row); 
                    }
                }
                
                double impurity = ((double) l_index.size()/(double) pnode.size)*criterion_tree->get(Y, l_index, weights) + 
                                  ((double) r_index.size()/(double) pnode.size)*criterion_tree->get(Y, r_index, weights);
                
                std::lock_guard<std::mutex> lock(mutex_impurity);
                if (!isnan(impurity) && impurity < pnode.impurity && r_index.size() > conf_trees.min_leaf_size &&
                    l_index.size() > conf_trees.min_leaf_size) 
                {
                    std::cout <<"impurity :"<< impurity <<std::endl;
                    pnode.impurity = impurity;
                    pnode.threshold = threshold;
                    pnode.index_col = index_col;
                    pnode.isleaf = false;
                }
            }  
        }
    }
}

template<class T> 
void tree<T>::_grow(node<T>& pnode,
                    const data& tr, 
                    const std::vector<T>& Y, 
                    const std::vector<double>& weights) 
{
    if (pnode.level < conf_trees.max_depth) {  
        ThreadPool pool(conf_gbt.number_of_threads);
        // std::vector<std::thread> workers;
        // workers.reserve(conf_gbt.number_of_threads);

        for(int thread_n = 0; thread_n < conf_gbt.number_of_threads; thread_n++) {
             // this->_calculate_impurity(pnode, tr, Y, index, i);
             // workers.emplace_back(std::thread(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), std::ref(index), i));
            std::function<void()> bound_calculate_impurity = std::bind(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), thread_n, weights); 
            pool.enqueue(bound_calculate_impurity);
        }
        // for (auto &thread: workers) { thread.join(); }
        pool.wait();
    }
    if (pnode.isleaf == false) {         
        std::vector<int> l_index, r_index;
        for(auto const &index_row : pnode.index) {
            if (tr.x[index_row * tr.number_of_cols +  pnode.index_col] <= pnode.threshold) {  
                l_index.push_back(index_row);
            }
            else {        
                r_index.push_back(index_row);
            }
        }
        node<T>* l_node = node_Factory<T>(pnode.level+1, ++this->id_node, pnode.impurity, l_index); 
        node<T>* r_node = node_Factory<T>(pnode.level+1, ++this->id_node, pnode.impurity, r_index);
        pnode.set_children(l_node, r_node);
        this->_grow(*l_node, tr, Y, weights);
        this->_grow(*r_node, tr, Y, weights);    
    } 
    if (pnode.isleaf ==true) {
        std::cout <<" Y[0] " << Y[0] <<" "<< Y[1]<<" "<< Y[2]<<" " << Y[3]<<" " <<std::endl;
        pnode.set_leaf_value(Y);
    }
}

///////////////////////////////////////// Predict Area
std::vector<std::unordered_map<int, double>> tree_classification::predict_proba(const data &d) 
{  
    std::vector<std::unordered_map<int, double>> pred;
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++)
    {
        pred.push_back(this->predict_proba_row(d.x + index_row * d.number_of_cols));
    }
    return pred;
}

inline std::unordered_map<int, double>  tree_classification::predict_proba_row(const double* row) const 
{  
    return this->_traverse_proba(*this->node_0, row);
}

std::unordered_map<int, double> tree_classification::_traverse_proba(const node<int>& pnode, const double * row) const 
{
    if (pnode.isleaf)
    { 
        return pnode.probabilities_of_each_class; 
    }
    if (*(row + pnode.index_col) <= pnode.threshold) 
    {
        return this->_traverse_proba(pnode.get_l_children(), row);
    } 
    else 
    {
        return this->_traverse_proba(pnode.get_r_children(), row);
    }
}
////////////// Classic Predict
template<class T> 
std::vector<T> tree<T>::predict(const data &d) 
{  
    std::vector<T> pred;
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++)
    {
        pred.push_back(this->predict_row(d.x + index_row * d.number_of_cols));
    }
    return pred;
}

template<class T> 
inline T tree<T>::predict_row(const double* row) const 
{  
    return this->_traverse(*this->node_0, row);
}

template<class T> 
T tree<T>::_traverse(const node<T>& pnode, const double * row) const 
{
    if (pnode.isleaf)
    { 
        return pnode.leaf_value; 
    }
    if (*(row + pnode.index_col) <= pnode.threshold)
    {
        return this->_traverse(pnode.get_l_children(), row);
    } 
    else 
    {
        return this->_traverse(pnode.get_r_children(), row);
    }
}

///////////////////////////////////////// Print Area
template<class T> 
void tree<T>::print_node_0() 
{
    this->node_0->print();
}

template<class T> 
void tree<T>::printBT(const std::string& prefix, const node<T>* pnode, bool isLeft) 
{
    if( pnode != nullptr )
    {
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──" );
        
        if (pnode->isleaf) 
        {
            std::cout << pnode->id_node << ": leaf_value: " << pnode->leaf_value << ": size: " << pnode->size << std::endl;
        } 
        else 
        {
            std::cout << pnode->id_node << std::endl;
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

///////////////////////////////////////// Save/Load Area
template<class T> 
void tree<T>::load(node<T>*& pnode, std::string& line) 
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

    pnode = new node<T>();
    pnode->id_node = id_node;
    pnode->isleaf = isleaf;
    pnode->level = level;
    pnode->size = size;
    pnode->index_col = index_col;
    pnode->impurity = impurity;
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
void tree<T>::save(std::ofstream& file) 
{
    save(this->node_0, file);  
}

template<class T> 
void tree<T>::save(const node<T>* pnode, std::ofstream& file) 
{
    if (pnode == NULL)
    { 
        file << "#:";  
        return;
    }  
    else
    {
        file << pnode->id_node << ":" << pnode->isleaf << ":" << pnode->level << ":" << pnode->size 
            << ":" << pnode->index_col << ":" << pnode->impurity << ":" << pnode->threshold << ":" << pnode->leaf_value
            << ",";
    }
    this->save(pnode->l_node, file);
    this->save(pnode->r_node, file);
}

template class tree<int>;  // Explicit instantiation
template class tree<double>;  // Explicit instantiation
