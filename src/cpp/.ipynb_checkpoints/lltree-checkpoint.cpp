#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include "node.hpp"
#include "criterion.hpp"


class lltree{
private :
    int max_depth;
    long unsigned int min_size_split;
    node node_0;
    criterion tree_criterion;

    void _grow(node* pnode, std::vector<std::vector<double>> XY) {
        bool depth = (lltree::max_depth >= (pnode->tree_level + 1));
        bool msamp = (lltree::min_size_split <= (XY.size()));
        
        size_t index_col_Y = XY[0].size() - 1;
        size_t n_rows = XY.size();
        
        if (depth && msamp) {    
            for (size_t index_col = 0; index_col < index_col_Y; index_col ++) {
                std::vector<double> col;
                for (size_t index_row = 0; index_row < n_rows; index_row ++){
                    col.push_back(XY[index_row][index_col]);
                }
                
                std::sort(col.begin(), col.end());
                col.erase(std::unique(col.begin(), col.end()), col.end());

                for(size_t idx = 0; idx < col.size() - 1; idx++) {
                    double threshold = (col[idx] + col[idx + 1])/2;
                    std::vector<double> l_Y, r_Y;
                    for (size_t index_row = 0; index_row < n_rows; index_row ++){
                        if (XY[index_row][index_col] < threshold) {
                            l_Y.push_back(XY[index_row][index_col_Y]);
                        }
                        else {
                            r_Y.push_back(XY[index_row][index_col_Y]);
                        }
                    }
                    double dj = double(idx);
                    double loss = (dj/(double) n_rows)*impurity(l_Y) + (((double) n_rows - dj)/(double) n_rows)*impurity (r_Y);
                    if ((loss < pnode->saved_loss) && (r_Y.size() >= lltree::min_size_split) && (l_Y.size() >= lltree::min_size_split)) {
                        pnode->saved_loss = loss;
                        pnode->saved_col_index = index_col;
                        pnode->saved_threshold = threshold;
                        pnode->isleaf = false ;  
                    }
                }  
            }
        }
        
        if (!pnode->isleaf) {
            std::vector<std::vector<double>> l_XY, r_XY;
            for (size_t index_row = 0; index_row < n_rows; index_row ++){
                if (XY[index_row][pnode->saved_col_index] < pnode->saved_threshold) 
                    {l_XY.push_back(XY[index_row]);}
                else 
                    {r_XY.push_back(XY[index_row]);}
            }
            node* l_node = new node(pnode->tree_level + 1);
            node* r_node = new node(pnode->tree_level + 1);
            pnode->set_children(l_node, r_node);
            this->_grow(l_node, l_XY);
            this->_grow(r_node, r_XY);
        } 
        else {
            double average = 0;
            for (size_t index_row = 0; index_row < n_rows; index_row ++){
                average = average + XY[index_row][index_col_Y];
            }                
            pnode->leaf_value = average / n_rows; 
            pnode->isleaf = true ;  
        }  
        
    }

    double _traverse(node* pnode, std::vector<double> row) {
        if (pnode->isleaf){
            return(pnode->leaf_value);
        }
        
        if (row[pnode->saved_col_index] < pnode->saved_threshold) {
            return(_traverse(pnode->get_l_children(),row)); 
        }
        else {
            return(_traverse(pnode->get_r_children(),row)); 
        }
    }

    void _print_tree(node* node) {
        if (!node->isleaf) {
            this->_print_tree(node->get_l_children());
            this->_print_tree(node->get_r_children());
        }
        else {node->print();}
    }  

public:
    lltree() : max_depth(5), min_size_split(2) {} //default constructor to be remove (need at least a loss function).
    //lltree(int max_depth, int min_size_split) : max_depth(max_depth), min_size_split(min_size_split) {}
    // No Copy constructor (no need).
    // No Copy assigment constructor (no need).

    // Utility method to get the address of the instance.
    uintptr_t get_address() const {
        return reinterpret_cast<uintptr_t>(this);
    }

    void set_criterion(std::string criterion_name) {
        this->tree_criterion.set(criterion_name);
    }

    std::string get_criterion() {
        return this->tree_criterion.get();
    }

    void list_criterion() {
        this->tree_criterion.print();
    }

    double impurity(const std::vector<double> Y) {
        if(Y.empty()){ return 0; }    
        return this->tree_criterion.get(Y);
    }    

    void fit(boost::python::numpy::ndarray const & np_X, boost::python::numpy::ndarray const & np_Y) {
        double *X = reinterpret_cast<double *>(np_X.get_data());
        double *Y = reinterpret_cast<double *>(np_Y.get_data());
        std::vector<std::vector<double>> XY;
        for (int i = 0; i < np_X.shape(0); i++) {
            std::vector<double> row;
            for (int j = 0; j < np_X.shape(1); j++) {row.push_back(X[i * np_X.shape(1) + j ]);} 
            row.push_back(Y[i]);  
            XY.push_back(row);
        }
        this->_grow(&this->node_0, XY);
    }

    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        double *X_arr = reinterpret_cast<double *>(np_X.get_data());
        std::vector<std::vector<double>> X;
        for (int i = 0; i < np_X.shape(0); i++) {
            std::vector<double> row;
            for (int j = 0; j < np_X.shape(1); j++) {row.push_back(X_arr[i * np_X.shape(1) + j ]);} 
            X.push_back(row);
        }
        
        // double* results = new double[np_X.shape(0)];
        std::vector<double> results;
        for (size_t index_row = 0; index_row < np_X.shape(0); index_row ++){
            double r = _traverse(&this->node_0, X[index_row]); 
            results.push_back(r);
            // results[index_row] = _traverse(&this->node_0, X[index_row]); 
        }

        Py_intptr_t shape[1] = { results.size() };
        boost::python::numpy::ndarray result = boost::python::numpy::zeros(1, shape, boost::python::numpy::dtype::get_builtin<double>());
        std::copy(results.begin(), results.end(), reinterpret_cast<double*>(result.get_data()));
    
        // boost::python::numpy::ndarray result = boost::python::numpy::from_data(results,  
        //                                 boost::python::numpy::dtype::get_builtin<double>(),  
        //                                 boost::python::make_tuple(X.size()), 
        //                                 boost::python::make_tuple(sizeof(double)), boost::python::object());  
        return result;
    } 

    void print() {
        this->_print_tree(&this->node_0);
    }  

    void test() {
        std::cout << "1234" << std::endl;
    }  


    void print_XY(const std::vector<std::vector<double>> X) {
        for (size_t i = 0; i < X.size(); i ++){
            std::vector<double> row = X[i];
            for (size_t j = 0; j < row.size(); j ++){std::cout << row[j] << " ";}
            std::cout << std::endl;
        }
    }  
    void print_XY_shape(const std::vector<std::vector<double>> X) {
        std::cout << "[" << X.size() << "," << X[0].size() << "]" << std::endl;
    }  
};

BOOST_PYTHON_MODULE(lltree) {
    Py_Initialize();
    boost::python::numpy::initialize();
    
    boost::python::class_<lltree>("tree", boost::python::init())
        .def("get_address", &lltree::get_address)
        .def("fit", &lltree::fit)
        .def("predict", &lltree::predict)
        .def("print", &lltree::print)
        .def("list_criterion", &lltree::list_criterion)
        .def("get_criterion", &lltree::get_criterion)
        .def("set_criterion", &lltree::set_criterion)
        .def("test", &lltree::test);
}