#include "wrapper.hpp"

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// XY ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template <class T> 
T* XY::get_y() const
{
    const Y<T>* subclass = static_cast<const Y<T>*>(this);
    return subclass->y;
}

template int* XY::get_y() const; // explicit instantiation.
template double* XY::get_y() const; // explicit instantiation.

template <class T> 
void XY::set_pred(std::vector<T> preds)
{
    Y<T>* subclass = static_cast<Y<T>*>(this);
    subclass->pred = preds;
}
template void XY::set_pred(std::vector<int>); // explicit instantiation.
template void XY::set_pred(std::vector<double>); // explicit instantiation.

boost::python::numpy::ndarray XY::get_proba()
{
    Y<double>* subclass = static_cast<Y<double>*>(this);
    auto * const data_ptr = subclass->pred.data();
    boost::python::tuple shape = boost::python::make_tuple(subclass->pred.size(), this->number_of_classes);
    boost::python::tuple stride = boost::python::make_tuple(sizeof(double) * this->number_of_classes, sizeof(double));
    boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<double>();
    return boost::python::numpy::from_data(data_ptr, dt, shape, stride, boost::python::object());
}

void XY::set_x(const boost::python::numpy::ndarray & np_x)
{
    this->number_of_rows = np_x.shape(0);
    this->number_of_cols = np_x.shape(1);
    this->size_x = np_x.shape(0) * np_x.shape(1);
    this->x = reinterpret_cast<double *>(np_x.get_data()); 
}

std::vector<double> XY::get_column(const int index_col)
{
    std::vector<int> index(this->number_of_rows);
    std::iota(index.begin(), index.end(), 0);
    this->index = index;
    return this->get_column(index_col, index);
}     

std::vector<double> XY::get_column(const int index_col, const std::vector<int>& index) const 
{
    std::vector<double> columns;
    for(auto const &index_row : index) {
        columns.push_back(this->x[index_row * this->number_of_cols + index_col]); 
    }
    return columns;
} 

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Y /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template <class T> 
Y<T>::~Y()
{
    delete this->x;
    delete this->y;
    std::cout<<"delete"<<std::endl;
}

template <class T> 
void Y<T>::set_y(const boost::python::numpy::ndarray & np_y)
{
    this->y = reinterpret_cast<T *>(np_y.get_data()); 
}

// template <> 
// void Y<int>::set_y(const boost::python::numpy::ndarray & np_y)
// {
//     int64_t * Y =reinterpret_cast<int64_t *>(np_y.get_data()); 
//     this->y = new int[this->number_of_rows];
//     for (auto i=0; i<this->number_of_rows; i++) this->y[i] = Y[i];
// }

template <class T> 
void Y<T>::set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y)
{
    // assert((std::is_same_v<np_x.shape(0), np_y.shape(0)>) && "sizes do not match");
    this->set_x(np_x);
    this->set_y(np_y);
}

template <class T> 
boost::python::numpy::ndarray Y<T>::get_pred()
{
    return boost::python::numpy::from_data(this->pred.data(),
            boost::python::numpy::dtype::get_builtin<T>(), 
            boost::python::make_tuple(this->pred.size()), 
            boost::python::make_tuple(sizeof(T)), 
            boost::python::object());
}

template class Y<int>;// Explicit instantiation
template class Y<double>;// Explicit instantiation
