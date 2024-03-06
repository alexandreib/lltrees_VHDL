#include "tree.hpp"

tree_configuration conf_trees;

///////////////////////////////////////// Destructor
template<class T> void tree<T>::deleteTree(node<T>* node){ 
    if (node == NULL) return; 
    this->deleteTree(node->l_node); 
    this->deleteTree(node->r_node); 
    delete node;
} 

///////////////////////////////////////// Fit Area
template<class T> void tree<T>::fit(const data& tr, const std::vector<double>& Y) {
    this->node_0 = new node<T>(0, this->id_node, tr.number_of_rows, std::numeric_limits<int>::max());
    this->_grow(*this->node_0, tr, Y, tr.index);
}

template<class T> void tree<T>::_grow(node<T>& pnode, const data& tr, const std::vector<double>& Y, std::vector<int> index) {
    
    const data_type<T>& trs = static_cast <const data_type<T>&> (tr);
    
    if (conf_trees.max_depth > pnode.level) {
        for (int index_col = 0; index_col < tr.number_of_cols; index_col++) {
            std::vector<double> column = tr.get_column(index_col, index);
            std::vector<double> unique_sorted = column;
            std::sort(unique_sorted.begin(), unique_sorted.end());
            unique_sorted.erase(std::unique(unique_sorted.begin(), unique_sorted.end()), unique_sorted.end());
            
            for(long unsigned int idx = 0; idx < unique_sorted.size() - 1; idx++) {
                double threshold = (unique_sorted[idx] + unique_sorted[idx + 1])/2;
                std::vector<T> l_Y, r_Y;
                for(auto &index_row : index) {
                    if (column[index_row] < threshold) { l_Y.push_back(trs.y[index_row]); }
                    else { r_Y.push_back(trs.y[index_row]); }
                }
                double loss = (l_Y.size()/(double) index.size())*crit->get(l_Y) + (r_Y.size()/(double) index.size())*crit->get(r_Y);
                if (loss < pnode.loss) 
                {
                    pnode.loss = loss;
                    pnode.index_col = index_col;
                    pnode.threshold = threshold;
                    pnode.isleaf = false;  
                    pnode.l_size = l_Y.size();  
                    pnode.r_size = r_Y.size();
                }
            }  
        }
    }
    if (!pnode.isleaf) {
        std::vector<int> l_index, r_index;
        for(auto & index_row : index) {
            if (tr.x[index_row * tr.number_of_cols +  pnode.index_col] < pnode.threshold) {  
                l_index.push_back(index_row);
            }
            else {        
                r_index.push_back(index_row);
            }
        }            
        node<T>* l_node = new node<T>(pnode.level+1, ++this->id_node, l_index.size(), pnode.loss);
        node<T>* r_node = new node<T>(pnode.level+1, ++this->id_node, r_index.size(), pnode.loss);
        
        pnode.set_children(l_node, r_node);
        this->_grow(*l_node, tr, Y, l_index);
        this->_grow(*r_node, tr, Y, r_index);           
    } else {
        this->get_leaf_value(pnode, Y, index);
     
    }
}

template<> void tree<double>::get_leaf_value(node<double>& pnode, const std::vector<double>& Y, std::vector<int> index) {
    double average = 0;
    for(const int& index_row : index) {
        average = average + Y[index_row];
    }                
    pnode.leaf_value = average / index.size(); 
    pnode.isleaf = true ;  
    pnode.size = index.size(); 
}

template<> void tree<int>::get_leaf_value(node<int>& pnode, const std::vector<double>& Y, std::vector<int> index) {
    std::unordered_map<int, int> freqMap; 
    for (long unsigned int i = 0; i < index.size(); i++) { freqMap[Y[i]]++;}  
    auto maxElement = max_element(freqMap.begin(), freqMap.end(), 
                    [](const auto& a, const auto& b) { 
                      return a.second < b.second; 
                  }); 
    pnode.leaf_value = maxElement->first; 
    pnode.isleaf = true ;  
    pnode.size = index.size(); 
}

///////////////////////////////////////// Predict Area
template<class T> double tree<T>::predict_row(const double* row) {   
    return this->_traverse(*this->node_0, row);
}

template<class T> double tree<T>::_traverse(node<T>& pnode, const double * row) {
    if (pnode.isleaf){ return (double) pnode.leaf_value; }
    if (row[pnode.index_col] < pnode.threshold) {
        return this->_traverse(pnode.get_l_children(),row);
    } else {
        return this->_traverse(pnode.get_r_children(),row);
    }
}

// std::vector<double> predict_vector(const std::vector<double>& X,const double lr = 1.0) {
//     std::vector<double> preds(X.size() / this->number_of_cols);
//     for (long unsigned int index_row = 0; index_row < X.size() / this->number_of_cols; index_row ++){
//         preds[index_row] = lr *  this->predict_row(&X[index_row * this->number_of_cols]);
//     }
//     return preds;
// }
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
