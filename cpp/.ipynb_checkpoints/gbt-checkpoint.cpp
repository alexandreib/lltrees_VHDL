   
#include <bits/stdc++.h>
#include "factories.hpp"
#include "conf.hpp"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Logs / Prints ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<class T>
void Gbt<T>::print_epoch_log(int& epoch, double & metric_tr, double & metric_va, double& residuals_average) {
    if (conf_gbt.verbose == 1) {
        std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric va : " << std::setw(7) << metric_va << " Residuals (mean) : " << std::setw(7) << residuals_average << std::endl;
    }
}

template<class T>
void Gbt<T>::print() {
    for (long unsigned int i =0; i < this->trees.size(); i++){
        std::cout << "Tree : " << i << std::endl;
        this->trees[i]->printBT();
    }
}
//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Classification ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<>
void Gbt<int>::fit(const data& tr, const data& va) {
    std::cout<< "Gbt_classification fit" << std::endl;
    const data_type<int>& type_tr = static_cast <const data_type<int>&> (tr);
    const data_type<int>& type_va = static_cast <const data_type<int>&> (va);
    std::shared_ptr<criterion<int>> crit = criterion_Factory<int>(); 
    std::shared_ptr<metrics<int>> metr = metric_Factory<int>(); 

    std::vector<int> pred_tr_final(tr.number_of_rows, 0.0);
    std::vector<int> pred_va_final(va.number_of_rows, 0.0);
    
    std::vector<int> Y;
    Y.insert(Y.end(), type_tr.y, type_tr.y+tr.number_of_rows); 
    
    std::vector<double> weights(tr.number_of_rows, 1);/// (double) tr.number_of_rows );
    double sum_model_weights=0;
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++) {
        tree<int>* my_tree = tree_Factory(crit);
        
        my_tree->fit(tr,  Y, weights);
        my_tree->printBT();
        this->trees.push_back(my_tree);

        pred_tr_final = my_tree->predict(tr);
        pred_va_final = my_tree->predict(va);
        std::cout <<"tr :" << type_tr.y[0]<< " "<< type_tr.y[1]<< " "<< type_tr.y[2]<< " "<< type_tr.y[3]<< " "<< type_tr.y[4]<< std::endl;
        std::cout <<"pred_tr_final :" << pred_tr_final[0] << " " << pred_tr_final[1] << " " << pred_tr_final[2] << " " << pred_tr_final[3] << " " << pred_tr_final[4] << std::endl;
        bool all_zero = true;
        for(auto const &pred_temp : pred_tr_final) 
        {
            if(pred_temp == 1) {all_zero = false; break;}
        }
        if (all_zero)
            std::cout << "all zeros" <<std::endl;
        
        double model_weight = 0;
        for (long unsigned int i =0; i<pred_tr_final.size() ; i++) {
            // std::cout << pred_tr_final[i] << " ";
            if (pred_tr_final[i] != Y[i]) {
                weights[i] *= std::exp(conf_gbt.learning_rate);
            }
            else 
            {
                model_weight +=1;
            }
        }
        std::cout <<"weights[0]: "<<weights[0]<<" weights[1]: "<<weights[1]<<std::endl;
        this->model_weights.push_back(model_weight);
        
        sum_model_weights += model_weight;
        std ::cout << "sum_model_weights :" << sum_model_weights << std::endl;
        
        double metric_tr = metr->get(pred_tr_final, type_tr.y);
        double metric_va = metr->get(pred_va_final, type_va.y);
        
        this->print_epoch_log(epoch, metric_tr, metric_va, model_weight );
    }
    
    for (long unsigned int i = 1; i < this->model_weights.size() + 1; i++) {     
        this->model_weights[i] /= sum_model_weights;
    }
}

template<>
void Gbt<int>::predict(data& d) {
    
    std::vector<int> preds(d.number_of_rows, 0);
    std::vector<std::unordered_map<int, double>> aggregated_preds(d.number_of_rows);
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++) 
    {
        std::cout<<epoch<<std::endl;
        tree_classification& type_tree = static_cast <tree_classification&> (*trees[epoch]);
        std::vector<std::unordered_map<int, double>> tree_pred = type_tree.predict_proba(d);    
        for (int idx = 0; idx < d.number_of_rows; idx++) 
        {   
            for(auto const &classe_pred : tree_pred[idx]) 
            {
                if(aggregated_preds[idx].find(classe_pred.first) == aggregated_preds[idx].end())
                {
                    aggregated_preds[idx][classe_pred.first] += this->model_weights[epoch] * classe_pred.second;
                }
                else
                {
                    aggregated_preds[idx][classe_pred.first] = this->model_weights[epoch] * classe_pred.second;
                }
                if (idx == 0) {
                    
                    std::cout<<"aggregated_preds" << aggregated_preds[idx][0] << " " << std::endl;
                }
            }
        }
    }
    for (int idx = 0; idx < d.number_of_rows; idx++) 
    {   
        double _highest_value = 0;
        int _saved_key = 0;
        for(auto const &classe_pred : aggregated_preds[idx]) 
        {
            if (_highest_value < classe_pred.second) 
            {
                _saved_key = classe_pred.first;
            }
        }
        preds.push_back(_saved_key);
    }
    
    data_type<int>& type_d = static_cast <data_type<int>&> (d);
    type_d.pred = preds;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Regression /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<>
void Gbt<double>::pred_and_add(const data& d, const tree<double>& tree, std::vector<double>& pred) {
    for (int index_row = 0; index_row < d.number_of_rows; index_row ++){
        pred[index_row] += conf_gbt.learning_rate * tree.predict_row(d.x + index_row * d.number_of_cols);
    }
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
    
    std::vector<double> weights(tr.number_of_rows, 1);
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++){        
        tree<double>* my_tree = tree_Factory(crit);// new tree<double>(crit);
        
        my_tree->fit(tr, tr_residuals, weights);
        this->trees.push_back(my_tree);
        
        this->pred_and_add(va, *my_tree, pred_va_final);
        double mean_residuals = 0;
        for (int index = 0; index < tr.number_of_rows; index ++){
            double row_pred = conf_gbt.learning_rate * my_tree->predict_row(tr.x + index * tr.number_of_cols);
            tr_residuals[index] -= row_pred;
            pred_tr_final[index] += row_pred; 
            mean_residuals += row_pred;
        }
        double metric_tr = metr->get(pred_tr_final, type_tr.y);
        double metric_va = metr->get(pred_va_final, type_va.y);
        this->residuals_average.push_back(mean_residuals);        
        this->print_epoch_log(epoch,metric_tr, metric_va, mean_residuals );
    }
}

template<>
void Gbt<double>::predict(data& d) {
    std::vector<double> preds(d.number_of_rows);
    for(auto const &tree : trees) {
        this->pred_and_add(d, *tree, preds);            
    }
    
    data_type<double>& type_d = static_cast <data_type<double>&> (d);
    type_d.pred = preds;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Save / Load ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<class T>
void Gbt<T>::save() {    
    std::ofstream myfile("trees.txt");
    myfile << conf_gbt.verbose << ":"  << conf_gbt.mode << ":"  << conf_gbt.criterion_name << ":"  
          << conf_gbt.metric_name << ":"  << conf_gbt.epochs << ":"   << conf_gbt.learning_rate << ":" 
          << conf_gbt.number_of_threads  << "\n";
    myfile << conf_trees.max_depth << ":"  << conf_trees.min_leaf_size << "\n";
    for (long unsigned int i =0; i < this->trees.size(); i++){
        this->trees[i]->save(myfile);
        myfile << "\n";
    }
    
    myfile.close();
}

template<class T>
void Gbt<T>::load() {    
    std::ifstream myfile("trees.txt");
    std::string delimiter = ":";
    std::string line;
    std::getline(myfile, line);
    std::string token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf_gbt.verbose = std::stoi(token);

    conf_gbt.mode = line.substr(0, line.find(delimiter));
    line.erase(0, conf_gbt.mode.size() + delimiter.size());
    
    conf_gbt.criterion_name = line.substr(0, line.find(delimiter));
    line.erase(0, conf_gbt.criterion_name.size() + delimiter.size());
    
    conf_gbt.metric_name = line.substr(0, line.find(delimiter));
    line.erase(0, conf_gbt.metric_name.size() + delimiter.size());
    
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf_gbt.epochs = std::stoi(token);

    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf_gbt.learning_rate = std::stod(token);
    
    token = line.substr(0, line.find(delimiter));
    conf_gbt.number_of_threads = std::stoi(token);

    std::getline(myfile, line);
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf_trees.max_depth = std::stoi(token);

    token = line.substr(0, line.find(delimiter));
    conf_trees.min_leaf_size = std::stoi(token);
    
    while (std::getline(myfile, line))
    {             
        tree<T>* my_tree = new tree<T>();
        my_tree->load(line);
        this->trees.push_back(my_tree);
    }
    myfile.close();
}

template class Gbt<int>;  // Explicit instantiation
template class Gbt<double>;  // Explicit instantiation
