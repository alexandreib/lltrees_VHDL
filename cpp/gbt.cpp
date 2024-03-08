#include "factories.hpp"
#include "conf.hpp"

template<class T>
void Gbt<T>::print_epoch_log(int& epoch, double & metric_tr, double & metric_va, double& residuals_average) {
    if (conf_gbt.verbose == 1) {
        std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric va : " << std::setw(7) << metric_va << " Residuals (mean) : " << std::setw(7) << residuals_average << std::endl;
    }
}


template<>
void Gbt<int>::fit(const data& base_tr, const data& base_va) {
    std::cout<< "Gbt_classification fit" << std::endl;
}

template<>
void Gbt<double>::fit(const data& tr, const data& va) {
    
    const data_type<double>& type_tr = static_cast <const data_type<double>&> (tr);
    const data_type<double>& type_va = static_cast <const data_type<double>&> (va);
    std::shared_ptr<criterion<double>> crit = criterion_Factory<double>(); 
    std::shared_ptr<metrics<double>> metr = metric_Factory<double>(); 

    std::vector<double> pred_tr_final(tr.number_of_rows, 0.0);
    std::vector<double> pred_va_final(va.number_of_rows, 0.0);
    
    std::vector<double> tr_residuals;
    tr_residuals.insert(tr_residuals.end(), type_tr.y, type_tr.y+tr.number_of_rows); 
    
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++){        
        tree<double>* my_tree = new tree<double>(crit);
        
        my_tree->fit(tr, tr_residuals);
        this->trees.push_back(my_tree);
        
        std::vector<double> pred_tr = my_tree->predict(tr);
        std::vector<double> pred_va = my_tree->predict(va);
                
        double mean_residuals = 0;
        for (int index = 0; index < tr.number_of_rows; index ++){
            tr_residuals[index] -= conf_gbt.learning_rate * pred_tr[index];
            pred_tr_final[index] += conf_gbt.learning_rate * pred_tr[index];
            mean_residuals += tr_residuals[index];
        }
        
        for (int index = 0; index < va.number_of_rows; index ++){
            pred_va_final[index] += conf_gbt.learning_rate * pred_va[index];
        }
        
        double metric_tr = metr->get(pred_tr_final, type_tr.y);
        double metric_va = metr->get(pred_va_final, type_va.y);
        this->residuals_average.push_back(mean_residuals);        
        this->print_epoch_log(epoch,metric_tr, metric_va, mean_residuals );
    }
}

template<>
void Gbt<double>::pred_and_add(const data& d, const tree<double>& tree, std::vector<double>& pred) {
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++){
        pred[index_row] += conf_gbt.learning_rate * tree.predict_row(d.x + index_row * d.number_of_cols);
    }
}

template<>
void Gbt<double>::predict(data& d) {
    std::vector<double> preds(d.number_of_rows);
    for(auto const &tree : trees) {
        this->pred_and_add(d, *tree, preds);            
    }
    d.pred = preds;
}

template<>
void Gbt<int>::predict(data& d) {
    std::vector<int> preds(d.number_of_rows);
    // d.pred = preds;
}
