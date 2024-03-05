#pragma once
#include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <tuple>

class data {
public:
std::vector<double> x;
int size_x, number_of_rows, number_of_cols;
std::vector<int> index;
void set_x(const boost::python::numpy::ndarray & np_x);
std::vector<double> get_column(const int index_columns, const std::vector<int>& index) const;
void create_index();
virtual void set_y(const boost::python::numpy::ndarray & np_y) = 0;
virtual void set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) = 0;
};

template<class T> class data_type : public data {
public :
std::vector<T> y;
std::vector<T> prediction;
virtual void set_y(const boost::python::numpy::ndarray & np_y) override;
virtual void set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) override ;
};

std::unique_ptr<data> data_Factory(const std::string& type);