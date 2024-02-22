#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

#include "lltree.hpp"

class boosted_lltrees{
private :
    int epochs;
    double learning_rate = 0.1;
    std::vector<lltree> lltrees;

public:
    boosted_lltrees() : epochs(5) {} //default constructor;

    void fit(boost::python::numpy::ndarray const & np_X, boost::python::numpy::ndarray const & np_Y) {
        boost::python::numpy::ndarray residual = np_Y.copy();
        double *a_Y= reinterpret_cast<double *>(np_Y.get_data());  
        std::cout << "here " << a_Y[0] << std::endl;
        for (int epoch = 0; epoch < this->epochs; epoch++){
            lltree tree;
            tree.fit(np_X, residual);
            lltrees.push_back(tree);
            
            boost::python::numpy::ndarray new_residual = tree.predict(np_X);
            double *a_new_residual= reinterpret_cast<double *>(new_residual.get_data()); 
            std::cout << "new_residual " << a_new_residual[0] << std::endl; 
            double *a_residual = reinterpret_cast<double *>(residual.get_data());  
            for (int index = 0; index < residual.shape(0); index ++){
                a_residual[index] = a_residual[index] - this->learning_rate * a_new_residual[index];
            }
            
            double average = 0;
            for(int idx = 0; idx < residual.shape(0); idx++){
                average = average + std::abs(a_Y[idx] - a_residual[idx]);
            }
            average = average / residual.shape(0);
            std::cout << average <<std::endl;

            
            residual = boost::python::numpy::from_data(a_residual,  
                            boost::python::numpy::dtype::get_builtin<double>(),  
                            boost::python::make_tuple(np_X.shape(0)), 
                            boost::python::make_tuple(sizeof(double)), 
                            boost::python::object());  
        }
    }

    void test() {
        std::cout << "12345" << std::endl;
    }
};

BOOST_PYTHON_MODULE(boosted_lltrees) {
    Py_Initialize();
    boost::python::numpy::initialize();
    
    boost::python::class_<boosted_lltrees>("boosted_trees", boost::python::init())
        .def("fit", &boosted_lltrees::fit)
        .def("test", &boosted_lltrees::test);
}
