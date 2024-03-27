#include "conf.hpp"
#include "gbt.hpp"
#include "factories.hpp"

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////// base_gbt //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void base_gbt::print_epoch_log(int & epoch, double & metric_tr, double & metric_va, double & residuals_average) 
{
    if (conf_gbt.verbose == 1) 
    {
        std::cout << "Epoch : " << std::setw(5) << epoch << " Metric Train : " << std::setw(7) << metric_tr << " Metric va : " << std::setw(7) << metric_va << " Residuals (mean) : " << std::setw(7) << residuals_average << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Gbt template ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<class T>
Gbt<T>::Gbt() 
{
}

template<class T>
Gbt<T>::~Gbt() 
{
    for (auto p : this->trees) 
    {
        delete p;
    } 
    this->trees.clear();
}

template<class T>
void Gbt<T>::print() 
{
    for (long unsigned int i =0; i < this->trees.size(); i++)
    {
        std::cout << "Tree : " << i << std::endl;
        this->trees[i]->printBT();
    }
}

///////////////////////////////// Save / Load ////////////////////////////////
template<class T>
void Gbt<T>::save() 
{    
    std::ofstream myfile("trees.txt");
    myfile << conf_gbt.verbose << ":"  
        << conf_gbt.mode << ":"  
        << conf_gbt.criterion_name << ":"  
        << conf_gbt.metric_name << ":" 
        << conf_gbt.epochs << ":"  
        << conf_gbt.learning_rate << ":" 
        << conf_gbt.number_of_threads  << "\n";
    myfile << conf_trees.max_depth << ":"  << conf_trees.min_leaf_size << "\n";
    for (long unsigned int i =0; i < this->trees.size(); i++)
    {
        this->trees[i]->save(myfile);
        myfile << "\n";
    }
    myfile.close();
}

template<class T>
void Gbt<T>::load() 
{    
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

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Classification ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void classification::fit(const XY & tr, const XY & va) 
{
    std::cout<< "Gbt_classification fit" << std::endl;  
    const int* y_tr = tr.get_y<int>();
    const int* y_va = va.get_y<int>();

    this->classes.insert(y_tr, y_tr + tr.number_of_rows); 
    std::cout<<"All the distinct element for classification in sorted order are: " << std::endl;
    for(auto it:this->classes) std::cout<<it<<" "; std::cout << std::endl;
    
    std::shared_ptr<criterion<int>> crit = criterion_Factory<int>(); 
    std::shared_ptr<metrics<int>> metr = metric_Factory<int>(); 

    std::vector<int> pred_tr_final(tr.number_of_rows, 0.0);
    std::vector<int> pred_va_final(va.number_of_rows, 0.0);
    
    std::vector<int> tr_residuals;
    // tr_residuals.insert(tr_residuals.end(), type_tr.y, type_tr.y + tr.number_of_rows); 
    tr_residuals.insert(tr_residuals.end(), y_tr, y_tr + tr.number_of_rows); 
    
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++){        
        tree<int>* my_tree = new tree<int>(crit);
        
        my_tree->fit(tr, tr_residuals);
        this->trees.push_back(my_tree);
        pred_tr_final = my_tree->predict<int>(tr);
        pred_va_final = my_tree->predict<int>(va);

        double metric_tr = metr->get(pred_tr_final, y_tr);
        double metric_va = metr->get(pred_va_final, y_va);
         
        this->print_epoch_log(epoch,metric_tr, metric_va,  metric_tr);
    }
}

void classification::predict(XY & d) 
{
    std::vector<int> preds(d.number_of_rows);
    for(auto const tree : trees)
    {
        preds = tree->predict<int>(d);            
    }
    d.set_pred<int>(preds);
}

void classification::predict_proba(XY & d) 
{
    std::cout << "predict_proba"<<std::endl;
    
    std::vector<std::unordered_map<int, double>> preds;
    for(auto const tree : trees) 
    {
        preds = tree->predict<std::unordered_map<int, double>>(d);     
    }

    std::vector<double> predictions;
    for (auto pre : preds) 
    {
        for (auto classe : classes) 
        {
            // std::cout << classe << " " << pre[classe] <<std::endl;
            predictions.push_back(pre[classe]);
        }
    }
    d.number_of_classes = this->classes.size();

    // for (auto p : pre) 
    // {
    //     std::cout << p.first << " " << p.second <<std::endl;
    // }    
    d.set_pred<double>(predictions);
    // for (auto p : predictions) 
    // {
    //     std::cout << p  <<std::endl;
    // }   
        
    // }
    // Y<int>& type_d = static_cast <Y<int>&> (d);
    // type_d.pred = preds;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Regression /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void regression::fit(const XY & tr, const XY & va) 
{
    const double* y_tr = tr.get_y<double>();
    const double* y_va = va.get_y<double>();
    std::shared_ptr<criterion<double>> crit = criterion_Factory<double>(); 
    std::shared_ptr<metrics<double>> metr = metric_Factory<double>(); 

    std::vector<double> pred_tr_final(tr.number_of_rows, 0.0);
    std::vector<double> pred_va_final(va.number_of_rows, 0.0);
    
    std::vector<double> tr_residuals;
    tr_residuals.insert(tr_residuals.end(), y_tr, y_tr + tr.number_of_rows); 
    
    for (int epoch = 1; epoch < conf_gbt.epochs + 1; epoch++)
    {        
        tree<double>* my_tree = new tree<double>(crit);
        
        my_tree->fit(tr, tr_residuals);
        this->trees.push_back(my_tree);
        
        my_tree->pred_and_add(va, pred_va_final);
        double mean_residuals = 0;
        for (int index = 0; index < tr.number_of_rows; index ++)
        {
            double row_pred = conf_gbt.learning_rate * my_tree->predict_row<double>(tr.x + index * tr.number_of_cols);
            tr_residuals[index] -= row_pred;
            pred_tr_final[index] += row_pred; 
            mean_residuals += row_pred;
        }
        double metric_tr = metr->get(pred_tr_final, y_tr);
        double metric_va = metr->get(pred_va_final, y_va);
        this->residuals_average.push_back(mean_residuals);        
        this->print_epoch_log(epoch,metric_tr, metric_va, mean_residuals );
    }
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


