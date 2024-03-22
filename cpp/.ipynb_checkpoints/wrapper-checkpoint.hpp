#pragma once
#include <boost/python/numpy.hpp>
#include <iostream>
#include <numeric>

class data 
{
public:
~data() = default;
double* x = NULL;
int size_x, number_of_rows, number_of_cols;
std::vector<int> index;

// Data
void set_x(const boost::python::numpy::ndarray & np_x);
std::vector<double> get_column(const int index_col);
std::vector<double> get_column(const int index_col, const std::vector<int>& index) const;

template<class T>
T* get_y<T>() const;

// Y
virtual void set_y(const boost::python::numpy::ndarray & np_y) = 0;
virtual void set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) = 0;
virtual boost::python::numpy::ndarray get_prediction() =0;
};

template<class T>
class data_type : public data 
{
public :
~data_type() ;
T* y = NULL;
std::vector<T> pred;

// T* get_y() override;
void set_y(const boost::python::numpy::ndarray & np_y) override;
void set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) override ;
boost::python::numpy::ndarray get_prediction() override;
};
