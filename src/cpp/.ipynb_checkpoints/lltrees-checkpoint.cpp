#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

#include "lltree.hpp"

class lltrees{
private :
    int epochs;
    double learning_rate = 0.1;
    std::vector<lltree> v_lltrees;
    std::vector<double> residuals;

public:
    lltrees() : epochs(250) {} //default constructor;

    void fit(boost::python::numpy::ndarray const & np_X, boost::python::numpy::ndarray const & np_Y) {
        //double* Y= reinterpret_cast<double *>(np_Y.get_data()); 
        double* X = reinterpret_cast<double *>(np_X.get_data());  
        
        boost::python::numpy::ndarray Y_copy = np_Y.copy();
        double *residuals= reinterpret_cast<double *>(Y_copy.get_data()); 
        for (int epoch = 0; epoch < this->epochs; epoch++){
            lltree tree;
            tree.fit(X, residuals, np_X.shape(0), np_X.shape(1));
            this->v_lltrees.push_back(tree);
            
            std::vector<double> pred = tree.predict(X, np_X.shape(0), this->learning_rate);

            double average = 0;
            for (int index = 0; index < np_Y.shape(0); index ++){
                residuals[index] = residuals[index] - pred[index];
                average = average + residuals[index];
            }
            this->residuals.push_back(average / np_Y.shape(0));
        }
    }

    boost::python::numpy::ndarray get_residuals() {
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(this->residuals.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(this->residuals.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        double* X = reinterpret_cast<double *>(np_X.get_data());  
        std::vector<double> results(np_X.shape(0));
        std::generate(results.begin(), results.end(), [&](){return 0;});
        for (lltree& tree : v_lltrees){
            std::vector<double> result = tree.predict(X, np_X.shape(0), this->learning_rate);
            std::transform (results.begin(), results.end(), result.begin(), results.begin(), std::plus<double>());
        }
        
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(results.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(results.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    void test() {
        std::cout << "12345" << std::endl;
    }
};

BOOST_PYTHON_MODULE(lltrees) {
    Py_Initialize();
    boost::python::numpy::initialize();
    
    boost::python::class_<lltrees>("lltree", boost::python::init())
        .def("fit", &lltrees::fit)
        .def("predict", &lltrees::predict)
        .def("get_residuals", &lltrees::get_residuals)
        .def("test", &lltrees::test);
}
