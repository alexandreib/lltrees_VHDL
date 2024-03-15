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
void tree<T>::fit(const data& tr, const std::vector<T>& target) {
    this->node_0 = new node<T>(tr.number_of_rows);
    std::vector<int> index(tr.number_of_rows);
    std::iota(index.begin(), index.end(), 0);
    numbers_col = tr.number_of_cols / conf_gbt.number_of_threads;
    this->_grow(*this->node_0, tr, target, index);
}

std::mutex mutex_impurity;

template<class T> 
void tree<T>::_calculate_impurity(node<T>& pnode, 
                                const data& tr, 
                                const std::vector<T>& Y, 
                                const std::vector<int>& index,
                                const int i) {
    int start_col  = numbers_col * i;
    int end_col = numbers_col * (i+1);
    if (i == conf_gbt.number_of_threads -1) {
        if (tr.number_of_cols % conf_gbt.number_of_threads != 0 ) {
            ++end_col;
        }
    }
    for (int index_col = start_col; index_col < end_col; index_col++) {
        // std::cout << "start_col "<< start_col << "end_col " << end_col << "index_col " << index_col <<std::endl;
        std::vector<double> unique_sorted;
        for(auto const &index_row : index) {
            unique_sorted.push_back(tr.x[index_row * tr.number_of_cols + index_col]); 
        }
        std::sort(unique_sorted.begin(), unique_sorted.end());
        unique_sorted.erase(std::unique(unique_sorted.begin(), unique_sorted.end()), unique_sorted.end());
        
        if (unique_sorted.size() !=1 ) {
            for(long unsigned int idx = 0; idx < unique_sorted.size() -1 ; idx++) {
                double threshold = (unique_sorted[idx] + unique_sorted[idx+1])/2;
                std::vector<T> l_Y, r_Y;
                for(auto const &index_row : index) {
                    if (tr.x[index_row * tr.number_of_cols + index_col] <= threshold) {
                        l_Y.push_back( Y[index_row]); 
                    }
                    else { 
                        r_Y.push_back( Y[index_row]); 
                    }
                }
                
                double impurity = (l_Y.size()/(double) index.size())* criterion_tree->get(l_Y) + 
                (r_Y.size()/(double) index.size())*criterion_tree->get(r_Y);
                std::lock_guard<std::mutex> lock(mutex_impurity);
                if (!isnan(impurity) && impurity < pnode.impurity && r_Y.size() > conf_trees.min_leaf_size &&
                    l_Y.size() > conf_trees.min_leaf_size  ) {
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
void tree<T>::_grow(node<T>& pnode, const data& tr, const std::vector<T>& Y, const std::vector<int>& index) {
    if (pnode.level < conf_trees.max_depth) {  
        ThreadPool pool(conf_gbt.number_of_threads);
        // std::vector<std::thread> workers;
        // workers.reserve(conf_gbt.number_of_threads);

        for(int i = 0; i < conf_gbt.number_of_threads; i++) {
             // this->_calculate_impurity(pnode, tr, Y, index, i);
             // workers.emplace_back(std::thread(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), std::ref(index), i));
            std::function<void()> bound_calculate_impurity = std::bind(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), std::ref(index), i); 
            pool.enqueue(bound_calculate_impurity);
        }
        // for (auto &thread: workers) { thread.join(); }
        pool.wait();
    }
    if (pnode.isleaf == false) {
        std::vector<int> l_index, r_index;
        for(auto const &index_row : index) {
            if (tr.x[index_row * tr.number_of_cols +  pnode.index_col] <= pnode.threshold) {  
                l_index.push_back(index_row);
            }
            else {        
                r_index.push_back(index_row);
            }
        }         
        
        node<T>* l_node = new node<T>(pnode.level+1, ++this->id_node, l_index.size(), pnode.impurity);
        node<T>* r_node = new node<T>(pnode.level+1, ++this->id_node, r_index.size(), pnode.impurity);
        pnode.l_size = l_index.size();  
        pnode.r_size = r_index.size();
        pnode.set_children(l_node, r_node);
        this->_grow(*l_node, tr, Y, l_index);
        this->_grow(*r_node, tr, Y, r_index);    
    } 
    pnode.leaf_value = this->get_leaf_value(Y, index);
}

template<> 
double tree<double>::get_leaf_value(const std::vector<double>& Y, const std::vector<int>& index) {
    double average = 0;
    for(auto const &index_row : index) {
        average = average + Y[index_row];
    }                
     return average / index.size(); 
}

template<> 
int tree<int>::get_leaf_value(const std::vector<int>& Y, const std::vector<int>& index) {
    std::unordered_map<int, int> freqMap; 
    for (long unsigned int i = 0; i < index.size(); i++) { freqMap[Y[i]]++;}  
    auto maxElement = max_element(freqMap.begin(), freqMap.end(), 
                    [](const auto& a, const auto& b) { 
                      return a.second < b.second; 
                  }); 
    return maxElement->first; 
}

///////////////////////////////////////// Predict Area
template<class T> 
std::vector<T> tree<T>::predict(const data &d) {  
    std::vector<T> pred;
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++){
        pred.push_back(this->predict_row(d.x + index_row * d.number_of_cols));
    }
    return pred;
}

template<class T> 
inline T tree<T>::predict_row(const double* row) const {  
    return this->_traverse(*this->node_0, row);
}

template<class T> 
T tree<T>::_traverse(const node<T>& pnode, const double * row) const {
    if (pnode.isleaf){ 
        return pnode.leaf_value; 
    }
    if (*(row + pnode.index_col) <= pnode.threshold) {
        return this->_traverse(pnode.get_l_children(), row);
    } else {
        return this->_traverse(pnode.get_r_children(), row);
    }
}

///////////////////////////////////////// Print Area
template<class T> 
void tree<T>::print() {
    this->print(*this->node_0);
}

template<class T> 
void tree<T>::print(node<T>& node) {
    if (!node.isleaf) {
        node.print();
        this->print(node.get_l_children());
        this->print(node.get_r_children());
    }
    else {
        node.print();
    }
}

template<class T> 
void tree<T>::print_node_0() {
    this->node_0->print();
}

template<class T> 
void tree<T>::printBT(const std::string& prefix, const node<T>* pnode, bool isLeft) {
    if( pnode != nullptr ){
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──" );
        
        if (pnode->isleaf) {
            std::cout << pnode->id_node << ":" << pnode->leaf_value << std::endl;
        } else {
            std::cout << pnode->id_node << std::endl;
        }
        printBT( prefix + (isLeft ? "│   " : "    "), pnode->l_node, true);
        printBT( prefix + (isLeft ? "│   " : "    "), pnode->r_node, false);
    }
}

template<class T> 
void tree<T>::printBT() {
    printBT("", this->node_0, false);    
}

///////////////////////////////////////// Save/Load Area
template<class T> 
void tree<T>::load(node<T>*& pnode, std::string& line) {
    std::cout << "line 0 :" << line <<std::endl;
    
    std::string delimiter = ",";
    std::string token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    if (token == "#") {
        return;
    }    
    pnode = new node<T>(0, std::stoi(token), 0, 0);
    std::cout << "line 1 :" <<token << " : " << line <<std::endl;
    this->load(pnode->l_node, line );
    this->load(pnode->r_node, line );
}

template<class T> 
void tree<T>::load(std::string line) {
    std::cout << "tree load " <<std::endl;
    this->load(this->node_0, line );
    this->printBT();
}

template<class T> 
void tree<T>::save(std::ofstream& file) {
    std::cout << "tree save " <<std::endl;
    save(this->node_0, file);  
}

template<class T> 
void tree<T>::save(const node<T>* pnode, std::ofstream& file) {
    if (pnode == NULL) { file << "#,";  return ;}    
    {
        file << pnode->id_node << "#" << pnode->id_node << ",";
    }
    this->save(pnode->l_node, file);
    this->save(pnode->r_node, file);
}

template class tree<int>;  // Explicit instantiation
template class tree<double>;  // Explicit instantiation
