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

// std::shared_ptr<metrics> metric_Factory();

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

