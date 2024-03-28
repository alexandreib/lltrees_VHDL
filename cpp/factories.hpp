#pragma once
#include "gbt.hpp"
#include "tree.hpp"
#include "node.hpp"
#include "criterion.hpp"
#include "metrics.hpp"
#include "wrapper.hpp"

class base_factory
{
private:
typedef std::map< std::string, base_factory* > factory_map;
static factory_map s_factory_map;

protected:
void Register( std::string, base_factory * );

public :
// // Singletons are not be cloneable.
// factory(factory &other) = delete;
// // Singletons are not be assignable.
// void operator=(const factory &) = delete;
static base_factory * get_instance();

virtual base_gbt * Gbt() = 0;

virtual std::shared_ptr<base_criterion> Criterion() = 0;
virtual std::shared_ptr<base_metrics> Metric() = 0;
virtual std::unique_ptr<XY> Data() = 0;
};

class regression_factory : public base_factory 
{
private:
regression_factory();
regression_factory( const regression_factory& );
static regression_factory s_regression_factory;

public:
base_gbt * Gbt();
std::shared_ptr<base_criterion> Criterion();
std::shared_ptr<base_metrics> Metric();
std::unique_ptr<XY> Data();
};

class classification_factory : public base_factory 
{
private:
classification_factory();
classification_factory( const classification_factory& );
static classification_factory s_classification_factory;

public:
base_gbt * Gbt();
std::shared_ptr<base_criterion> Criterion();
std::shared_ptr<base_metrics> Metric();
std::unique_ptr<XY> Data();
};
