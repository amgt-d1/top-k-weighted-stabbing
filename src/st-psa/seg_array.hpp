#include "../utils/utils.hpp"
#include <map>


// define the format of interval
typedef std::pair<unsigned int, std::pair<unsigned int, unsigned int>> weight_pair;

// define BST node
struct Node
{
    unsigned int key;
    Node* parent = NULL;
    Node* left = NULL,  *right = NULL;
    unsigned int left_most = domain_size;
    unsigned int right_most = 0;
    std::vector<std::pair<unsigned int, weight_pair>> intervals;
    unsigned int node_idx = 0;
};


class seg_array
{
    /* member variable */
    std::vector<Node> node_set;
    Node* root = NULL;
    std::vector<std::vector<std::pair<unsigned int, unsigned int>>> path_arrays;


    // Utility function to create a new node
    Node* newNode(const unsigned int key)
    {
        Node node;
        // Node* node = new Node;
        node.key = key;
        node.node_idx = node_set.size();
        node_set.push_back(node);

        return &node_set[node_set.size() -1];
    }

    // A utility function to insert a new node with given key in BST
    Node* insert(Node* node, const unsigned int key)
    {
        // If the tree is empty, return a new node
        if (node == NULL) return newNode(key);
    
        // Otherwise, recur down the tree
        if (key < node->key)
        {
            node->left = insert(node->left, key);
        }
        else if (key > node->key)
        {
            node->right = insert(node->right, key);
        }
    
        // Return the (unchanged) node pointer
        return node;
    }

    // This function traverse the skewed binary tree and stores its nodes pointers in vector nodes[]
    void storeBSTNodes(Node* root, std::vector<Node*> &nodes)
    {
        // Base case
        if (root == NULL) return;
    
        // Store nodes in Inorder (which is sorted order for BST)
        storeBSTNodes(root->left, nodes);
        nodes.push_back(root);
        storeBSTNodes(root->right, nodes);
    }
 
    // Recursive function to construct binary tree
    Node* buildTreeUtil(std::vector<Node*> &nodes, const int start, const int end)
    {
        // base case
        if (start > end) return NULL;
    
        /* Get the middle element and make it root */
        int mid = (start + end) / 2;
        Node *root = nodes[mid];
    
        /* Using index in Inorder traversal, construct left and right subtress */
        root->left  = buildTreeUtil(nodes, start, mid-1);
        root->right = buildTreeUtil(nodes, mid+1, end);
    
        return root;
    }

    // This functions converts an unbalanced BST to a balanced BST
    Node* buildTree(Node* root)
    {
        // Store nodes of given BST in sorted order
        std::vector<Node*> nodes;
        storeBSTNodes(root, nodes);
    
        // Constructs BST from nodes[]
        int n = nodes.size();
        return buildTreeUtil(nodes, 0, n - 1);
    }

    // assign parent for each node
    void get_parent(Node* node, Node* parent)
    {
        node->parent = parent;
        if (node->left != NULL) get_parent(node->left, node);
        if (node->right != NULL) get_parent(node->right, node);
    }

    // get all leaf nodes
    void get_leaf(Node* node, std::vector<Node*> &leaves)
    {
        if (node->left != NULL) get_leaf(node->left, leaves);
        if (node->right != NULL) get_leaf(node->right, leaves);
        if (node->left == NULL && node->right == NULL)
        {
            // node->leaf_idx = leaves.size();
            leaves.push_back(node);
        }
    }

    // get interval of each node
    void get_interval(Node* node, const unsigned int left_most, const unsigned int right_most)
    {
        if (node->left == NULL) node->left_most = node->key;
        if (node->right == NULL) node->right_most = node->key;
        if (node->left_most > left_most) node->left_most = left_most;
        if (node->right_most < right_most) node->right_most = right_most;
        if (node->parent != NULL) get_interval(node->parent, node->left_most, node->right_most);
    }

    // function for inserting an interval into a BST
    void insert_interval(Node* node, const weight_pair &interval, const unsigned int idx)
    {
        // dis-joint
        if (node->right_most < interval.second.first || interval.second.second < node->left_most) return;

        if (interval.second.first <= node->left_most && node->right_most <= interval.second.second) // the interval covers the node's region
        {
            node->intervals.push_back({idx, interval});
        }
        else
        {
            if (node->left != NULL) insert_interval(node->left, interval, idx);
            if (node->right != NULL) insert_interval(node->right, interval, idx);
        }
    }

    // function for getting a path array
    void get_path_array(Node* node, const unsigned int &node_idx, std::vector<bool> &flags)
    {
        const unsigned int size = node->intervals.size();
        for (unsigned int i = 0; i < size; ++i) 
        {
            if (flags[node->intervals[i].first] == 0)
            {
                path_arrays[node_idx].push_back({node->intervals[i].second.first, node->intervals[i].first});
                flags[node->intervals[i].first] = 1;
            }
        }
        if (node->parent != NULL) get_path_array(node->parent, node_idx, flags);
    }

    // void get_path_array(Node* node, std::vector<bool> &flags, std::vector<std::pair<unsigned int, unsigned int>> &p_array)
    // {
    //     const unsigned int size = node->intervals.size();
    //     for (unsigned int i = 0; i < size; ++i) 
    //     {
    //         if (flags[node->intervals[i].first] == 0)
    //         {
    //             p_array.push_back({node->intervals[i].second.first, node->intervals[i].first});
    //             flags[node->intervals[i].first] = 1;
    //         }
    //     }
    //     std::sort(p_array.begin(), p_array.end());
    //     path_arrays[node->node_idx] = p_array;

    //     if (node->left != NULL) get_path_array(node->left, flags, p_array);
    //     if (node->right != NULL) get_path_array(node->right, flags, p_array);
    // }

    /* function for result update */
    void update_result(const unsigned int idx, const weight_pair &interval, std::multimap<unsigned int, unsigned int> &result)
    {
        if (result.size() < k)
        {
            result.insert({interval.first, idx});
        }
        else
        {
            // get threshold
            auto itr = result.end();
            --itr;
            unsigned int ths = itr->first;
            if (interval.first < ths)
            {
                // duplication check
                bool f = 1;
                auto it = result.find(interval.first);
                while (it->first == interval.first)
                {
                    if (it->second == idx)
                    {
                        f = 0;
                        break;
                    }
                    ++it;
                }

                if (f)
                {
                    result.insert({interval.first, idx});
                    itr = result.end();
                    --itr;
                    result.erase(itr);
                }
            }
        }
    }

    /* function for stabbing */
    void stabbing(const Node* n, const unsigned int &query, std::multimap<unsigned int, unsigned int> &result, unsigned int &count)
    {
        // access intervals
        unsigned int size = n->intervals.size();
        if (size > k) size = k;
        ++count;
        for (unsigned int i = 0; i < size; ++i)
        {
            ++count;
            update_result(n->intervals[i].first, n->intervals[i].second, result);
        }

        if (n->key > query)  // visit left child case
        {
            // go to left child
            if (n->left != NULL) stabbing(n->left, query, result, count);
        }
        else if (n->key <= query)    // visit right child case
        {
            // go to right child
            if (n->right != NULL) stabbing(n->right, query, result, count);
        }
    }

    /* function for stabbing */
    void stabbing(const Node* n, const unsigned int &query, unsigned int &count, int &node_idx)
    {
        ++count;

        // get node_idx
        if (n->left == NULL && n->right == NULL)
        {
            // leaf_idx = n->leaf_idx;
            node_idx = n->node_idx;
            return;
        }

        if (n->key > query)
        {
            // go to left child
            if (n->left != NULL)
            {
                if (n->left->left_most <= query && query <= n->left->right_most)
                {
                    stabbing(n->left, query, count, node_idx);
                }
                else
                {
                    node_idx = n->node_idx;
                    return;
                }
            }
        }
        else
        {
            // go to right child
            if (n->right != NULL)
            {
                if (n->right->left_most <= query && query <= n->right->right_most)
                {
                    stabbing(n->right, query, count, node_idx);
                }
                else
                {
                    node_idx = n->node_idx;
                    return;
                }
            }
        }
    }

public:

    /* constructor */
    seg_array()
    {
        const unsigned int size = intervals.size();
        mem = process_mem_usage();

        // start pre-processing
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // sort intervals
        std::sort(intervals.begin(), intervals.end());
        // std::cout << " sorting has been done successfully.\n";

        // insert into a BST
        node_set.reserve(size * 2);
        root = insert(NULL, intervals[0].second.first);
        insert(root, intervals[0].second.second);
        for (unsigned int i = 1; i < size; ++i)
        {
            insert(root, intervals[i].second.first);
            insert(root, intervals[i].second.second);
        }
        // std::cout << " A bst has been built successfully.\n";

        // balance
        root = buildTree(root);
        // std::cout << " The bst has been balanced successfully.\n";

        // get parent
        get_parent(root, NULL);
        // std::cout << " Parents have been computed successfully.\n";

        // get leaf nodes
        std::vector<Node*> leaves;
        get_leaf(root, leaves);
        // path_arrays.resize(leaves.size());
        // std::cout << " Leaf nodes have been computed successfully.\n";

        // compute intervals for nodes
        for (unsigned int i = 0; i < leaves.size(); ++i)
        {
            leaves[i]->left_most = leaves[i]->key;
            leaves[i]->right_most = leaves[i]->key;
            get_interval(leaves[i]->parent, leaves[i]->left_most, leaves[i]->right_most);
        }
        // std::cout << " Intervals of the nodes have been computed successfully.\n";

        // insert intervals to the BST
        for (unsigned int i = 0; i < size; ++i) insert_interval(root, intervals[i], i);

        // get path_arrays
        const unsigned int n_size = node_set.size();
        path_arrays.resize(n_size);

        #pragma omp parallel for num_threads(12)
        for (unsigned int i = 0; i < node_set.size(); ++i)
        {
            std::vector<bool> flags(size);
            get_path_array(&node_set[i], node_set[i].node_idx, flags);
            std::sort(path_arrays[node_set[i].node_idx].begin(), path_arrays[node_set[i].node_idx].end());
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n";

        mem = process_mem_usage() - mem;
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    /* function for stabbing query */
    void stabbing_query(const unsigned int query)
    {
        const unsigned int size = intervals.size();
        unsigned int count = 0;
        std::vector<bool> bitset(size);

        // start
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // prepare top-k result: pair of weight-idx
        std::vector<std::pair<unsigned int, unsigned int>> result;

        // get stabbed intervals
        int l_idx = -1;
        stabbing(root, query, count, l_idx);
        unsigned int size_ = path_arrays[l_idx].size();
        if (size_ > k) size_ = k;
        count += size_;
        for (unsigned int i = 0; i < size_; ++i) result.push_back(path_arrays[l_idx][i]);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_search_average += time_search;
        result_size = count;
        result_size_aggregate += count;
        time_vec.push_back(time_search);

        // output result
        output_result(1);
    }
};