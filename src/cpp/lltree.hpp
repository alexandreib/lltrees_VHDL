#ifndef __LLTREE_H_INCLUDED__
#define __LLTREE_H_INCLUDED__
// #include <boost/move/core.hpp>
#include "node.hpp"
#include "criterion.hpp"

class lltree{
private :
    // BOOST_COPY_ASSIGN_REF(lltree)
    int id_node, tree_max_depth, number_of_cols;
    long unsigned int tree_min_leaf_size;
    criterion* ptr_criterion;
    std::string tree_mode;

    void _print_tree(node* node) {
        if (!node->isleaf) {
            node->print();
            this->_print_tree(node->get_l_children());
            this->_print_tree(node->get_r_children());
        }
        else {node->print();}
    }
            
    template<typename T> void _grow(node* pnode, const std::vector<double>& X, const std::vector<T>& Y, const std::vector<int>& index) {
        // bool depth = (this->tree_max_depth > pnode->level);
        // std::cout<< "X " << X[0] << X[this->number_of_cols] << std::endl;
        // std::cout<< "X " << X[0] << X[this->number_of_cols] << std::endl;
        // std::cout<< "Y " << Y[0]  << std::endl;
        // std::cout<< "index " << index[0]  << " " << index[10] <<std::endl;
        if (this->tree_max_depth > pnode->level) {    
            for (int index_col = 0; index_col < this->number_of_cols; index_col ++) {
                std::vector<double> col;
                for(auto & index_row : index) {
                    col.push_back(X[index_row * this->number_of_cols + index_col]); 
                }
                std::sort(col.begin(), col.end());
                col.erase(std::unique(col.begin(), col.end()), col.end());
                
                for(long unsigned int idx = 0; idx < col.size() - 1; idx++) {
                    double threshold = (col[idx] + col[idx + 1])/2;
                    // std::cout<< "threshold " << threshold << std::endl;
                    std::vector<T> l_Y, r_Y;
                    for(auto & index_row : index) {
                        // std::cout<< "index_row " <<index_row  << std::endl;
                        //std::cout<< "X " << X[0]  << std::endl;
                        if (X[index_row * this->number_of_cols + index_col] < threshold) {
                            l_Y.push_back(Y[index_row]);
                        }
                        else {
                            r_Y.push_back(Y[index_row]);
                        }
                        // std::cout<< "index_row " <<index_row  << " " << X[index_row * this->number_of_cols + index_col] << " " << threshold << " "<< Y[index_row] << std::endl;
                    }
                    // std::cout<< "r_Y " << r_Y[0]  << std::endl;
                    double loss = (l_Y.size()/(double) index.size())*this->ptr_criterion->get<T>(l_Y) + (r_Y.size()/(double) index.size())*this->ptr_criterion->get<T>(r_Y);
                    // std::cout<< "l_Y.size() " << l_Y.size()  << std::endl;
                    if ((loss < pnode->loss) && (r_Y.size() >=  this->tree_min_leaf_size) && (l_Y.size() >= this->tree_min_leaf_size)) {
                        pnode->loss = loss;
                        pnode->index_col = index_col;
                        pnode->threshold = threshold;
                        pnode->isleaf = false;  
                        pnode->l_size = l_Y.size();  
                        pnode->r_size = r_Y.size();
                        // pnode->print();
                        //std::cout<< "loss " << loss << " " << threshold << std::endl;
                    }
                }  
            }
        }
        // pnode->print();
        if (!pnode->isleaf) {
            // std::cout<< "!pnode->isleaf " << std::endl;
            int l_arr_idx = 0;
            int r_arr_idx = 0;
            std::vector<int> l_index(pnode->l_size);
            std::vector<int> r_index(pnode->r_size);
            for(auto & index_row : index) {
                if (X[index_row * this->number_of_cols +  pnode->index_col] < pnode->threshold) {  
                    l_index[l_arr_idx++] = index_row;
                }
                else {        
                    r_index[r_arr_idx++] = index_row;
                }
            }            
            //std::cout << "(*this)->id_node  "<<(*this).id_node << std::endl;
            node* l_node = new node(pnode->level+1, ++this->id_node, l_index.size(), pnode->loss);
            node* r_node = new node(pnode->level+1, ++this->id_node, r_index.size(), pnode->loss);
            pnode->set_children(l_node, r_node);
            this->_grow(l_node, X, Y, l_index);
            this->_grow(r_node, X, Y, r_index);
        } 
        else {
            // std::cout<< "pnode->isleaf " << std::endl;
            // std::cout<< "this->tree_mode  " << this->tree_mode << std::endl;
            if (this->tree_mode == "regression"){
                /// Regression : Return Average
                double average = 0;
                for(const int& index_row : index) {
                    // std::cout << "index_row " << index_row << " Y " << Y[index_row] << std::endl;
                    average = average + Y[index_row];
                }                
                pnode->leaf_value = average / index.size(); 
                pnode->isleaf = true ;  
                pnode->size = index.size(); 
                // std::cout<< "average " << pnode->leaf_value  << std::endl;
                // pnode->print();
            }
            else {
                /// Classification : Return Mode
                std::unordered_map<int, int> freqMap; 
                for (long unsigned int i = 0; i < index.size(); i++) { freqMap[Y[i]]++;}  
                auto maxElement = max_element(freqMap.begin(), freqMap.end(), 
                                [](const auto& a, const auto& b) { 
                                  return a.second < b.second; 
                              }); 
                pnode->leaf_value = maxElement->first;
                pnode->isleaf = true ;  
                pnode->size = index.size();
            }
        } 
        // pnode->print();
    }
    
    void _save(node* node) {
        std::cout<<"temp"<<std::endl;
    }         

    double _traverse(node* pnode, const double * row) {
        if (pnode->isleaf){ return (double) pnode->leaf_value; }
        if (row[pnode->index_col] < pnode->threshold) {
            return this->_traverse(pnode->get_l_children(),row);
        }
        else {
            return this->_traverse(pnode->get_r_children(),row);
        }
    }



public:
    node* node_0;
    lltree() {}
    // lltree(const lltree &tree);
    // lltree(const lltree& other) = default;
    lltree(int tree_max_depth, int tree_min_leaf_size, criterion* ptr_criterion, std::string tree_mode) : id_node(0), tree_max_depth(tree_max_depth), tree_min_leaf_size(tree_min_leaf_size), tree_mode(tree_mode) {this->ptr_criterion = ptr_criterion;} //default constructor
    
    template<typename T> void fit(const std::vector<double>& X, const std::vector<T>& Y, const int number_of_cols) {
        this->number_of_cols = number_of_cols; 
        std::vector<int> index(Y.size());
        std::iota(index.begin(), index.end(), 0);
        std::cout<<"tree fit 0 " << Y.size() << " " << number_of_cols << " " << index[0] << " " << index[10]<<  " "<< index[Y.size()-1] <<std::endl;
        static node* nnode = new node(Y.size());
        this->node_0 = nnode;
        this->_grow<T>(this->node_0, X, Y, index);
        // std::cout << "INSIDE TREE FIT" <<std::endl;
        // this->print_node_0();        
        // std::cout << "OUTSIDE TREE FIT" <<std::endl;
    }

    double predict_row(const double* row) {   
        return this->_traverse(this->node_0, row);
    }

    std::vector<double> predict_vector(const std::vector<double>& X,const double lr = 1.0) {
        std::vector<double> preds(X.size() / this->number_of_cols);
        for (long unsigned int index_row = 0; index_row < X.size() / this->number_of_cols; index_row ++){
            preds[index_row] = lr *  this->predict_row(&X[index_row * this->number_of_cols]);
        }
        return preds;
    }
       
    void print_node_0() {
        this->node_0->print();
    }

    void print() {
        this->_print_tree(this->node_0);
    }

    void save() {
        this->_save(this->node_0);
    }

};

#endif // __LLTREE_H_INCLUDED__ 