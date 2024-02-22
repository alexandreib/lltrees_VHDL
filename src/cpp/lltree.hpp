#ifndef __LLTREE_H_INCLUDED__
#define __LLTREE_H_INCLUDED__
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include "node.hpp"
#include "criterion.hpp"

class lltree{
private :
    int max_depth, number_of_cols;
    long unsigned int min_size_split;
    node node_0;
    criterion tree_criterion;
   
public:
    lltree();
    double _traverse(node* pnode, double* row);
    void _print_tree(node* node);
    void _grow(node* pnode, double *X , double *Y, const std::vector<int> index);
    void fit(boost::python::numpy::ndarray const & np_X, boost::python::numpy::ndarray const & np_Y);
    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X);
    uintptr_t get_address();
    void set_criterion(std::string criterion_name);
    std::string get_criterion();
    void list_criterion();
    double impurity(std::vector<double>);
    void print(); 
    void test();
};

// BOOST_PYTHON_MODULE(lltree) {
//     Py_Initialize();
//     boost::python::numpy::initialize();
    
//     boost::python::class_<lltree>("tree", boost::python::init())
//         .def("get_address", &lltree::get_address)
//         .def("fit", &lltree::fit)
//         .def("predict", &lltree::predict)
//         .def("print", &lltree::print)
//         .def("list_criterion", &lltree::list_criterion)
//         .def("get_criterion", &lltree::get_criterion)
//         .def("set_criterion", &lltree::set_criterion)
//         .def("test", &lltree::test);
// }

#endif // __LLTREE_H_INCLUDED__ 