#include <iostream>
#include "conf.hpp"
#include "factories.hpp"

// default configuration
configuration conf_global = {1, "regression", "variance", "mae", 1, 1 };

class lltrees {
private :
    Gbt* gbt;

public:
    lltrees() {}
    ~lltrees() {delete this->gbt; }

    void set_conf(boost::python::dict d) {
        auto items = d.attr("items")();
        for (auto it = boost::python::stl_input_iterator<boost::python::tuple>(items); 
                it != boost::python::stl_input_iterator<boost::python::tuple>(); ++it) {
            boost::python::tuple kv = *it;
            std::string key = boost::python::extract<std::string>(boost::python::str(kv[0]));
            if (key == "mode") {conf_global.mode = boost::python::extract<std::string>(kv[1]);}
            // if (key == "boost_algo_name") { // feature to develop : select boosting type}
            if (key == "epochs") {conf_global.epochs = boost::python::extract<int>(kv[1]);}
            if (key == "learning_rate") { conf_global.learning_rate = boost::python::extract<double>(kv[1]); }
            if (key == "metric") { conf_global.metric_name = boost::python::extract<std::string>(kv[1]); }
            if (key == "criterion") { conf_global.criterion_name = boost::python::extract<std::string>(kv[1]); }
            if (key == "max_depth") { conf_trees.max_depth = boost::python::extract<int>(kv[1]); }
            if (key == "min_leaf_size") { conf_trees.min_leaf_size = boost::python::extract<int>(kv[1]); }
            if (key == "verbose") { conf_global.verbose = boost::python::extract<int>(kv[1]);}
        }
    }

    void get_conf() {
        std::cout << std::left << std::setw(20) << "mode : " << conf_global.mode << std::endl;
        // std::cout << std::setw(20) << "boost_algo_name : " << << std::endl;
        std::cout << std::setw(20) << "epochs : " << conf_global.epochs << std::endl;
        std::cout << std::setw(20) << "learning_rate : " << conf_global.learning_rate << std::endl;
        std::cout << std::setw(20) << "metric : " << conf_global.metric_name << std::endl;
        std::cout << std::setw(20) << "criterion : " << conf_global.criterion_name << std::endl;
        std::cout << std::setw(20) << "max_depth : " << conf_trees.max_depth << std::endl;
        std::cout << std::setw(20) << "min_leaf_size : " << conf_trees.min_leaf_size << std::endl;
        std::cout << std::setw(20) << "verbose : " << std::boolalpha << conf_global.verbose << std::endl;
    }


    void fit(const boost::python::numpy::ndarray & x_tr,  const boost::python::numpy::ndarray & y_tr, 
             const boost::python::numpy::ndarray & x_va = boost::python::numpy::array(boost::python::list()),
             const boost::python::numpy::ndarray & y_va = boost::python::numpy::array(boost::python::list())) {
        std::string dt =  boost::python::extract<std::string>(boost::python::str(y_tr.get_dtype()));
        // need to add asser to check dt and mode
        std::cout << "Type of Training Data : "<< dt << std::endl;
        std::cout << "Configuration mode : "<< conf_global.mode << std::endl;
        
        this->gbt = gbt_Factory();
        std::unique_ptr<data> tr = data_Factory();
        tr->set_xy(x_tr, y_tr);
        tr->create_index();
        
        if (x_va.get_shape()[0] !=0) {
            std::unique_ptr<data> va = data_Factory();
            va->set_xy(x_va, y_va);
            this->gbt->fit(*tr, *va);
        } else {
            std::cout << "No Validate Data, will use Training Data." << std::endl;
            this->gbt->fit(*tr, *tr);
        }
    }
    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        std::cout << "Configuration mode : "<< conf_global.mode << std::endl;
        std::unique_ptr<data> x = data_Factory();
        x->set_x(np_X);
        std::vector<double> preds = this->gbt->predict(*x);

        boost::python::numpy::ndarray result = boost::python::numpy::from_data(preds.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(preds.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    boost::python::numpy::ndarray get_residuals() {
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(this->gbt->residuals_average.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(this->gbt->residuals_average.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    void save() {
        std::cout<<"save"<<std::endl;
    }

    // static boost::python::object enter(boost::python::object self) {
    //     //lltrees& myself = boost::python::extract<lltrees&>(self);
    //     return self;
    //   }

    // bool exit(boost::python::object type, boost::python::object value, boost::python::object traceback) {
    //     delete gbt;
    //     return false; 
    // }
};

BOOST_PYTHON_MODULE(lltrees) {
    Py_Initialize();
    boost::python::numpy::initialize();
    // boost::python::class_<lltrees>("lltrees", boost::python::no_init// const std::string &
    //     .def("lltree", &lltrees::lltree)
    // boost::python::class_<lltrees>("lltrees", boost::python::init<>())
    // def("create", create, return_value_policy<manage_new_object>());
    
    boost::python::class_<lltrees>("lltree", boost::python::init<>())
        .def("fit", &lltrees::fit, (boost::python::arg("x_tr"), 
                                    boost::python::arg("y_tr"), 
                                    boost::python::arg("x_va") = boost::python::numpy::array(boost::python::list()),
                                    boost::python::arg("y_va") = boost::python::numpy::array(boost::python::list())))
        .def("predict", &lltrees::predict)
        .def("get_residuals", &lltrees::get_residuals)
        .def("set_conf", &lltrees::set_conf)
        .def("get_conf", &lltrees::get_conf)
        .def("save", &lltrees::save)
        ;
}
