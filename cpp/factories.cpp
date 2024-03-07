#include "factories.hpp"
#include "conf.hpp"

Gbt* gbt_Factory() {
    if (conf_gbt.mode == "classification")
        return new classification();
    if (conf_gbt.mode == "regression")
        return new regression();
    return nullptr;
} 

std::unique_ptr<data> data_Factory() {
    if (conf_gbt.mode == "classification") 
        return std::make_unique<data_type<int>>();
    if (conf_gbt.mode == "regression") 
        return std::make_unique<data_type<double>>();
    return nullptr;
}


template<> std::shared_ptr<metrics<double>> metric_Factory(){
    if (conf_gbt.metric_name == "mae") 
        return std::make_shared<mae>();
    return nullptr;
}

template<> std::shared_ptr<metrics<int>> metric_Factory(){
    if (conf_gbt.metric_name == "accuracy") 
        return std::make_shared<accuracy>();
    return nullptr;
}


template<> std::shared_ptr<criterion<int>> criterion_Factory(){
    if (conf_gbt.criterion_name == "gini") 
        return std::make_shared<gini>();
    return nullptr;
}

template<> std::shared_ptr<criterion<double>> criterion_Factory(){
    if (conf_gbt.criterion_name == "variance") 
        return std::make_shared<variance>();
    if (conf_gbt.criterion_name == "absolute_error") 
        return std::make_shared<absolute_error>();
    return nullptr;
}
