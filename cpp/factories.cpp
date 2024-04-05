#include "factories.hpp"
#include "conf.hpp"


std::unique_ptr<base_factory> base_factory::get_instance()
{
    if (conf::mode.find("regression") != std::string::npos)
        return std::make_unique<regression_factory>();
    if (conf::mode.find("classification") != std::string::npos)
        return std::make_unique<classification_factory>();
    {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); }
}

base_gbt * regression_factory::Gbt() 
{
    return new regression();
} 

base_gbt * classification_factory::Gbt() 
{
    if (conf::mode == "classification")     
        return new classification();
    if (conf::mode == "classic_classification")     
        return new classic_classification();
    {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); }
} 

std::shared_ptr<base_criterion> regression_factory::Criterion() 
{
    if (conf::tree::criterion_name == "variance") 
        return std::make_shared<variance>();
    if (conf::tree::criterion_name == "absolute_error") 
        return std::make_shared<absolute_error>();
    {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); }
}

std::shared_ptr<base_criterion> classification_factory::Criterion() 
{
    if (conf::tree::criterion_name == "gini") 
        return std::make_shared<gini>();   
    if (conf::tree::criterion_name == "entropy") 
        return std::make_shared<entropy>();
    {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); }
}

std::shared_ptr<base_metrics> regression_factory::Metric() 
{
    if (conf::gbt::metric_name == "mae") 
        return std::make_shared<mae>();
    {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); }
}

std::shared_ptr<base_metrics> classification_factory::Metric() 
{
    if (conf::gbt::metric_name == "accuracy") 
        return std::make_shared<accuracy>();
    {std::cout <<  __PRETTY_FUNCTION__ << std::endl; __builtin_unreachable(); }
}

std::unique_ptr<XY> regression_factory::Data(const boost::python::numpy::ndarray & np_x) 
{
    return std::make_unique<Y<double>>(np_x);
}

std::unique_ptr<XY> classification_factory::Data(const boost::python::numpy::ndarray & np_x) 
{
    return std::make_unique<Y<int>>(np_x);
}
std::unique_ptr<XY> regression_factory::Data(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) 
{
    return std::make_unique<Y<double>>(np_x, np_y);
}

std::unique_ptr<XY> classification_factory::Data(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) 
{
    return std::make_unique<Y<int>>(np_x, np_y);
}

std::unique_ptr<XY> regression_factory::Data() 
{
    return std::make_unique<Y<double>>();
}

std::unique_ptr<XY> classification_factory::Data() 
{
    return std::make_unique<Y<int>>();
}
