#include <iostream>
#include "factories.hpp"
#include "conf.hpp"

class lltrees 
{
private :
    base_factory * factory = NULL;
    base_gbt * gbt = NULL;

public:
    lltrees() 
    {
    }

    ~lltrees() 
    {
        delete this->gbt;
    }

    void set_conf(boost::python::dict d) 
    {
        auto items = d.attr("items")();
        for (auto it = boost::python::stl_input_iterator<boost::python::tuple>(items); 
                it != boost::python::stl_input_iterator<boost::python::tuple>();
                ++it) 
        {
            boost::python::tuple kv = *it;
            std::string key = boost::python::extract<std::string>(boost::python::str(kv[0]));
            if (key == "mode") { conf::mode = boost::python::extract<std::string>(kv[1]);}
            if (key == "epochs") { conf::gbt::epochs = boost::python::extract<int>(kv[1]);}
            if (key == "learning_rate") { conf::gbt::learning_rate = boost::python::extract<double>(kv[1]); }
            if (key == "metric") { conf::gbt::metric_name = boost::python::extract<std::string>(kv[1]); }
            if (key == "criterion") { conf::tree::criterion_name = boost::python::extract<std::string>(kv[1]); }
            if (key == "max_depth") { conf::tree::max_depth = boost::python::extract<int>(kv[1]); }
            if (key == "min_leaf_size") { conf::tree::min_leaf_size = boost::python::extract<int>(kv[1]); }
            if (key == "verbose") { conf::verbose = boost::python::extract<int>(kv[1]);}
        }
        factory = base_factory::get_instance();        
    }

    void get_conf() 
    {
        std::cout << "-----------------------------------------" << std::endl;
        std::cout << std::left << std::setw(20) << "mode : " << conf::mode << std::endl;
        std::cout << std::setw(20) << "epochs : " << conf::gbt::epochs << std::endl;
        std::cout << std::setw(20) << "learning_rate : " << conf::gbt::learning_rate << std::endl;
        std::cout << std::setw(20) << "metric : " << conf::gbt::metric_name << std::endl;
        std::cout << std::setw(20) << "criterion : " << conf::tree::criterion_name << std::endl;
        std::cout << std::setw(20) << "max_depth : " << conf::tree::max_depth << std::endl;
        std::cout << std::setw(20) << "min_leaf_size : " << conf::tree::min_leaf_size << std::endl;
        std::cout << std::setw(20) << "verbose : " << std::boolalpha << conf::verbose << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    }


    void fit(const boost::python::numpy::ndarray & x_tr,  const boost::python::numpy::ndarray & y_tr, 
             const boost::python::numpy::ndarray & x_va = boost::python::numpy::array(boost::python::list()),
             const boost::python::numpy::ndarray & y_va = boost::python::numpy::array(boost::python::list())) 
    {
        std::string dt =  boost::python::extract<std::string>(boost::python::str(y_tr.get_dtype()));
        // need to add assert to check dt and mode
        std::cout << "Type of Training Data : "<< dt << std::endl;
        std::cout << "Configuration mode : "<< conf::mode << std::endl;
        
        this->gbt = factory->Gbt();
        std::unique_ptr<XY> tr = factory->Data();
        tr->set_xy(x_tr, y_tr);
        
        if (x_va.get_shape()[0] !=0) 
        {
            std::unique_ptr<XY> va = factory->Data();
            va->set_xy(x_va, y_va);
            this->gbt->fit(*tr, *va);
        } 
        else 
        {
            std::cout << "No Validate Data, will use Training Data." << std::endl;
            this->gbt->fit(*tr, *tr);
        }
    }

    boost::python::numpy::ndarray predict(boost::python::numpy::ndarray const & np_X) 
    {
        std::unique_ptr<XY> x = factory->Data();
        x->set_x(np_X);
        this->gbt->predict(*x);
        return x->get_pred();
    }

    boost::python::numpy::ndarray predict_proba(boost::python::numpy::ndarray const & np_X) 
    {
        std::unique_ptr<XY> x = factory->Data();
        x->set_x(np_X);
        this->gbt->predict_proba(*x);
        return x->get_proba();
    }

    boost::python::numpy::ndarray get_residuals() 
    {
        boost::python::numpy::ndarray result = boost::python::numpy::from_data(this->gbt->residuals_average.data(),  
                                boost::python::numpy::dtype::get_builtin<double>(),  
                                boost::python::make_tuple(this->gbt->residuals_average.size()), 
                                boost::python::make_tuple(sizeof(double)), 
                                boost::python::object());  
        return result.copy();
    }

    void save() 
    {
        std::cout<<"Save."<<std::endl;
        this->gbt->save();       
    }

    void load()
    {
        std::cout<<"Load."<<std::endl;
        if (this->gbt != nullptr) 
        {
            delete this->gbt;
        }
        this->gbt = this->factory->Gbt();
        this->gbt->load();       
    }

    void print() 
    {
        std::cout<<"Print."<<std::endl;
        this->gbt->print();       
    }
};

BOOST_PYTHON_MODULE(lltrees) 
{
    Py_Initialize();
    boost::python::numpy::initialize();

    boost::python::class_<lltrees>("lltree", boost::python::init<>())
        .def("fit", &lltrees::fit, (boost::python::arg("x_tr"), 
                                    boost::python::arg("y_tr"), 
                                    boost::python::arg("x_va") = boost::python::numpy::array(boost::python::list()),
                                    boost::python::arg("y_va") = boost::python::numpy::array(boost::python::list())))
        .def("predict", &lltrees::predict)
        .def("predict_proba", &lltrees::predict_proba)
        .def("get_residuals", &lltrees::get_residuals)
        .def("set_conf", &lltrees::set_conf)
        .def("get_conf", &lltrees::get_conf)
        .def("save", &lltrees::save)
        .def("load", &lltrees::load)
        .def("print", &lltrees::print)
        ;
}
