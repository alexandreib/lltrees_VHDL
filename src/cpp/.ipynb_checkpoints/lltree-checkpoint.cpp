#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include "lltree.hpp"

lltree::lltree() : max_depth(5), min_size_split(2) {} //default constructor

void lltree::fit(boost::python::numpy::ndarray const & np_X, boost::python::numpy::ndarray const & np_Y) {
    double *X = reinterpret_cast<double *>(np_X.get_data());
    double *Y = reinterpret_cast<double *>(np_Y.get_data());
    this->number_of_cols = np_X.shape(1);
    std::vector<int> index(np_X.shape(0));
    int startValue = 0;
    std:: generate (index.begin(), index.end(), [&](){return startValue++;});
    this->_grow(&this->node_0, X, Y, index);
}

boost::python::numpy::ndarray lltree::predict(boost::python::numpy::ndarray const & np_X) {
    double *X = reinterpret_cast<double *>(np_X.get_data());        
    std::vector<double> results;
    for (int index_row = 0; index_row < np_X.shape(0); index_row ++){
        results.push_back(_traverse(&this->node_0, &X[index_row * this->number_of_cols]));
    }
    boost::python::numpy::ndarray result = boost::python::numpy::from_data(results.data(),  
                                    boost::python::numpy::dtype::get_builtin<double>(),  
                                    boost::python::make_tuple(results.size()), 
                                    boost::python::make_tuple(sizeof(double)), 
                                    boost::python::object());  
    boost::python::numpy::ndarray output_array = result.copy();
    return output_array;
}

uintptr_t lltree::get_address() {
    return reinterpret_cast<uintptr_t>(this);
}

void lltree::set_criterion(std::string criterion_name) {
    this->tree_criterion.set(criterion_name);
}

std::string lltree::get_criterion() {
    return this->tree_criterion.get();
}

void lltree::list_criterion() {
    this->tree_criterion.print();
}

double lltree::impurity(const std::vector<double> Y) {
    if(Y.empty()){ return 0; }    
    return this->tree_criterion.get(Y);
}

void lltree::print() {
    this->_print_tree(&this->node_0);
}

void lltree::test() {
    std::cout << "12345" << std::endl;
}

 void lltree::_print_tree(node* node) {
    if (!node->isleaf) {
        this->_print_tree(node->get_l_children());
        this->_print_tree(node->get_r_children());
    }
    else {node->print();}
}
        
void lltree::_grow(node* pnode, double *X , double *Y, const std::vector<int> index) {
    bool depth = (lltree::max_depth >= (pnode->tree_level + 1));
    if (depth) {    
        for (int index_col = 0; index_col < this->number_of_cols; index_col ++) {
            std::vector<double> col;
            for(const int& index_row : index) {
                col.push_back(X[index_row * this->number_of_cols + index_col]); 
            }
            std::sort(col.begin(), col.end());
            col.erase(std::unique(col.begin(), col.end()), col.end());
            
            for(long unsigned int idx = 0; idx < col.size() - 1; idx++) {
                double threshold = (col[idx] + col[idx + 1])/2;
                std::vector<double> l_Y, r_Y;
                for(const int& index_row : index) {
                    if (X[index_row * this->number_of_cols + index_col] < threshold) {
                        l_Y.push_back(Y[index_row]);
                    }
                    else {
                        r_Y.push_back(Y[index_row]);
                    }
                }
                double loss = (l_Y.size()/(double) index.size())*impurity(l_Y) + (r_Y.size()/(double) index.size())*impurity (r_Y);
                if ((loss < pnode->saved_loss) && (r_Y.size() >= lltree::min_size_split) && (l_Y.size() >= lltree::min_size_split)) {
                    pnode->saved_loss = loss;
                    pnode->saved_col_index = index_col;
                    pnode->saved_threshold = threshold;
                    pnode->isleaf = false;  
                    pnode->l_size = l_Y.size();  
                    pnode->r_size = r_Y.size();
                }
            }  
        }
    }
    if (!pnode->isleaf) {
        unsigned int l_arr_idx = 0;
        unsigned int r_arr_idx = 0;
        std::vector<int> l_index(pnode->l_size);
        std::vector<int> r_index(pnode->r_size);
        for(const int& index_row : index) {
            if (X[index_row * this->number_of_cols +  pnode->saved_col_index] < pnode->saved_threshold) {  
                l_index[l_arr_idx++] = index_row;
            }
            else {        
                r_index[r_arr_idx++] = index_row;
            }
        }            
        node* l_node = new node(pnode->tree_level + 1);
        node* r_node = new node(pnode->tree_level + 1);
        pnode->set_children(l_node, r_node);
        this->_grow(l_node, X, Y, l_index);
        this->_grow(r_node, X, Y, r_index);
    } 
    else {
        double average = 0;
        for(const int& index_row : index) {average = average + Y[index_row];}                
        pnode->leaf_value = average / index.size(); 
        pnode->isleaf = true ;  
    }  
}
    
double lltree::_traverse(node* pnode, double* row) {
    if (pnode->isleaf){ return(pnode->leaf_value); }
    
    if (row[pnode->saved_col_index] < pnode->saved_threshold) {
        return(this->_traverse(pnode->get_l_children(),row)); 
    }
    else {
        return(this->_traverse(pnode->get_r_children(),row)); 
    }
}