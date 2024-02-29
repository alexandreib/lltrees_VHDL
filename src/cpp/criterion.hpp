#ifndef __CRITERIONE_H_INCLUDED__
#define __CRITERIONE_H_INCLUDED__

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <map>

// class criterion{
// private :
//     std::string criterion_name;
// public:
//     criterion();
//     void set_name(std::string name) {
//         this->criterion_name = name;
//     }

//     std::string get_name() {
//         return this->criterion_name;
//     }

//     virtual double get(const std::vector<double>& Y) {
//         double average = 0, sum = 0;
//         for(double value : Y){average = average + value;}
//         average = average / Y.size();
//         for(double value : Y) {sum += pow(average - value,2);} 
//         return sum/ Y.size();  
//     }
// };

// class dervariance : public criterion {
// public:
//     dervariance();
//     double get(const std::vector<double>& Y) override {
//         double average = 0, sum = 0;
//         for(double value : Y){average = average + value;}
//         average = average / Y.size();
//         for(double value : Y) {sum += pow(average - value,2);} 
//         return sum/ Y.size();  
//     }
// };

// class absolute_error : public criterion {
// public:
//     absolute_error();
//     double get(const std::vector<double>& Y) {
//         double average = 0, sum = 0;
//         for(double value : Y){average = average + value;}
//         average = average / Y.size();
//         for(double value : Y) {sum += std::abs(average - value);} 
//         return sum/ Y.size();  
//     }
// };

// template<typename T> criterion * createInstance() { return new T; }

// typedef std::map<std::string, criterion*(*)()> criterionmap_type;

// criterionmap_type criterionmap;
// criterionmap["int"] = &createInstance<int>;
// criterionmap["double"] = &createInstance<double>;

// typedef criterion* (*criterionFactory)();
// template<class T> criterion *newcriterion() {return new T();}

// std::map<std::string, criterionFactory> criterionMap;
// criterionMap["variance"] = &newcriterion<variance>;
// criterionMap["absolute_error"] = &newcriterion<absolute_error>;




class criterion{
private:
    std::string criterion_name;

    template<typename T> double _variance(const std::vector<T>& Y) {
        double average = 0, sum = 0;
        for(double value : Y){average = average + value;}
        average = average / Y.size();
        for(double value : Y) {sum += pow(average - value,2);} 
        // std::cout<<"variance " << sum << " " << Y[0] << std::endl;
        return sum/ Y.size();  
    }

    template<typename T> double _absolute_error(const std::vector<T>& Y) {
        double average = 0, sum = 0;
        for(double value : Y){average = average + value;}
        average = average / Y.size();
        for(double value : Y) {sum += std::abs(average - value);} 
        return sum/ Y.size();  
    }

    template<typename T> double _gini(const std::vector<T>& Y) {
        double G = 0;
        double p = 0;
        std::vector<T> col = Y;
        std::sort(col.begin(), col.end());
        col.erase(std::unique(col.begin(), col.end()), col.end());
        for (auto &y : col){
            p = std::count(Y.begin(), Y.end(), y);
            G += pow(p / Y.size(), 2);
            // p  = p / Y.size();
            // G += p * (1-p);
        }
        // return G;
        return 1 - G;
    }

    template<typename T> double _entropy(const std::vector<T>& Y) {
        double H = 0;
        double p = 0;
        std::vector<T> col = Y;
        std::sort(col.begin(), col.end());
        col.erase(std::unique(col.begin(), col.end()), col.end());
        for (auto &y : col){
            p = std::count(Y.begin(), Y.end(), y);
            H -= p*p; //np.log2(p)
        }
        return H;
    }

public:
    criterion() : criterion_name("variance") {}
    criterion(std::string name) : criterion_name(name) {}

    void set_name(std::string name) {
        this->criterion_name = name;
    }

    std::string get_name() {
        return this->criterion_name;
    }

    template<typename T> double get(const std::vector<T>& Y){
        if (this->criterion_name == "variance") { return this->_variance<T>(Y); }
        else if (this->criterion_name == "absolute_error") { return this->_absolute_error<T>(Y); }
        else if (this->criterion_name == "gini") { return this->_gini<T>(Y); }
        else if (this->criterion_name == "entropy") { return this->_entropy<T>(Y); }
        else {assert(true && "criterion name is not defined"); return 0; }
    }

    void print() {
        std::cout << "Critetion available for decisison tree : variance, absolute_error, gini, entropy." << std::endl;
    }
};

#endif // __CRITERIONE_H_INCLUDED__