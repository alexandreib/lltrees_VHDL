#include "factories.hpp"
#include "conf.hpp"

regression_factory::regression_factory()
{
    Register( "regression", this );
}

classification_factory::classification_factory()
{
    Register( "classification", this );
}

void base_factory::Register( std::string mode, base_factory * factory )
{
    s_factory_map.insert( factory_map::value_type( mode, factory ) );
}

base_factory * base_factory::get_instance()
{
    return s_factory_map.find(conf::mode)->second;
}

base_gbt * regression_factory::Gbt() 
{
    return new regression();
} 

base_gbt * classification_factory::Gbt() 
{
    return new classification();
} 

std::shared_ptr<base_criterion> regression_factory::Criterion() 
{
    if (conf::tree::criterion_name == "variance") 
        return std::make_shared<variance>();
    if (conf::tree::criterion_name == "absolute_error") 
        return std::make_shared<absolute_error>();
    return nullptr;
}

std::shared_ptr<base_criterion> classification_factory::Criterion() 
{
    if (conf::tree::criterion_name == "gini") 
        return std::make_shared<gini>();   
    if (conf::tree::criterion_name == "entropy") 
        return std::make_shared<entropy>();
    return nullptr;
}

std::shared_ptr<base_metrics> regression_factory::Metric() 
{
    if (conf::gbt::metric_name == "mae") 
        return std::make_shared<mae>();
    return nullptr;
}

std::shared_ptr<base_metrics> classification_factory::Metric() 
{
    if (conf::gbt::metric_name == "accuracy") 
        return std::make_shared<accuracy>();
    return nullptr;
}

std::unique_ptr<XY> regression_factory::Data() 
{
    return std::make_unique<Y<double>>();
}

std::unique_ptr<XY> classification_factory::Data() 
{
    return std::make_unique<Y<int>>();
}

base_factory::factory_map base_factory::s_factory_map;
regression_factory regression_factory::s_regression_factory;
classification_factory classification_factory::s_classification_factory;
