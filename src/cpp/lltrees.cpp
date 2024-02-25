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


#include "lltree.hpp"
#include "metrics.hpp"

class lltrees{
private :
    // Parameters of the base tree
    int lltree_max_depth;
    int lltree_min_size_split;
    std::string lltree_criterion;

    // Parameters of the boost
    int epochs;
    double learning_rate;

    // Other configuration parameters
    int verbose;

    metrics metric;
    std::vector<lltree> v_lltrees;
    std::vector<double> residuals;

    std::vector<double> _predict(double* X, int number_of_rows) {
        std::vector<double> results(number_of_rows, 0.0);
        for (lltree& tree : this->v_lltrees){
            std::vector<double> result = tree.predict(X, number_of_rows, this->learning_rate);
            std::transform (results.begin(), results.end(), result.begin(), results.begin(), std::plus<double>());
        }
        return results;
    }

public:
    lltrees() : lltree_max_depth(5), lltree_min_size_split(2), lltree_criterion("value_metric"),
                epochs(10), learning_rate(0.1), 
                verbose(1) {} ; //default constructor;

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
            else if (key == "metric_name") {
                this->metric.set_name(boost::python::extract<std::string>(kv[1]));
            }
            else if (key == "lltree_max_depth") {
                this->lltree_max_depth = boost::python::extract<int>(kv[1]);
            }
            else if (key == "lltree_min_size_split") {
                this->lltree_min_size_split = boost::python::extract<int>(kv[1]);
            }
            else if (key == "lltree_criterion") {
                this->lltree_criterion = boost::python::extract<std::string>(kv[1]);
            }
            else if (key == "verbose") {
                this->verbose = boost::python::extract<bool>(kv[1]);
            }
            std::cout << "Set : " << key << std::endl;
        }
    }

    void get_conf() {
        std::cout << "Get : lltree_max_depth : " << this->lltree_max_depth << std::endl;
        std::cout << "Get : lltree_min_size_split : " << this->lltree_min_size_split << std::endl;
        std::cout << "Get : lltree_criterion : " << this->lltree_criterion << std::endl;
        std::cout << "Get : epochs : " << this->epochs << std::endl;
        std::cout << "Get : learning_rate : " << this->learning_rate << std::endl;
        std::cout << "Get : metric_name : " << this->metric.get_name() << std::endl;
        std::cout << "Get : verbose : " << std::boolalpha <<this->verbose << std::endl;
    }

    void fit(boost::python::numpy::ndarray const & np_x_tr, 
             boost::python::numpy::ndarray const & np_y_tr, 
             boost::python::numpy::ndarray const & np_x_ts = boost::python::numpy::array(boost::python::list()),
             boost::python::numpy::ndarray const & np_y_ts = boost::python::numpy::array(boost::python::list())) {

        double* x_tr = reinterpret_cast<double *>(np_x_tr.get_data()); 
        double* y_tr = reinterpret_cast<double *>(np_y_tr.get_data()); 
        std::vector<double> pred_tr(np_y_tr.shape(0), 0.0);
        double metric_tr = 0.0;
        
        double* x_ts = reinterpret_cast<double *>(np_x_ts.get_data()); 
        double* y_ts = reinterpret_cast<double *>(np_y_ts.get_data()); 
        std::vector<double> pred_ts(np_y_ts.shape(0), 0.0);
        double metric_ts = 0.0;

        boost::python::numpy::ndarray np_y_tr_copy = np_y_tr.copy(); 
        double* residuals = reinterpret_cast<double *>(np_y_tr_copy.get_data());                   
        for (int epoch = 1; epoch < this->epochs + 1; epoch++){
            // Fit Tree
            lltree tree(this->lltree_max_depth, 
                        this->lltree_min_size_split,
                        this->lltree_criterion);
            tree.fit(x_tr, residuals, np_x_tr.shape(0), np_x_tr.shape(1));
            this->v_lltrees.push_back(tree);

            // Predict Tree, tr and metric tr
            std::vector<double> tree_pred = tree.predict(x_tr, np_x_tr.shape(0), this->learning_rate);
            std::transform (pred_tr.begin(), pred_tr.end(), tree_pred.begin(), pred_tr.begin(), std::plus<double>());
            metric_tr = this->metric.get(pred_tr, y_tr);

            // Residuals
            double average = 0;
            for (int index = 0; index < np_y_tr.shape(0); index ++){
                residuals[index] = residuals[index] - tree_pred[index];
                average = average + residuals[index];
            }
            this->residuals.push_back(average / np_y_tr.shape(0));

            // Predict TS and Metric
            if (np_y_ts.shape(0) > 0) {
                tree_pred = tree.predict(x_ts, np_x_ts.shape(0), this->learning_rate);
                std::transform (pred_ts.begin(), pred_ts.end(), tree_pred.begin(), pred_ts.begin(), std::plus<double>());
                metric_ts = this->metric.get(pred_ts, y_ts);
            }
            
            // Log
            if (this->verbose == 1) {
                std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric Test : " << std::setw(7) << metric_ts << " Residuals (mean) : " << std::setw(7) << average << std::endl;
            }
        }
    }

    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) {
        double* X = reinterpret_cast<double *>(np_X.get_data());  
        std::vector<double> results = this->_predict(X, np_X.shape(0));
        
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(results.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(results.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    boost::python::numpy::ndarray get_residuals() {
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(this->residuals.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(this->residuals.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
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
        .def("get_conf", &lltrees::get_conf);
}
