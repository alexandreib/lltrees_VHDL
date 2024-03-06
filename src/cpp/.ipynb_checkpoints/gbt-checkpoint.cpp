#include "conf.hpp"
#include "factories.hpp"

void Gbt::print_epoch_log(int& epoch, double & metric_tr, double & metric_va, double residuals_average) {
    if (conf_global.verbose == 1) {
        std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric va : " << std::setw(7) << metric_va << " Residuals (mean) : " << std::setw(7) << residuals_average << std::endl;
    }
}

void classification::fit(const data& base_tr, const data& base_va) {
    std::cout<< "Gbt_classification fit" << std::endl;
}
    
void regression::fit(const data& tr, const data& va) {
    const data_type<double>& type_tr = static_cast <const data_type<double>&> (tr);
    const data_type<double>& type_va = static_cast <const data_type<double>&> (va);
    
    
    std::shared_ptr<criterion> crit = criterion_Factory(); 
    std::shared_ptr<metrics> metr = metric_Factory(); 
    std::vector<double> pred_tr(tr.number_of_rows);
    std::vector<double> pred_va(tr.number_of_rows);
    std::vector<double> residuals = type_tr.y;// copy
    for (int epoch = 1; epoch < conf_global.epochs + 1; epoch++){
        tree<double>* my_tree = new tree<double>(crit);
        my_tree->fit(tr, residuals);
        this->trees.push_back(my_tree);
        
        this->pred_and_add(tr, *my_tree, pred_tr);
        double residuals_average = 0;
        for (int index = 0; index < tr.number_of_rows; index ++){
            residuals[index] = residuals[index] - pred_tr[index];
            residuals_average = residuals_average + residuals[index];
        }
        this->residuals_average.push_back(residuals_average);
        double metric_tr = metr->get(pred_tr, type_tr.y);

        this->pred_and_add(va, *my_tree, pred_va);
        double metric_va = metr->get(pred_va, type_va.y);
        this->print_epoch_log(epoch,metric_tr, metric_va, residuals_average );
        
    }
}

void regression::pred_and_add(const data& d, tree<double>& tree, std::vector<double>& results) {
    for ( int index_row = 0; index_row < d.number_of_rows; index_row ++){
        results[index_row] = results[index_row]  + conf_global.learning_rate *  tree.predict_row(&d.x[index_row * d.number_of_cols]);
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
        results[index_row] = results[index_row]  + conf_global.learning_rate *  tree.predict_row(&X.x[index_row * X.number_of_cols]);
    }
}
    
std::vector<double> classification::predict(const data& X) {
    std::vector<double> pred(X.number_of_rows);
    for (auto &tree : this->trees){        
        this->pred_and_add(X, *tree, pred);
    }
    return pred;
}
