#include <chrono>
#include <mutex>
#include <thread> 
#include "tree.hpp"
#include "conf.hpp"
// #include "threads_pool.hpp"

///////////////////////////////////////// Constructor / Destructor
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
    this->_grow(*this->node_0, tr, target, index);
}

// template<class T> 
// void tree<T>::_calculate_impurity_old(node<T>& pnode, 
//                                 const data& tr, 
//                                 const std::vector<T>& Y, 
//                                 const std::vector<int>& index, 
//                                 const int& index_col) {
    
//     std::vector<double> unique_sorted;
//     for(auto const &index_row : index) {
//         unique_sorted.push_back(tr.x[index_row * tr.number_of_cols + index_col]); 
//     }
//     std::sort(unique_sorted.begin(), unique_sorted.end());
//     unique_sorted.erase(std::unique(unique_sorted.begin(), unique_sorted.end()), unique_sorted.end());
    
//     if (unique_sorted.size() !=1 ) {
//         for(long unsigned int idx = 0; idx < unique_sorted.size() -1 ; idx++) {
//             double threshold = (unique_sorted[idx] + unique_sorted[idx+1])/2;
//             std::vector<T> l_Y, r_Y;
//             for(auto const &index_row : index) {
//                 if (tr.x[index_row * tr.number_of_cols + index_col] <= threshold) {
//                     l_Y.push_back( Y[index_row]); 
//                 }
//                 else { 
//                     r_Y.push_back( Y[index_row]); 
//                 }
//             }
            
//             if (r_Y.size() < 1) {std::cout << "r_Y.size() < 1" << std::endl; break;}
//             if (l_Y.size() < 1) {std::cout << "l_Y.size() < 1" << std::endl; break;}
//             double impurity = (l_Y.size()/(double) index.size())* criterion_tree->get(l_Y) + (r_Y.size()/(double) index.size())*criterion_tree->get(r_Y);
//             if (!isnan(impurity) && impurity < pnode.impurity && 
//                 r_Y.size() > conf_trees.min_leaf_size && l_Y.size() > conf_trees.min_leaf_size  ) {
//                 pnode.impurity = impurity;
//                 pnode.threshold = threshold;
//                 pnode.index_col = index_col;
//                 pnode.isleaf = false;
//             }      
//         }  
//     } 
// }

std::mutex mutex_impurity;

template<class T> 
void tree<T>::_calculate_impurity(node<T>& pnode, 
                                const data& tr, 
                                const std::vector<T>& Y, 
                                const std::vector<int>& index, 
                                const int start_col, 
                                const int end_col) {
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
                
                if (r_Y.size() < 1) {std::cout << "r_Y.size() < 1" << std::endl; break;}
                if (l_Y.size() < 1) {std::cout << "l_Y.size() < 1" << std::endl; break;}
                
                double impurity = (l_Y.size()/(double) index.size())* criterion_tree->get(l_Y) + (r_Y.size()/(double) index.size())*criterion_tree->get(r_Y);
                std::lock_guard<std::mutex> lock(mutex_impurity);
                if (!isnan(impurity) && impurity < pnode.impurity && 
                    r_Y.size() > conf_trees.min_leaf_size && l_Y.size() > conf_trees.min_leaf_size  ) {
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
        int number_of_threads = 8;
        int numbers_col = tr.number_of_cols / number_of_threads;
        std::vector<std::thread> workers;
        workers.reserve(number_of_threads);

        for(int i = 0; i < number_of_threads; i++) {
            int start_col  = numbers_col * i;
            int end_col = numbers_col * (i+1);
            if (i == number_of_threads -1) {
                if (tr.number_of_cols % number_of_threads != 0 ) {++end_col;}
            }
            workers.emplace_back(std::thread(&tree::_calculate_impurity, this, std::ref(pnode), std::ref(tr), std::ref(Y), std::ref(index), start_col, end_col));
        }
        for (auto &thread: workers) { thread.join(); }
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
template<class T> void tree<T>::print() {
    this->_print_tree(*this->node_0);
}

template<class T> void tree<T>::_print_tree(node<T>& node) {
    if (!node.isleaf) {
        node.print();
        this->_print_tree(node.get_l_children());
        this->_print_tree(node.get_r_children());
    }
    else {
        node.print();
    }
}

template<class T> void tree<T>::print_node_0() {
    this->node_0->print();
}

template class tree<int>;  // Explicit instantiation
template class tree<double>;  // Explicit instantiation
