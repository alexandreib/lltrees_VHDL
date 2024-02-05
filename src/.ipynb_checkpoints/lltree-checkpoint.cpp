#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

extern "C"
{
    class Node{
        public:
            Node() {std::cout << "Node" << std::endl; }
            int index_feature;
            int split;
            double leaf;
            // this->left_child = NULL;
            // this->right_child = NULL;

        // private :
        //     void set_children(Node left, Node right):
        //         Node::left
        //         self.__right = right
            
    };

    class lltree{
        private :
            int max_depth;
            int min_samples_split;

        public:
            lltree() : max_depth(8), min_samples_split(2) { }
            lltree(int max_depth, int min_samples_split) : max_depth(max_depth), min_samples_split(min_samples_split) { }

            void grow(const std::vector<std::vector<double>>& X, int level) {
                bool depth = (lltree::max_depth >= (level+1));
                std::cout << std::boolalpha <<  depth;
                
                int index_feature = 999;
                double split = 999;
                double total_loss = 10000;
                
                for (size_t i = 0; i < X.size(); i ++){
                    std::vector<double> x = X[i];
                    sort(x.begin(), x.end()); 
                    size_t lower = 0;
                    size_t upper = x.size();                 
                    // double min = x.front();
                    // double max = x.back();
                    for (size_t j = lower + 1; j < upper - 1; j ++){
                        // double loss = impurity(x, 0, j) + impurity (x, j, x.size());
                        double loss = (double(j)/double(x.size()))*impurity(x, lower, j) + (double(x.size() - j)/double(x.size()))*impurity (x, j, x.size());
        				if (loss < total_loss) {
                            total_loss = loss;
                            index_feature = i;
                            split = (x[j - 1] + x[j]) / 2;
                        }
                    }       
                }
                std::cout << total_loss << " " << index_feature <<  " " << split <<std::endl;
                left_node  = Node()
                right_node = Node()
                node.set_children(left_node,right_node)
                // self.__grow(node.get_left_node(),left_D,level+1)
                // self.__grow(node.get_right_node(),right_D,level+1)
            }

            double impurity(const std::vector<double>& ds, size_t lower, size_t upper) {
                if(ds.empty()){ return 0; }                  
                double average = 0;
                for(size_t i = lower; i<upper; i++){average = average + ds[i];}
                average = average / (upper-lower);
                double sum = 0;
                for (size_t i = lower; i < upper; i++) {sum += std::abs(average - ds[i]);} 
                return sum / ds.size(); 
            }    
            void printx(const std::vector<std::vector<double>> &X) {
                for (size_t i = 0; i < X.size(); i ++){
                    std::vector<double> x = X[i];
                    for (size_t j = 0; j < x.size(); j ++){std::cout<<x[j]<< " ";}
                    std::cout<<std::endl;
                }
            }    
    };
     
    void load_np(double *x_from_np, size_t m, size_t n, double *y_from_np) {
        std::vector<std::vector<double>> X;
        std::vector<double> Y;
        
        for (size_t i = 0; i < m; i++) {
            Y.push_back(y_from_np[i]);
        }
        for (size_t j = 0; j < n; j++) {
            std::vector<double> x;
            for (size_t i = 0; i < m; i++) {
                x.push_back(x_from_np[j * n + i]);
            }   
            X.push_back(x);
        }
        lltree mytree;
        mytree.grow(X, 1);
    }
}

int main() {}