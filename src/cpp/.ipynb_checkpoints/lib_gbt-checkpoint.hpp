#ifndef __GBT_H_INCLUDED__
#define __GBT_H_INCLUDED__
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <iomanip>

#include "lltree.hpp"
#include "metrics.hpp"
#include "criterion.hpp"

class lib_gbt {
public:
    lib_gbt() {}
    lib_gbt(const lib_gbt& other) = default;
    ~lib_gbt() {} 

    int epochs, tree_max_depth, tree_min_leaf_size, verbose;
    double learning_rate;
    std::string algo_name, tree_mode;
    metrics metric;
    criterion tree_criterion;
    std::vector<double> residuals; 
    std::vector<lltree*> v_lltrees; //(5);
    int number_of_cols;

    template<typename T>void fit(const std::vector<double>& x_tr, const std::vector<T>& y_tr, const int & number_of_cols, const std::vector<double>& x_ts, const std::vector<T>& y_ts) {
        int number_of_rows = x_tr.size() / number_of_cols;
        this->number_of_cols = number_of_cols;
        double metric_tr = 0.0;
        double metric_ts = 0.0;
        std::vector<double> pred_tr(number_of_rows);
        std::vector<double> pred_ts(y_ts.size());
        std::vector<double> residuals (y_tr.begin(), y_tr.end()); //= y_tr;
        std::cout<< "y_tr " << y_tr[0]  << std::endl;
        std::cout<< "number_of_rows " << number_of_rows  << std::endl;
        std::cout<< "number_of_cols " << this->number_of_cols   << std::endl;

        for (int epoch = 1; epoch < this->epochs + 1; epoch++){
            static lltree* tree = new lltree(this->tree_max_depth, 
                        this->tree_min_leaf_size,
                        &this->tree_criterion,
                        this->tree_mode);
            // tree->print_node_0();
            // tree->node_0->print();
            // this->v_lltrees[0]->print_node_0();
            
            tree->fit(x_tr, residuals, this->number_of_cols);
            this->v_lltrees.push_back(tree);
            
            // tree->node_0->print();
            // tree->print_node_0();
            // this->v_lltrees[0]->print();
            // this->v_lltrees[0]->print_node_0();
            // this->v_lltrees[0]->node_0->print();
            
            this->pred_and_add(x_tr, tree, pred_tr);
            std::cout <<"residuals" << residuals[0] << std::endl;
            std::cout <<"pred_tr" << pred_tr[0] << std::endl;
            
            // tree->node_0->print();
            // this->v_lltrees[0]->node_0->print();

            double average = 0;
            for (int index = 0; index < number_of_rows; index ++){
                residuals[index] = residuals[index] - pred_tr[index];
                average = average + residuals[index];
            }
            metric_tr = this->metric.get(residuals, y_tr);
            std::cout <<"gbt fit " << average << "  " << average / number_of_rows << std::endl;
            this->residuals.push_back(average / number_of_rows);
            std::cout <<"residuals" << residuals[0] << std::endl;
            // Predict TS and Metric        
            if (y_ts.size() != 0 ) {
                this->pred_and_add(x_ts, tree, pred_ts);
                metric_ts = this->metric.get(pred_ts, y_ts);
            }
       
            // Log
            if (this->verbose == 1) {
                std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric Test : " << std::setw(7) << metric_ts << " Residuals (mean) : " << std::setw(7) << average << std::endl;
            }
        }
        // std::cout << "AFTER" << std::endl;
        // tree->node_0->print();
        // this->v_lltrees[0]->node_0->print();
        // this->v_lltrees[0]->print_node_0();
        // std::vector<double>this->predict(x_tr);
        
        // this->v_lltrees[0]->print();
    }

    void pred_and_add(const std::vector<double>& X, lltree* tree, std::vector<double>& results) {
        for (long unsigned int index_row = 0; index_row < X.size() / this->number_of_cols; index_row ++){
            results[index_row] = results[index_row]  + this->learning_rate *  tree->predict_row(&X[index_row * this->number_of_cols]);
        }
    }
        
    std::vector<double> predict(std::vector<double>& X) {
        std::vector<double> pred(X.size()/this->number_of_cols);
        // this->v_lltrees[0]->node_0->print();
        for (auto &tree : this->v_lltrees){
            
            // tree->print_node_0();
            // tree->print();l
            this->pred_and_add(X, tree, pred);
        }
        std::cout <<"gbt predict " << pred[0] << std::endl;
        return pred;
    }
};

#endif // __GBT_H_INCLUDED__