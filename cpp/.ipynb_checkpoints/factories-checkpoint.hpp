#pragma once
#include "gbt.hpp"
#include "wrapper.hpp"
#include "node.hpp"
#include "metrics.hpp"
#include "criterion.hpp"

base_gbt* gbt_Factory();
std::unique_ptr<data> data_Factory();

template<typename T> 
tree<T>* tree_Factory(std::shared_ptr<criterion<T>> criterion_tree);
template<> 
tree<int>* tree_Factory(std::shared_ptr<criterion<int>> criterion_tree);
template<> 
tree<double>*tree_Factory(std::shared_ptr<criterion<double>> criterion_tree);

template<typename T> 
node<T>* node_Factory(int level, int id_node, double impurity, std::vector<int> index) ;
template<> 
node<int>* node_Factory(int level, int id_node, double impurity, std::vector<int> index) ;
template<> 
node<double>*node_Factory(int level, int id_node, double impurity, std::vector<int> index) ;

template<typename T> 
std::shared_ptr<metrics<T>> metric_Factory();

template<> 
std::shared_ptr<metrics<int>> metric_Factory();

template<> 
std::shared_ptr<metrics<double>> metric_Factory();

template<typename T> 
std::shared_ptr<criterion<T>> criterion_Factory();

template<> 
std::shared_ptr<criterion<int>> criterion_Factory();

template<> 
std::shared_ptr<criterion<double>> criterion_Factory();

