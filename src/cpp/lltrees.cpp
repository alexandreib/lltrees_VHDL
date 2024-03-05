#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <map>
#include "conf.hpp"
#include "wrapper.hpp"
#include "gbt.hpp"

configuration conf_global = {1, "regression", "variance", "mae"};

class lltrees {
private :
    Gbt* gbt;

public:
    lltrees(std::string mode) {
        conf_global.mode =  mode;
        this->gbt = gbt_Factory(mode);
    }

    void set_conf(boost::python::dict d) {
        auto items = d.attr("items")();
        for (auto it = boost::python::stl_input_iterator<boost::python::tuple>(items); 
                it != boost::python::stl_input_iterator<boost::python::tuple>(); ++it) {
            boost::python::tuple kv = *it;
            std::string key = boost::python::extract<std::string>(boost::python::str(kv[0]));
            if (key == "epochs") {this->gbt->epochs = boost::python::extract<int>(kv[1]);}
            if (key == "learning_rate") { this->gbt->learning_rate = boost::python::extract<double>(kv[1]); }
            // if (key == "boost_algo_name") { this->gbt.algo_name = boost::python::extract<std::string>(kv[1]); }
            if (key == "max_depth") { conf_trees.max_depth = boost::python::extract<int>(kv[1]); }
            if (key == "min_leaf_size") { conf_trees.min_leaf_size = boost::python::extract<int>(kv[1]); }
            if (key == "verbose") { conf_global.verbose = boost::python::extract<int>(kv[1]); 
        // std::cout << std::setw(20) << "verbose : " << std::boolalpha << this->gbt->verbose << std::endl;
            }
            // if (key == "metric") { this->gbt.metric.set_name(boost::python::extract<std::string>(kv[1])); }
            if (key == "criterion") { conf_global.criterion_name = boost::python::extract<std::string>(kv[1]); }
        }
    }

    void get_conf() {
        std::cout << std::left << std::setw(20) << "mode : " << conf_global.mode << std::endl;
        // std::cout << std::setw(20) << "algo_name : " << this->gbt.tree_max_depth << std::endl;
        std::cout << std::setw(20) << "epochs : " << this->gbt->epochs << std::endl;
        std::cout << std::setw(20) << "learning_rate : " << this->gbt->learning_rate << std::endl;
        std::cout << std::setw(20) << "max_depth : " << conf_trees.max_depth << std::endl;
        std::cout << std::setw(20) << "min_leaf_size : " << conf_trees.min_leaf_size << std::endl;
        std::cout << std::setw(20) << "criterion : " << conf_global.criterion_name << std::endl;
        // std::cout << std::setw(20) << "metric : " << this->gbt.metric.get_name() << std::endl;
        std::cout << std::setw(20) << "verbose : " << std::boolalpha << conf_global.verbose << std::endl;
    }

    // void fit(const boost::python::numpy::ndarray& np_x_tr, const boost::python::numpy::ndarray& np_y_tr, 
    //          const boost::python::numpy::ndarray& np_x_ts, const boost::python::numpy::ndarray& np_y_ts) {
    //     std::string dt =  boost::python::extract<std::string>(boost::python::str(np_y_tr.get_dtype()));
    //     if (np_y_tr.get_dtype() == boost::python::numpy::dtype::get_builtin<double>()) {
    //         std::cout << "Datatype is: " << dt << std::endl ;
    //         this->_fit<double>(np_x_tr, np_y_tr, np_x_ts, np_y_ts);
    //     } else if(dt == "int64") {   
    //         std::cout << "Datatype is: " << dt << std::endl ;         
    //         this->_fit<int>(np_x_tr, np_y_tr, np_x_ts, np_y_ts);
    //     }
    // }

    void fit(const boost::python::numpy::ndarray & x_tr,  const boost::python::numpy::ndarray & y_tr, 
             const boost::python::numpy::ndarray & x_va = boost::python::numpy::array(boost::python::list()),
             const boost::python::numpy::ndarray & y_va = boost::python::numpy::array(boost::python::list())) {
        std::string dt =  boost::python::extract<std::string>(boost::python::str(y_tr.get_dtype()));
        std::cout << "Type of Training Data : "<< dt << std::endl;
        std::cout << "Configuration mode : "<< conf_global.mode << std::endl;
        std::unique_ptr<data> tr = data_Factory( conf_global.mode );
        tr->set_xy(x_tr, y_tr);
        tr->create_index();
        
        if (x_va.get_shape()[0] !=0) {
            std::unique_ptr<data> va = data_Factory( conf_global.mode );
            va->set_xy(x_va, y_va);
            this->gbt->fit(*tr, *va);
        } else {
            std::cout << "No Validate Data, will use Training Data." << std::endl;
            this->gbt->fit(*tr, *tr);
        }
    }
    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        std::cout << "Configuration mode : "<< conf_global.mode << std::endl;
        std::unique_ptr<data> x = data_Factory( conf_global.mode );
        x->set_x(np_X);
        std::vector<double> preds = this->gbt->predict(*x);

        boost::python::numpy::ndarray result = boost::python::numpy::from_data(preds.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(preds.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    // boost::python::numpy::ndarray get_residuals() {
    //     boost::python::numpy::ndarray result = boost::python::numpy::from_data(this->gbt.residuals.data(),  
    //                             boost::python::numpy::dtype::get_builtin<double>(),  
    //                             boost::python::make_tuple(this->gbt.residuals.size()), 
    //                             boost::python::make_tuple(sizeof(double)), 
    //                             boost::python::object());  
    //     return result.copy();
    // }

    void save() {
        std::cout<<"save"<<std::endl;
    }
};

BOOST_PYTHON_MODULE(lltrees) {
    Py_Initialize();
    boost::python::numpy::initialize();

    boost::python::class_<lltrees>("lltree", boost::python::init<const std::string &>())
        .def("fit", &lltrees::fit, (boost::python::arg("x_tr"), 
                                    boost::python::arg("y_tr"), 
                                    boost::python::arg("x_va") = boost::python::numpy::array(boost::python::list()),
                                    boost::python::arg("y_va") = boost::python::numpy::array(boost::python::list())))
        .def("predict", &lltrees::predict)
        // .def("get_residuals", &lltrees::get_residuals)
        .def("set_conf", &lltrees::set_conf)
        .def("get_conf", &lltrees::get_conf)
        .def("save", &lltrees::save)
        ;
}
