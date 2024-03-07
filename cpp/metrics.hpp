#pragma once
#include <vector>
#include <memory>
#include <iostream>

template<class T> class metrics{
public:
virtual double get(std::vector<T>& pred,const std::vector<T>& target);
};

class mae: public metrics<double>  {
double get(std::vector<double>& pred,const std::vector<double>& target) override;
};

class accuracy: public metrics<int>   {
double get(std::vector<int>& pred,const std::vector<int>& target) override;
};


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

