#include "wrapper.hpp"


template <class T> data_type<T>::~data_type(){
    delete this->x;
    delete this->y;
    std::cout<<"delete"<<std::endl;
}

void data::set_x(const boost::python::numpy::ndarray & np_x){
    this->number_of_rows = np_x.shape(0);
    this->number_of_cols = np_x.shape(1);
    this->size_x = np_x.shape(0) * np_x.shape(1);
    this->x = reinterpret_cast<double *>(np_x.get_data()); 
}

std::vector<double> data::get_column(const int index_col, const std::vector<int>& index) const {
    std::vector<double> columns;
    for(auto const &index_row : index) {
        columns.push_back(this->x[index_row * this->number_of_cols + index_col]); 
    }
    return columns;
}


template <class T> 
void data_type<T>::set_y(const boost::python::numpy::ndarray & np_y)  {
    this->y = reinterpret_cast<T *>(np_y.get_data()); 
}
                                                             
template <class T> 
void data_type<T>::set_xy(const boost::python::numpy::ndarray & np_x, const boost::python::numpy::ndarray & np_y) {
    // assert((std::is_same_v<np_x.shape(0), np_y.shape(0)>) && "sizes do not match");
    this->set_x(np_x);
    this->set_y(np_y);
}

template <class T> 
boost::python::numpy::ndarray data_type<T>::get_prediction() {
    return boost::python::numpy::from_data(this->pred.data(),  
            boost::python::numpy::dtype::get_builtin<T>(),  
            boost::python::make_tuple(this->pred.size()), 
            boost::python::make_tuple(sizeof(T)), 
            boost::python::object());  
}

template class data_type<int>;  // Explicit instantiation
template class data_type<double>;  // Explicit instantiation
