#include "conf.hpp"
#include "gbt.hpp"
#include "factories.hpp"

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////// base_gbt //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void base_gbt::print_epoch_log(int & epoch, double & metric_tr, double & metric_va, double & residuals_average) 
{
    if (conf::verbose == 1) 
    {
        std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric va : " << std::setw(7) << metric_va << " Residuals (mean) : " << std::setw(7) << residuals_average << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Gbt template ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<class T>
gbt<T>::gbt() 
{
}

template<class T>
gbt<T>::~gbt() 
{
    for (auto p : this->trees) 
    {
        delete p;
    } 
    this->trees.clear();
}

template<class T>
void gbt<T>::print() 
{
    for (long unsigned int i =0; i < this->trees.size(); i++)
    {
        std::cout << "Tree : " << i << std::endl;
        this->trees[i]->printBT();
    }
}

///////////////////////////////// Save / Load ////////////////////////////////
template<class T>
void gbt<T>::save() 
{    
    std::ofstream myfile("trees.txt");
    myfile << conf::verbose << ":"  
        << conf::mode << ":"  
        << conf::tree::criterion_name << ":"  
        << conf::gbt::metric_name << ":" 
        << conf::gbt::epochs << ":"  
        << conf::gbt::learning_rate << ":" 
        << conf::number_of_threads  << "\n";
    myfile << conf::tree::max_depth << ":"  << conf::tree::min_leaf_size << "\n";
    for (long unsigned int i =0; i < this->trees.size(); i++)
    {
        this->trees[i]->save(myfile);
        myfile << "\n";
    }
    myfile.close();
}

template<class T>
void gbt<T>::load() 
{    
    std::ifstream myfile("trees.txt");
    std::string delimiter = ":";
    std::string line;
    std::getline(myfile, line);
    std::string token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf::verbose = std::stoi(token);

    conf::mode = line.substr(0, line.find(delimiter));
    line.erase(0, conf::mode.size() + delimiter.size());
    
    conf::tree::criterion_name = line.substr(0, line.find(delimiter));
    line.erase(0, conf::tree::criterion_name.size() + delimiter.size());
    
    conf::gbt::metric_name = line.substr(0, line.find(delimiter));
    line.erase(0, conf::gbt::metric_name.size() + delimiter.size());
    
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf::gbt::epochs = std::stoi(token);

    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf::gbt::learning_rate = std::stod(token);
    
    token = line.substr(0, line.find(delimiter));
    conf::number_of_threads = std::stoi(token);

    std::getline(myfile, line);
    token = line.substr(0, line.find(delimiter));
    line.erase(0, token.size() + delimiter.size());
    conf::tree::max_depth = std::stoi(token);

    token = line.substr(0, line.find(delimiter));
    conf::tree::min_leaf_size = std::stoi(token);
    
    while (std::getline(myfile, line))
    {             
        tree<T>* my_tree = new tree<T>();
        my_tree->load(line);
        this->trees.push_back(my_tree);
    }
    myfile.close();
}

template class gbt<int>;  // Explicit instantiation
template class gbt<double>;  // Explicit instantiation

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Classification ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void classification::fit(const XY & tr, const XY & va) 
{    
    ThreadPool * pool = new ThreadPool(conf::number_of_threads);
    std::cout<< "Gbt_classification fit" << std::endl;  
    const int* y_tr = tr.get_y<int>();
    const int* y_va = va.get_y<int>();

    this->classes.insert(y_tr, y_tr + tr.number_of_rows); 
    std::cout<<"All the distinct element for classification in sorted order are: ";
    for(auto it:this->classes) std::cout<<it<<" "; std::cout << std::endl;

    std::unique_ptr<base_factory> factory = base_factory::get_instance();  
    std::shared_ptr<base_criterion> criterion = factory->Criterion(); 
    std::shared_ptr<base_metrics> metric = factory->Metric(); 
    
    std::vector<double> weights(tr.number_of_rows, 1.0);
    std::vector<int> vec_y_tr;
    vec_y_tr.insert(vec_y_tr.end(), y_tr, y_tr + tr.number_of_rows); 
    
    int total_models_weights = 0;
    for (int epoch = 0; epoch < conf::gbt::epochs + 1; ++ epoch){        
        tree<int>* my_tree = new tree<int>(criterion, pool);
        
        my_tree->fit(tr, vec_y_tr, weights);
        this->trees.push_back(my_tree);
        std::vector<int> pred_tr = my_tree->predict<int>(tr);
        std::vector<int> pred_va = my_tree->predict<int>(va);
        int model_weight = 0;
        for (long unsigned int idx =0; idx < pred_tr.size(); idx ++)
        {
            if (pred_tr[idx] != y_tr[idx])
            {
                 weights[idx] *= std::exp(conf::gbt::learning_rate);
            }
            else 
            {
                model_weight ++;
            }
        }
        total_models_weights += model_weight;
        this->models_weights.push_back(model_weight);
        
        std::vector<double> models_weights_normalized;
        for (auto mw : this->models_weights)
        {
            models_weights_normalized.push_back(mw / total_models_weights);
        }
        pred_tr = this->get_predict(tr, models_weights_normalized);
        pred_va = this->get_predict(va, models_weights_normalized);
        
        double metric_tr = metric->get(pred_tr, y_tr);
        double metric_va = metric->get(pred_va, y_va);
        
        this->print_epoch_log(epoch, metric_tr, metric_va, metric_tr);
    }    

    for (long unsigned int idx =0; idx < this->models_weights.size(); idx ++)
    {
        this->models_weights[idx] /= total_models_weights;
    }
    delete pool;
}

std::vector<std::unordered_map<int, double>> classification::get_proba(const XY & d) const
{
    return this->get_proba(d, this->models_weights);
}

std::vector<std::unordered_map<int, double>> classification::get_proba(const XY & d, const std::vector<double> models_weights) const
{
    std::vector<std::unordered_map<int, double>> preds_proba(d.number_of_rows, this->init_map_from_clases());
    for (long unsigned int model_idx = 0; model_idx <  models_weights.size(); model_idx ++)
    {
        this->trees[model_idx]->pred_and_add(d, preds_proba, models_weights[model_idx]);     
    }
    return preds_proba;
}

void classification::predict_proba(XY & d) 
{
    std::vector<std::unordered_map<int, double>> preds_probas = this->get_proba(d);
    std::vector<double> predictions;
    for (auto pred_proba : preds_probas) 
    {
        for (auto proba : pred_proba) 
        {
            predictions.push_back(proba.second);
        }
    }
    d.number_of_classes = this->classes.size();
    d.set_pred<double>(predictions);
}

std::unordered_map<int, double> classification::init_map_from_clases() const
{
    std::unordered_map<int, double> New_Map;
    for (auto classe : this->classes) 
    {
        New_Map[classe] = 0;
    }
    return New_Map;
}  
std::vector<int> classification::get_predict(const XY & d) const
{
    return this->get_predict(d, this->models_weights);
}

std::vector<int> classification::get_predict(const XY & d, const std::vector<double> models_weights) const
{
    const std::vector<std::unordered_map<int, double>> preds_proba = this->get_proba(d, models_weights);
    const std::vector<int> preds = this->extract_pred_from_proba(preds_proba);
    return preds;
}

std::vector<int> classification::extract_pred_from_proba(const std::vector<std::unordered_map<int, double>> probas) const
{
    std::vector<int> preds(probas.size());
    for (long unsigned int row_idx = 0; row_idx < probas.size();  row_idx ++) 
    {
       auto biggest = std::max_element(probas[row_idx].begin(), probas[row_idx].end(),[] (const auto &a, const auto &b) { return a.second < b.second; });
        preds[row_idx] = biggest-> first;
    }
    return preds;
}

void classification::predict(XY & d) 
{
    std::vector<int> preds = this->get_predict(d);
    d.set_pred<int>(preds);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Regression /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void regression::fit(const XY & tr, const XY & va) 
{
    ThreadPool * pool = new ThreadPool(conf::number_of_threads);
    const double* y_tr = tr.get_y<double>();
    const double* y_va = va.get_y<double>();
    std::unique_ptr<base_factory> factory = base_factory::get_instance();  
    std::shared_ptr<base_criterion> criterion = factory->Criterion(); 
    std::shared_ptr<base_metrics> metric = factory->Metric(); 

    std::vector<double> pred_tr_final(tr.number_of_rows, 0.0);
    std::vector<double> pred_va_final(va.number_of_rows, 0.0);
    
    std::vector<double> tr_residuals;
    tr_residuals.insert(tr_residuals.end(), y_tr, y_tr + tr.number_of_rows); 
    
    for (int epoch = 1; epoch < conf::gbt::epochs + 1; epoch++)
    {        
        tree<double>* my_tree = new tree<double>(criterion, pool);
        
        my_tree->fit(tr, tr_residuals);
        this->trees.push_back(my_tree);
        
        my_tree->pred_and_add(va, pred_va_final);
        double mean_residuals = 0;
        for (int index = 0; index < tr.number_of_rows; index ++)
        {
            double row_pred = conf::gbt::learning_rate * my_tree->predict_row<double>(tr.x + index * tr.number_of_cols);
            tr_residuals[index] -= row_pred;
            pred_tr_final[index] += row_pred; 
            mean_residuals += row_pred;
        }
        double metric_tr = metric->get(pred_tr_final, y_tr);
        double metric_va = metric->get(pred_va_final, y_va);
        this->residuals_average.push_back(mean_residuals);        
        this->print_epoch_log(epoch,metric_tr, metric_va, mean_residuals );
    }
    delete pool;
    
}

void regression::predict(XY & d) 
{
    std::vector<double> preds(d.number_of_rows);
    for(auto const tree : trees) 
    {
        tree->pred_and_add(d, preds);            
    }
    d.set_pred<double>(preds);
}


