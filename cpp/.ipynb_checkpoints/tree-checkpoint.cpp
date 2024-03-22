#include <chrono>
#include <mutex>
#include <thread> 
#include <iostream>
#include <fstream>
#include "tree.hpp"
#include "conf.hpp"
#include "threadpool.hpp"

///////////////////////////////////////// Constructor / Destructor / set / get
template<class T>
tree<T>::~tree()
{ 
    deleteTree(this->node_0);
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

///////////////////////////////////////// Fit Area
template<class T>
void tree<T>::fit(const XY& tr, const std::vector<T>& Y) 
{
    this->numbers_col = tr.number_of_cols / conf_gbt.number_of_threads;
    
    double impurity = criterion_tree->get(Y);
    this->node_0 = new node<T>(tr.number_of_rows, impurity);
    
    std::vector<int> index(tr.number_of_rows);
    std::iota(index.begin(), index.end(), 0);
    
    this->fit(*this->node_0, tr, Y, index);
}

std::mutex mutex_impurity;

template<class T> 
void tree<T>::_calculate_impurity(node<T>& pnode, 
                                const XY& tr, 
                                const std::vector<T>& Y, 
                                const std::vector<int>& index,
                                double &current_impurity,
                                const int thread_n) 
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
                std::vector<T> l_Y, r_Y;
                for(auto const &index_row : index) 
                {
                    if (tr.x[index_row * tr.number_of_cols + index_col] <= threshold) 
                    {
                        l_Y.push_back( Y[index_row] ); 
                    }
                    else 
                    { 
                        r_Y.push_back( Y[index_row] ); 
                    }
                }
                
                double l_impurity = criterion_tree->get(l_Y);
                double r_impurity = criterion_tree->get(r_Y);
                double new_impurity = (l_Y.size() / (double) pnode.size) * l_impurity + (r_Y.size() / (double) pnode.size) * r_impurity;
                
                std::lock_guard lock(mutex_impurity);
                if (new_impurity < current_impurity && !isnan(new_impurity) && r_Y.size() > conf_trees.min_leaf_size && l_Y.size() > conf_trees.min_leaf_size)
                {
                    // std::cout << "pnode.id_node :" << pnode.id_node << " new_impurity :" << new_impurity << " l_Y.size():" << l_Y.size() << " r_Y.size():" << r_Y.size()<<std::endl;        
                    current_impurity = new_impurity;
                    pnode.threshold = threshold;
                    pnode.index_col = index_col; 
                    pnode.isleaf = false;  
                    pnode.l_impurity = l_impurity;
                    pnode.r_impurity = r_impurity;
                }
            }  
        }
    }
}

template<class T> 
void tree<T>::fit(node<T> & pnode, const XY & tr, const std::vector<T> & Y, const std::vector<int> & index) 
{      
    if (pnode.level < conf_trees.max_depth) 
    {
        ThreadPool pool(conf_gbt.number_of_threads);
        // std::vector<std::thread> workers;
        // workers.reserve(conf_gbt.number_of_threads);
        double current_impurity = pnode.impurity;
        for(int thread_n = 0; thread_n < conf_gbt.number_of_threads; thread_n++) 
        {
            // this->_calculate_impurity(pnode, tr, Y, index, thread_n);
            // workers.emplace_back(std::thread(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), std::ref(index), i));
            std::function<void()> bound_calculate_impurity = std::bind(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), std::ref(index), std::ref(current_impurity), thread_n); 
            pool.enqueue(bound_calculate_impurity);
        }
        // for (auto &thread: workers) { thread.join(); }
        pool.wait();
    }
    // pnode.print();  
    if (pnode.isleaf == false) 
    {
        std::vector<int> l_index, r_index;
        for(auto const &index_row : index) 
        {
            if (tr.x[index_row * tr.number_of_cols +  pnode.index_col] <= pnode.threshold) 
            {
                l_index.push_back(index_row);
            }
            else 
            {
                r_index.push_back(index_row);
            }
        }
        node<T>* l_node = new node<T>(pnode.level+1, ++this->id_node, l_index.size(), pnode.l_impurity);
        node<T>* r_node = new node<T>(pnode.level+1, ++this->id_node, r_index.size(), pnode.r_impurity);
        pnode.set_children(l_node, r_node);
        this->fit(*l_node, tr, Y, l_index);
        this->fit(*r_node, tr, Y, r_index);    
    }
    else
    {
        pnode.set_leaf_value(Y, index);
    }
}

///////////////////////////////////////// Predict Area
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

template<class T> 
void tree<T>::pred_and_add(const XY & d, std::vector<double> & pred) {
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++){
        pred[index_row] += conf_gbt.learning_rate * this->predict_row<double>(d.x + index_row * d.number_of_cols);
    }
}
///////////////////////////////////////// Print Area
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
            std::cout << pnode->id_node << ", impurity: " << pnode->impurity << ", leaf_value: " << pnode->leaf_value << ", size: " << pnode->size << std::endl;
        } 
        else 
        {
            std::cout << pnode->id_node << ", impurity: " << pnode->impurity << ", size: " << pnode->size << std::endl;
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
