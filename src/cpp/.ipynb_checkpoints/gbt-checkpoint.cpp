#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <iomanip>
#include "conf.hpp"
#include "gbt.hpp"
#include "metrics.hpp"

Gbt* gbt_Factory(const std::string& type)
{
    if (type == "classification")
        return new classification();
    if (type == "regression")
        return new regression();
    return nullptr;
} 
void Gbt::fit(const data& base_tr, const data& base_va) {
}

void classification::fit(const data& base_tr, const data& base_va) {
    std::cout<< "Gbt_classification fit" << std::endl;
    //const data_type<int>& tr = static_cast <const data_type<int>&> (base_tr);
}
    
void regression::fit(const data& tr, const data& base_va) {
    const data_type<double>& type_tr = static_cast <const data_type<double>&> (tr);
    this->residuals = type_tr.y;// copy
    
    std::shared_ptr<criterion> crit = criterion_Factory(conf_global.criterion_name); 
    std::shared_ptr<metric> metr = metric_Factory(conf_global.metric_name); 
    std::vector<double> pred_tr(tr.number_of_rows);
    
    for (int epoch = 1; epoch < this->epochs + 1; epoch++){
        tree<double>* my_tree = new tree<double>(crit);
        my_tree->fit(tr, residuals);
        this->trees.push_back(my_tree);
        
        this->pred_and_add(tr, *my_tree, pred_tr);
        double average = 0;
        for (int index = 0; index < tr.number_of_rows; index ++){
            this->residuals[index] = this->residuals[index] - pred_tr[index];
            average = average + residuals[index];
        }
        double metric_tr = metr.get(pred_tr, type_tr.y);
        std::cout <<metric_tr<<std::endl;
        
    }
}

        // std::cout <<"gbt fit " << average << "  " << average / number_of_rows << std::endl;
        // this->residuals.push_back(average / number_of_rows);
        // std::cout <<"residuals" << residuals[0] << std::endl;
        // // Predict TS and Metric        
        // if (y_ts.size() != 0 ) {
        //     this->pred_and_add(x_ts, my_tree, pred_ts);
        //     metric_ts = this->metric.get(pred_ts, y_ts);
        // }
   
        // // Log
        // if (this->verbose == 1) {
        //     std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric Test : " << std::setw(7) << metric_ts << " Residuals (mean) : " << std::setw(7) << average << std::endl;
        // }
    // }
    // std::cout << "AFTER" << std::endl;
    // my_tree->node_0->print();
    // this->trees[0]->node_0->print();
    // this->trees[0]->print_node_0();
    // std::vector<double>this->predict(x_tr);
    
    // this->trees[0]->print();
// }

void regression::pred_and_add(const data& d, tree<double>& tree, std::vector<double>& results) {
    for ( int index_row = 0; index_row < d.number_of_rows; index_row ++){
        results[index_row] = results[index_row]  + this->learning_rate *  tree.predict_row(&d.x[index_row * d.number_of_cols]);
    }
}
    
std::vector<double> regression::predict(const data& d) {
    std::vector<double> pred(d.number_of_rows);
    for (auto &tree : this->trees){        
        this->pred_and_add(d, *tree, pred);
    }
    return pred;
}


void classification::pred_and_add(const data& X, tree<int>& tree, std::vector<double>& results) {
    for ( int index_row = 0; index_row < X.number_of_rows; index_row ++){
        results[index_row] = results[index_row]  + this->learning_rate *  tree.predict_row(&X.x[index_row * X.number_of_cols]);
    }
}
    
std::vector<double> classification::predict(const data& X) {
    std::vector<double> pred(X.number_of_rows);
    for (auto &tree : this->trees){        
        this->pred_and_add(X, *tree, pred);
    }
    return pred;
}
