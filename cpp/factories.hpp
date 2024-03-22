#pragma once
#include "gbt.hpp"
#include "metrics.hpp"
#include "wrapper.hpp"
#include "criterion.hpp"
// #include "conf.hpp"

base_gbt* gbt_Factory();
std::unique_ptr<XY> data_Factory();

template<typename T> std::shared_ptr<metrics<T>> metric_Factory();
template<> std::shared_ptr<metrics<int>> metric_Factory();
template<> std::shared_ptr<metrics<double>> metric_Factory();

template<typename T> std::shared_ptr<criterion<T>> criterion_Factory();
template<> std::shared_ptr<criterion<int>> criterion_Factory();
template<> std::shared_ptr<criterion<double>> criterion_Factory();

