#pragma once
#include <vector>
#include <memory>
#include <iostream>

class metrics{
public:
virtual double get(std::vector<double>& pred,const std::vector<double>& target);
virtual double get(std::vector<int>& pred,const std::vector<int>& target);
};

class mae: public metrics  {
virtual double get(std::vector<double>& pred,const std::vector<double>& target) override;
};

class accuracy: public metrics  {
virtual double get(std::vector<int>& pred,const std::vector<int>& target) override;
};

std::shared_ptr<metrics> metric_Factory();

// template<typename T> double _mse(const std::vector<double>& pred, const std::vector<T>& Y){
//     double error = 0;
//     for (long unsigned int i = 0; i < pred.size(); i++){
//         error = error + pow(pred[i] - Y[i], 2);
//     }
//     error = error / pred.size();
//     return error; 
// }

// template<typename T> double _accuracy(const std::vector<double>& pred, const std::vector<T>& Y){
//     double acc = 0;
//     for (long unsigned int i = 0; i < pred.size(); i++){
//         if (pred[i] == Y[i]) {
//             acc++;
//         }
//     }
//     acc = acc / pred.size();
//     return acc; 
// }

// public:
//     metrics() : metric_name("mae") {}
//     metrics(std::string metric_name) : metric_name(metric_name) {}

//     void set_name(std::string name) {
//         this->metric_name = name;
//     }

//     std::string get_name() {
//         return metric_name;
//     }

//     void print() {
//         std::cout << "Metrics available for decisison tree : mae, mse." << std::endl;
//     }

//     template<typename T> double get(const std::vector<double>& pred, const std::vector<T>& Y){
//         if (this->metric_name == "mae") {
//             return this->_mae<T>(pred, Y);
//         }
//         else if (this->metric_name == "mse") {
//             return this->_mse<T>(pred, Y);
//         }
//         else if (this->metric_name == "accuracy") {
//             return this->_accuracy<T>(pred, Y);
//         }
//         else {
//             assert(true && "metric_name is not defined");
//             return 0; // to avoid the warning during compile
//         }
//     }


// };
