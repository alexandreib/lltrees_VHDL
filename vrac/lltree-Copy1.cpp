#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>

extern "C"
{
    class node{
        private:
            node* l_node;
            node* r_node;

        public:
            node() : isleaf(true), tree_level(0) {std::cout << "Creating Node 0." << std::endl; }
            node(int tree_level) : isleaf(true), tree_level(tree_level) {}
            bool isleaf;
            int tree_level;
            int saved_col_index = std::numeric_limits<int>::max();
            double saved_split = std::numeric_limits<double>::max();
            double saved_loss = std::numeric_limits<double>::max();
            double leaf_value = std::numeric_limits<double>::max();

            void set_children(node* left, node* right) {
                 l_node = left;
                 r_node = right;
             }

            node* get_l_children() {
                 return l_node;
             }
            
            node* get_r_children() {
                 return r_node;
             }

            void print() {
                std::cout << "level : " << tree_level << std::endl;
                std::cout << "saved_loss : " << saved_loss << std::endl;
                std::cout << "saved_col_index : " << saved_col_index << std::endl;
                std::cout << "saved_split : " << saved_split << std::endl;
                std::cout << "isleaf : " << std::boolalpha << isleaf << std::endl;
                std::cout << "leaf_value : " << leaf_value << std::endl;
                std::cout << "**********" << std::endl;
            }
    };

    class lltree{
        private :
            int max_depth;
            long unsigned int min_size_split;

            void grow(node* pnode, std::vector<std::vector<double>> XY) {
                bool depth = (lltree::max_depth >= (pnode->tree_level + 1));
                bool msamp = (lltree::min_size_split <= (XY.size()));
                size_t index_col_Y = XY[0].size() - 1;
                size_t n_rows = XY.size();
                if (depth && msamp) {    
                    //print_XY(XY);
                    for (size_t index_col = 0; index_col < index_col_Y; index_col ++) {
                        std::vector<double> col, Y;
                        for (size_t index_row = 0; index_row < n_rows; index_row ++){
                            //std::cout<< "XY[index_row][index_col] : " << XY[index_row][index_col];
                            col.push_back(XY[index_row][index_col]);
                            Y.push_back(XY[index_row][index_col]);
                            //std::cout<< "XY[index_row][index_col_Y] : " << XY[index_row][index_col_Y];
                        }
                        // sort(col.begin(), col.end()); 
                        // col.erase(unique(col.begin(), col.end()), col.end()); 
                        
                        // for (double val : col) {std::cout<<val << " end col" << index_col ;}
                        double ratio = double(col.size());
                        for(size_t idx = 0; idx < col.size() - 1; idx++) {
                            double split = (col[idx ] + col[idx + 1])/2;
                            //std::cout<< "index_col : " << index_col <<"split :"<<split << ". idx :"<< idx << ", val :" << col[idx] << std::endl;
                            std::vector<double> l_Y, r_Y;
                            for (size_t index_row = 0; index_row < n_rows; index_row ++){
                                //std::cout << "index_row"<< index_row <<std::endl;
                                if (XY[index_row][index_col] < split) {
                                    // std::cout << "l_Y"<< XY[index_row][index_col_Y] <<std::endl;
                                    l_Y.push_back(XY[index_row][index_col_Y]);
                                }
                                else {
                                    //std::cout << "r_Y"<< XY[index_row][index_col_Y] <<std::endl;
                                    r_Y.push_back(XY[index_row][index_col_Y]);
                                }
                            }
                            // std::cout << "r_Y "<< r_Y.size() <<std::endl;
                            // std::cout << "l_Y "<< l_Y.size() <<std::endl;
                            double dj = double(idx);
                            double loss = impurity(l_Y) + impurity (r_Y);
                            // double loss = (dj/ratio)*impurity(l_Y) + ((ratio - dj)/ratio)*impurity (r_Y);
                            // std::cout << loss << " " << pnode->saved_loss << " " << idx << " " << col.size()-idx << std::endl;
                            if ((loss < pnode->saved_loss) && (r_Y.size() >= lltree::min_size_split) && (l_Y.size() >= lltree::min_size_split)) {
                                pnode->saved_loss = loss;
                                pnode->saved_col_index = index_col;
                                pnode->saved_split = split;
                                pnode->isleaf = false ;  
                                // pnode->print();
                                // for (size_t i = 0; i<4; i++){std::cout<<l_Y[i] << " l_y" <<std::endl ;
                                //                             std::cout<<r_Y[i] << " r_y" <<std::endl ;}
                            }
                        }  
                    }
                }
                if (!pnode->isleaf) {
                    std::vector<std::vector<double>> l_XY, r_XY;
                    for (size_t index_row = 0; index_row < n_rows; index_row ++){
                        if (XY[index_row][pnode->saved_col_index] < pnode->saved_split) 
                            {l_XY.push_back(XY[index_row]);}
                        else 
                            {r_XY.push_back(XY[index_row]);}
                    }
                    node* l_node = new node(pnode->tree_level + 1);
                    node* r_node = new node(pnode->tree_level + 1);
                    pnode->set_children(l_node, r_node);
                    // pnode->print();
                    grow(l_node, l_XY);
                    grow(r_node, r_XY);
                } 
                else {
                    double average = 0;
                    // std::cout <<  n_rows << " n_rows" ;
                    for (size_t index_row = 0; index_row < n_rows; index_row ++){
                        //std::cout << index_row << " " ;
                        average = average + XY[index_row][index_col_Y];
                    }                
                    // std::cout << average / index_col_Y << " " ;
                    pnode->leaf_value = average / n_rows; 
                    pnode->isleaf = true ;  
                    // pnode->print();
                }  
            }

        public:
            lltree() : max_depth(5), min_size_split(2) {}
            lltree(int max_depth, int min_size_split) : max_depth(max_depth), min_size_split(min_size_split) {}

            double impurity(const std::vector<double> Y) {
                if(Y.empty()){ return 0; }                  
                double average = 0, sum = 0;
                for(double value : Y){average = average + value;}
                average = average / Y.size();
                for(double value : Y) {sum += std::abs(average - value);} 
                return sum / Y.size(); 
            }    

            node fit(std::vector<std::vector<double>> XY) {
                    node node_0;
                    grow(&node_0, XY);
                    return node_0;
            }

            double _traverse(node *pnode, std::vector<double> row) {
                if (pnode->isleaf){
                    return(pnode->leaf_value);
                }
                
                if (row[pnode->saved_col_index] < pnode->saved_split) {
                    return(_traverse(pnode->get_l_children(),row)); 
                }
                else {
                    return(_traverse(pnode->get_r_children(),row)); 
                }
            }

            std::vector<double>* predict(node *pnode, std::vector<std::vector<double>> &XY ) {
                std::vector<double>* results = new std::vector<double>;
                // node_0->print_node();
                for (size_t index_row = 0; index_row < XY.size(); index_row ++){
                    double result = _traverse(pnode, XY[index_row]);
                    results->push_back(result);
                }
                return results;
            } 

            std::vector<std::vector<double>> get_XY(double *X, size_t m, size_t n, double *Y){
                std::vector<std::vector<double>> XY;
                //// X[Row][Col] : indexing
                for (size_t i = 0; i < m; i++) {
                    std::vector<double> row;
                    for (size_t j = 0; j < n; j++) {row.push_back(X[i * n + j ]);} 
                    row.push_back(Y[i]);  
                    XY.push_back(row);
                }
                return XY;
            }

            void print_XY(const std::vector<std::vector<double>> X) {
                for (size_t i = 0; i < X.size(); i ++){
                    std::vector<double> row = X[i];
                    for (size_t j = 0; j < row.size(); j ++){std::cout << row[j] << " ";}
                    std::cout << std::endl;
                }
            }  
            void print_XY_shape(const std::vector<std::vector<double>> X) {
                std::cout << "[" << X.size() << "," << X[0].size() << "]" << std::endl;
            }  

            void print_tree(node *node_0) {
                if (!node_0->isleaf) {
                    print_tree(node_0->get_l_children());
                    print_tree(node_0->get_r_children());
                }
                else {node_0->print();}
            }  
    };

    void fit(double *x_np, size_t m, size_t n, double *y_np) {
        lltree mytree;
        
        std::vector<std::vector<double>> XY;
        XY = mytree.get_XY(x_np,m,n,y_np);
        // mytree.print_XY(XY);
        node node_0 = mytree.fit(XY);
        
        // mytree.print_tree(&node_0);
        
        std::vector<double>* results = new std::vector<double>;
        results = mytree.predict(&node_0, XY);
        
        double mae = 0;
        for (size_t i = 0; i < m; i++) { mae = mae + std::abs((*results)[i] - y_np[i]); }
        std::cout << "mae "<< mae / m << std::endl;
        }
}

int main() {}