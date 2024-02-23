#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/str.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

#include "lltree.hpp"
#include "metrics.hpp"

// static boost::python::list _dummy_list ;
// static boost::python::numpy::ndarray const & _dummy_ndarray = boost::python::numpy::array (_dummy_list) ;
// ,
             // boost::python::numpy::ndarray const & np_x_ts = _dummy_ndarray,
             // boost::python::numpy::ndarray const & np_y_ts = _dummy_ndarray
class lltrees{
private :
    int epochs;
    int size_lltree;
    double learning_rate;
    std::string metric_name;

    std::vector<lltree> v_lltrees;
    std::vector<double> residuals;
    metrics metric;

public:
    lltrees() {}; //default constructor;

    void set_conf(boost::python::dict d) {
        auto items = d.attr("items")();
        for (auto it = boost::python::stl_input_iterator<boost::python::tuple>(items); 
                it != boost::python::stl_input_iterator<boost::python::tuple>(); ++it) {
            boost::python::tuple kv = *it;
            std::string key = boost::python::extract<std::string>(boost::python::str(kv[0]));
            if (key == "epochs") {
                this->epochs = boost::python::extract<int>(kv[1]);
            }
            else if (key == "learning_rate") {
                this->learning_rate = boost::python::extract<double>(kv[1]);
            }
            else if (key == "size_lltree") {
                this->size_lltree = boost::python::extract<int>(kv[1]);
            }
            else if (key == "metric_name") {
                this->metric_name = boost::python::extract<std::string>(kv[1]);
                this->metric.set_name(boost::python::extract<std::string>(kv[1]));
            }
            std::cout << "Set : " << key << std::endl;
        }
    }

    void fit(boost::python::numpy::ndarray const & np_x_tr, 
             boost::python::numpy::ndarray const & np_y_tr) {
        //double* Y= reinterpret_cast<double *>(np_Y.get_data()); 
        
        double* x_tr = reinterpret_cast<double *>(np_x_tr.get_data());  
        
        boost::python::numpy::ndarray y_copy = np_y_tr.copy();
        double *residuals= reinterpret_cast<double *>(y_copy.get_data()); 
        for (int epoch = 0; epoch < this->epochs; epoch++){
            lltree tree;
            tree.fit(x_tr, residuals, np_x_tr.shape(0), np_x_tr.shape(1));
            this->v_lltrees.push_back(tree);
            
            std::vector<double> pred = tree.predict(x_tr, np_x_tr.shape(0), this->learning_rate);

            double average = 0;
            for (int index = 0; index < np_y_tr.shape(0); index ++){
                residuals[index] = residuals[index] - pred[index];
                average = average + residuals[index];
            }
            this->residuals.push_back(average / np_y_tr.shape(0));
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
        std::vector<double> results(np_X.shape(0), 0.0);
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
        .def("set_conf", &lltrees::set_conf)
        .def("test", &lltrees::test);
}
