#include "factories.hpp"

Gbt* gbt_Factory() {
    if (conf_global.mode == "classification")
        return new classification();
    if (conf_global.mode == "regression")
        return new regression();
    return nullptr;
} 

std::unique_ptr<data> data_Factory() {
    if (conf_global.mode == "classification") 
        return std::make_unique<data_type<int>>();
    if (conf_global.mode == "regression") 
        return std::make_unique<data_type<double>>();
    return nullptr;
}

std::shared_ptr<metrics> metric_Factory() {
    if (conf_global.metric_name == "mae") 
        return std::make_shared<mae>();
    return nullptr;
}

std::shared_ptr<criterion> criterion_Factory()
{
    if (conf_global.criterion_name == "variance") 
        return std::make_shared<variance>();
    if (conf_global.criterion_name == "absolute_error") 
        return std::make_shared<absolute_error>();
    if (conf_global.criterion_name == "gini") 
        return std::make_shared<gini>();
    return nullptr;
}