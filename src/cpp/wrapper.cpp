#include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <tuple>
#include "wrapper.hpp"

std::unique_ptr<data> data_Factory(const std::string& type)
{
    if (type == "classification") 
        return std::make_unique<data_type<int>>();
    if (type == "regression") 
        return std::make_unique<data_type<double>>();
    return nullptr;
}

void data::set_x(const boost::python::numpy::ndarray & np_x){
    this->number_of_rows = np_x.shape(0);
    this->number_of_cols = np_x.shape(1);
    this->size_x = np_x.shape(0) * np_x.shape(1);
    double* x_ptr = reinterpret_cast<double *>(np_x.get_data()); 
    std::vector<double> t(x_ptr, x_ptr + this->size_x); 
    this->x = std::move(t); 
    std::cout<< "x shape : " << this->number_of_rows << " "<< this->number_of_cols << std::endl;
}

std::vector<double> data::get_column(const int index_columns, const std::vector<int>& index) const {
    std::vector<double> columns;
    for(auto & index_row : index) {
        columns.push_back(this->x[index_row * this->number_of_cols + index_columns]); 
    }
    return columns;
}

void data::create_index() {
    std::vector<int> index(this->number_of_rows);
    std::iota(index.begin(), index.end(), 0);
    this->index = index;
    std::cout<< "create_index" << std::endl;
}

template <class T> void data_type<T>::set_y(const boost::python::numpy::ndarray & np_y)  {
    T* y_ptr = reinterpret_cast<T*>(np_y.get_data()); 
    std::vector<T> t(y_ptr, y_ptr + this->number_of_rows);
    this->y = std::move(t); 
    this->prediction.resize(this->number_of_rows);
    std::cout<< "y shape : " << this->number_of_rows << " 1" << std::endl;
}
                                                             
template <class T> void data_type<T>::set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) {
    // assert((std::is_same_v<np_x.shape(0), np_y.shape(0)>) && "sizes do not match");
    this->set_x(np_x);
    this->set_y(np_y);
    // std::cout<< "set_xy" << std::endl;
}


template class data_type<int>;  // Explicit instantiation
template class data_type<double>;  // Explicit instantiation
