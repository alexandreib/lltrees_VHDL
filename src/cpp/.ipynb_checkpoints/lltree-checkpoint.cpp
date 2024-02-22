#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include "node.hpp"
#include "criterion.hpp"

#include <chrono>

class lltree{
private :
    int max_depth, number_of_cols;
    long unsigned int min_size_split;
    node node_0;
    criterion tree_criterion;

    void _print_tree(node* node) {
        if (!node->isleaf) {
            this->_print_tree(node->get_l_children());
            this->_print_tree(node->get_r_children());
        }
        else {node->print();}
    }  

    void _grow(node* pnode, double *X , double *Y, const std::vector<int> index) {
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

    double _traverse(node* pnode, double *row) {
        if (pnode->isleaf){ return(pnode->leaf_value); }
        
        if (row[pnode->saved_col_index] < pnode->saved_threshold) {
            return(_traverse(pnode->get_l_children(),row)); 
        }
        else {
            return(_traverse(pnode->get_r_children(),row)); 
        }
    }
public:
    lltree() : max_depth(5), min_size_split(2) {} //default constructor

   void fit(boost::python::numpy::ndarray const & np_X, boost::python::numpy::ndarray const & np_Y) {
        // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        double *X = reinterpret_cast<double *>(np_X.get_data());
        double *Y = reinterpret_cast<double *>(np_Y.get_data());
        this->number_of_cols = np_X.shape(1);
        std::vector<int> index(np_X.shape(0));
        int startValue = 0;
        std:: generate (index.begin(), index.end(), [&](){return startValue++;});
        this->_grow(&this->node_0, X, Y, index);
        // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();        
        // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
        // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    }


    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        double *X = reinterpret_cast<double *>(np_X.get_data());        
        std::vector<double> results;
        for (int index_row = 0; index_row < np_X.shape(0); index_row ++){
            results.push_back(_traverse(&this->node_0, &X[index_row * this->number_of_cols]));
        }

        // Py_intptr_t shape[1] = { (Py_intptr_t) results.size() };
        // boost::python::numpy::ndarray result = boost::python::numpy::zeros(1, shape, boost::python::numpy::dtype::get_builtin<double>());
        // std::copy(results.begin(), results.end(), reinterpret_cast<double*>(result.get_data()));
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(results.data(),  
                                        boost::python::numpy::dtype::get_builtin<double>(),  
                                        boost::python::make_tuple(results.size()), 
                                        boost::python::make_tuple(sizeof(double)), 
                                        boost::python::object());  
        boost::python::numpy::ndarray output_array = result.copy();
        return output_array;
    } 

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

    void print() {
        this->_print_tree(&this->node_0);
    }  

    void test() {
        std::cout << "1234" << std::endl;
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