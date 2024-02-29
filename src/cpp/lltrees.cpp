#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/tuple.hpp>
// #include <boost/python/stl_iterator.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/str.hpp>

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <iomanip>

#include "lib_gbt.hpp"

class lltrees {
private :
    lib_gbt gbt;

public:
    lltrees() {} //default constructor;
    // lltrees(boost::python::dict d) : {this->set_conf(d);} ; 

    void set_conf(boost::python::dict d) {
        auto items = d.attr("items")();
        for (auto it = boost::python::stl_input_iterator<boost::python::tuple>(items); 
                it != boost::python::stl_input_iterator<boost::python::tuple>(); ++it) {
            boost::python::tuple kv = *it;
            std::string key = boost::python::extract<std::string>(boost::python::str(kv[0]));
            //std::cout<<"set_conf"<<key << std::endl;
            if (key == "epochs") {this->gbt.epochs = boost::python::extract<int>(kv[1]);}
            else if (key == "learning_rate") { this->gbt.learning_rate = boost::python::extract<double>(kv[1]); }
            else if (key == "boost_algo_name") { this->gbt.algo_name = boost::python::extract<std::string>(kv[1]); }
            else if (key == "max_depth") { this->gbt.tree_max_depth = boost::python::extract<int>(kv[1]); }
            else if (key == "min_leaf_size") { this->gbt.tree_min_leaf_size = boost::python::extract<int>(kv[1]); }
            else if (key == "mode") { this->gbt.tree_mode = boost::python::extract<std::string>(kv[1]);}
            else if (key == "verbose") { this->gbt.verbose = boost::python::extract<int>(kv[1]); }
            else if (key == "metric") { this->gbt.metric.set_name(boost::python::extract<std::string>(kv[1])); }
            else if (key == "criterion") { 
                this->gbt.tree_criterion = new criterion(); 
                // this->gbt.tree_criterion = new variance<double>(); criterionMap[boost::python::extract<std::string>(kv[1])]();}
                this->gbt.tree_criterion->set_name(boost::python::extract<std::string>(kv[1])); }
        }
    }

    void get_conf() {
        std::cout << std::left << std::setw(20) << "mode : " << this->gbt.tree_mode << std::endl;
        std::cout << std::setw(20) << "algo_name : " << this->gbt.tree_max_depth << std::endl;
        std::cout << std::setw(20) << "epochs : " << this->gbt.epochs << std::endl;
        std::cout << std::setw(20) << "learning_rate : " << this->gbt.learning_rate << std::endl;
        std::cout << std::setw(20) << "max_depth : " << this->gbt.algo_name << std::endl;
        std::cout << std::setw(20) << "min_leaf_size : " << this->gbt.tree_min_leaf_size << std::endl;
        std::cout << std::setw(20) << "criterion : " << this->gbt.tree_criterion->get_name() << std::endl;
        std::cout << std::setw(20) << "metric : " << this->gbt.metric.get_name() << std::endl;
        std::cout << std::setw(20) << "verbose : " << std::boolalpha << this->gbt.verbose << std::endl;
    }

    void fit(const boost::python::numpy::ndarray& np_x_tr, 
             const boost::python::numpy::ndarray& np_y_tr, 
             const boost::python::numpy::ndarray& np_x_ts,
             const boost::python::numpy::ndarray& np_y_ts) {
        std::string dt =  boost::python::extract<std::string>(boost::python::str(np_y_tr.get_dtype()));
        if (np_y_tr.get_dtype() == boost::python::numpy::dtype::get_builtin<double>()) {
            std::cout << "Datatype is: " << dt << std::endl ;
            this->_fit<double>(np_x_tr, np_y_tr, np_x_ts, np_y_ts);
        } else if(dt == "int64") {   
            std::cout << "Datatype is: " << dt << std::endl ;         
            this->_fit<int>(np_x_tr, np_y_tr, np_x_ts, np_y_ts);
        }
    }

    template<typename T> void _fit(const boost::python::numpy::ndarray & np_x_tr, 
                 const boost::python::numpy::ndarray & np_y_tr, 
                 const boost::python::numpy::ndarray & np_x_ts,
                 const boost::python::numpy::ndarray & np_y_ts) {
            double* xtr_ptr = reinterpret_cast<double *>(np_x_tr.get_data()); 
            std::vector<double> x_tr(xtr_ptr, xtr_ptr + np_x_tr.shape(0) * np_x_tr.shape(1)); 
            T* ytr_ptr = reinterpret_cast<T *>(np_y_tr.get_data()); 
            std::vector<T> y_tr(ytr_ptr, ytr_ptr + np_y_tr.shape(0)); 

            int number_of_rows = np_x_tr.shape(1);      
        
            int _size_x = 0, _size_y = 0;
            double* xts_ptr = reinterpret_cast<double *>(np_x_ts.get_data()); 
            T* yts_ptr = reinterpret_cast<T *>(np_y_ts.get_data()); 
            if (*np_x_ts.get_shape() !=0 ) {
                _size_y = np_x_ts.shape(0) * np_x_ts.shape(1);
                _size_y = np_y_ts.shape(0);
            }
        
            std::vector<double> x_ts(xts_ptr, xts_ptr + _size_x); 
            std::vector<T> y_ts(yts_ptr, yts_ptr + _size_y); 

            this->gbt.fit<T>(x_tr, y_tr, number_of_rows, x_ts, y_ts);
            //https://en.cppreference.com/w/cpp/language/reinterpret_cast  
        }

    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        double* xtr_ptr = reinterpret_cast<double *>(np_X.get_data()); 
        std::vector<double> X(xtr_ptr, xtr_ptr + np_X.shape(0) * np_X.shape(1));  
        std::vector<double> results = this->gbt.predict(X);

        boost::python::numpy::ndarray result = boost::python::numpy::from_data(results.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(results.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    boost::python::numpy::ndarray get_residuals() {
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(this->gbt.residuals.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(this->gbt.residuals.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    void save() {
        std::cout<<"save"<<std::endl;
    }
};

BOOST_PYTHON_MODULE(lltrees) {
    Py_Initialize();
    boost::python::numpy::initialize();

    boost::python::class_<lltrees>("lltree", boost::python::init())
        .def("fit", &lltrees::fit, (boost::python::arg("np_x_tr"), 
                                    boost::python::arg("np_y_tr"), 
                                    boost::python::arg("np_x_ts") = boost::python::numpy::array(boost::python::list()),
                                    boost::python::arg("np_y_ts") = boost::python::numpy::array(boost::python::list())))
        .def("predict", &lltrees::predict)
        .def("get_residuals", &lltrees::get_residuals)
        .def("set_conf", &lltrees::set_conf)
        .def("get_conf", &lltrees::get_conf)
        .def("save", &lltrees::save)
        ;
}
