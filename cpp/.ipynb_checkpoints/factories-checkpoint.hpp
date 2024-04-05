#pragma once
#include "gbt.hpp"
#include "tree.hpp"
#include "node.hpp"
#include "criterion.hpp"
#include "metrics.hpp"
#include "wrapper.hpp"

class base_factory
{
public :
static std::unique_ptr<base_factory> get_instance();

virtual base_gbt * Gbt() = 0;
virtual std::shared_ptr<base_criterion> Criterion() = 0;
virtual std::shared_ptr<base_metrics> Metric() = 0;
virtual std::unique_ptr<XY> Data() = 0;
virtual std::unique_ptr<XY> Data(const boost::python::numpy::ndarray & np_x) = 0;
virtual std::unique_ptr<XY> Data(const boost::python::numpy::ndarray & np_x,const boost::python::numpy::ndarray & np_y) = 0;
};

class regression_factory : public base_factory 
{
public:
base_gbt * Gbt();
std::shared_ptr<base_criterion> Criterion();
std::shared_ptr<base_metrics> Metric();
std::unique_ptr<XY> Data();
std::unique_ptr<XY> Data(const boost::python::numpy::ndarray & np_x);
std::unique_ptr<XY> Data(const boost::python::numpy::ndarray & np_x,const boost::python::numpy::ndarray & np_y);
};

class classification_factory : public base_factory 
{
public:
base_gbt * Gbt();
std::shared_ptr<base_criterion> Criterion();
std::shared_ptr<base_metrics> Metric();
std::unique_ptr<XY> Data();
std::unique_ptr<XY> Data(const boost::python::numpy::ndarray & np_x);
std::unique_ptr<XY> Data(const boost::python::numpy::ndarray & np_x,const boost::python::numpy::ndarray & np_y);
};


