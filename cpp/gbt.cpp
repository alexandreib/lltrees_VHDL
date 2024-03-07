#include "factories.hpp"
#include "conf.hpp"

void Gbt::print_epoch_log(int& epoch, double & metric_tr, double & metric_va, double residuals_average) {
    if (conf_gbt.verbose == 1) {
        std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric va : " << std::setw(7) << metric_va << " Residuals (mean) : " << std::setw(7) << residuals_average << std::endl;
    }
}

void classification::fit(const data& base_tr, const data& base_va) {
    std::cout<< "Gbt_classification fit" << std::endl;
}
    
void regression::fit(const data& tr, const data& va) {
    const data_type<double>& type_tr = static_cast <const data_type<double>&> (tr);
    const data_type<double>& type_va = static_cast <const data_type<double>&> (va);
    
    std::shared_ptr<criterion<double>> crit = criterion_Factory<double>(); 
    std::shared_ptr<metrics<double>> metr = metric_Factory<double>(); 
    std::vector<double> pred_tr(tr.number_of_rows, 0.0);
    std::vector<double> pred_va(va.number_of_rows, 0.0);
    double*residuals  = type_tr.y;// copy
    
    std::cout<<pred_tr[0]<< " " <<pred_va[0] << " " << residuals[0]<< " "<< pred_tr.size()<< " " << pred_va.size() <<" " << tr.number_of_rows <<std::endl;
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++){
        tree<double>* my_tree = new tree<double>(crit);

        my_tree->fit(tr, residuals);
        this->trees.push_back(my_tree);
        
        this->pred_and_add(tr, *my_tree, pred_tr);
        double metric_tr = metr->get(pred_tr, type_tr.y);

        this->pred_and_add(va, *my_tree, pred_va);
        double metric_va = metr->get(pred_va, type_va.y);
        
        std::cout<<pred_tr[0]<< " " <<pred_va[0] << " " << residuals[0]<< " "<< pred_tr.size()<< " " << pred_va.size() <<" " << tr.number_of_rows <<std::endl;
        
        double mean_residuals = 0;
        for (int index = 0; index < tr.number_of_rows; index ++){
            residuals[index] = residuals[index] - pred_tr[index];
            mean_residuals = mean_residuals + residuals[index];
        }
        this->residuals_average.push_back(mean_residuals);
        
        std::cout<<pred_tr[0]<< " " <<pred_va[0] << " " << residuals[0]<< " "<< pred_tr.size()<< " " << pred_va.size() <<" " << tr.number_of_rows <<std::endl;
        
        this->print_epoch_log(epoch,metric_tr, metric_va, mean_residuals );
        // my_tree->print();
    }
}

void regression::pred_and_add(const data& d, tree<double>& tree, std::vector<double>& results) {
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++){
        results[index_row] = results[index_row]  + conf_gbt.learning_rate *  tree.predict_row(&d.x[index_row * d.number_of_cols]);
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
        results[index_row] = results[index_row]  + conf_gbt.learning_rate *  tree.predict_row(&X.x[index_row * X.number_of_cols]);
    }
}
    
std::vector<double> classification::predict(const data& X) {
    std::vector<double> pred(X.number_of_rows);
    for (auto &tree : this->trees){        
        this->pred_and_add(X, *tree, pred);
    }
    return pred;
}
