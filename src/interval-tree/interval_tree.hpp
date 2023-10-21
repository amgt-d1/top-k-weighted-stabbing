#include "../utils/utils.hpp"
#include <map>


struct node
{
    unsigned int median = 0;
    node* left_child = NULL;
    node* right_child = NULL;
    std::vector<std::pair<unsigned int, unsigned int>> left_sorted;
    std::vector<std::pair<unsigned int, unsigned int>> right_sorted;
    unsigned int height = 0;
};


class interval_tree
{
    /* member variable */
    std::vector<unsigned int> sorted_array_val, sorted_array_idx;
    std::vector<node> nodes;
    unsigned int height = 0;

    /* function for making a node of interval tree */
    void make_node(const std::vector<unsigned int> &indexes, node* n)
    {
        // get size
        const unsigned int size = indexes.size();

        // get 2 x size endpoints
        std::vector<unsigned int> endpoints;
        for (unsigned int i = 0 ; i < size; ++i)
        {
            // get index
            const unsigned int idx = indexes[i];

            // push
            endpoints.push_back(intervals[idx].second.first);
            endpoints.push_back(intervals[idx].second.second);
        }

        // sort 2 x size end points
        std::sort(endpoints.begin(), endpoints.end());

        // get median
        n->median = endpoints[size];

        // compute overlap, left, and right
        std::vector<unsigned int> left_indexes, right_indexes;
        for (unsigned int i = 0; i < size; ++i)
        {
            // get index
            const unsigned int idx = indexes[i];

            if (intervals[idx].second.second < n->median)   // left case
            {
                left_indexes.push_back(idx);
            }
            else if (intervals[idx].second.first > n->median)  // right case
            {
                right_indexes.push_back(idx);
            }
            else    // overlap case
            {
                n->left_sorted.push_back({intervals[idx].second.first,idx});
                n->right_sorted.push_back({intervals[idx].second.second,idx});
            }
        }

        // sort left_sorted & right_sorted
        std::sort(n->left_sorted.begin(), n->left_sorted.end());        // ascending
        std::sort(n->right_sorted.rbegin(), n->right_sorted.rend());    // descending

        // get left child
        if (left_indexes.size() > 0)
        {
            node l;
            l.height = n->height + 1;
            if (l.height > height) height = l.height;

            nodes.push_back(l);
            n->left_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(left_indexes, n->left_child);
        }

        // get right child
        if (right_indexes.size() > 0)
        {
            node r;
            r.height = n->height + 1;
            if (r.height > height) height = r.height;

            nodes.push_back(r);
            n->right_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(right_indexes, n->right_child);
        }
    }

    /* function for result update */
    void update_result(const unsigned int idx, std::multimap<unsigned int, unsigned int> &result)
    {
        if (result.size() < k)
        {
            result.insert({intervals[idx].first, idx});
        }
        else
        {
            // get threshold
            auto itr = result.end();
            --itr;
            unsigned int ths = itr->first;
            if (intervals[idx].first < ths)
            {
                result.insert({intervals[idx].first, idx});
                itr = result.end();
                --itr;
                result.erase(itr);
            }
        }
    }

    /* function for stabbing */
    void stabbing(const node* n, const unsigned int query, std::multimap<unsigned int, unsigned int> &result, unsigned int &count)
    {
        if (n->median > query)  // visit left child case
        {
            // access left-sorted
            for (unsigned int i = 0; i < n->left_sorted.size(); ++i)
            {
                ++count;
                if (n->left_sorted[i].first <= query)
                {
                    update_result(n->left_sorted[i].second, result);
                }
                else
                {
                    break;
                }
            }

            // go to left child
            if (n->left_child != NULL) stabbing(n->left_child, query, result, count);
        }
        else if (n->median <= query)    // visit right child case
        {
            // access right-sorted
            for (unsigned int i = 0; i < n->right_sorted.size(); ++i)
            {
                ++count;
                if (n->right_sorted[i].first >= query)
                {
                    update_result(n->right_sorted[i].second, result);
                }
                else
                {
                    break;
                }
            }

            // go to right child
            if (n->right_child != NULL) stabbing(n->right_child, query, result, count);
        }
    }

    void stabbing(const node* n, const unsigned int query, std::vector<bool> &result, unsigned int &count)
    {
        if (n->median > query)  // visit left child case
        {
            // access left-sorted
            for (unsigned int i = 0; i < n->left_sorted.size(); ++i)
            {
                ++count;
                if (n->left_sorted[i].first <= query)
                {
                    result[n->left_sorted[i].second] = 1;
                }
                else
                {
                    break;
                }
            }

            // go to left child
            if (n->left_child != NULL) stabbing(n->left_child, query, result, count);
        }
        else if (n->median <= query)    // visit right child case
        {
            // access right-sorted
            for (unsigned int i = 0; i < n->right_sorted.size(); ++i)
            {
                ++count;
                if (n->right_sorted[i].first >= query)
                {
                    result[n->right_sorted[i].second] = 1;
                }
                else
                {
                    break;
                }
            }

            // go to right child
            if (n->right_child != NULL) stabbing(n->right_child, query, result, count);
        }
    }


public:

    /* constructor */
    interval_tree(){}

    /* function for building an interval tree */
    void build()
    {
        mem = process_mem_usage();

        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        const unsigned int size = intervals.size();

        /***********************/
        /* make a sorted array */
        /***********************/
        std::vector<std::pair<unsigned int, unsigned int>> sorted_array;
        for (unsigned int i = 0; i < size; ++i)
        {
            sorted_array.push_back({intervals[i].second.first,i});     // insert <left, ID> 
            sorted_array.push_back({intervals[i].second.second,i});    // insert <right, ID>
        }
        std::sort(sorted_array.begin(), sorted_array.end());
        for (unsigned int i = 0; i < sorted_array.size(); ++i)
        {
            sorted_array_val.push_back(sorted_array[i].first);
            sorted_array_idx.push_back(sorted_array[i].second);
        }

        /**************************/
        /* build an interval tree */
        /**************************/

        // get indexes of all intervals
        std::vector<unsigned int> indexes;
        for (unsigned int i = 0; i < size; ++i) indexes.push_back(i);

        // reserve memory
        nodes.reserve(size);

        // make a root node
        node n;
        nodes.push_back(n);

        // recursive partition
        make_node(indexes, &nodes[0]);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n\n";
        std::cout << " ---- Interval tree info. ----\n";
        std::cout << " #nodes: " << nodes.size() << "\n";
        std::cout << " height: " << height << "\n";

        mem = process_mem_usage() - mem;
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    /* function for stabbing query */
    void stabbing_query(const unsigned int query)
    {
        const unsigned int size = intervals.size();
        unsigned int count = 0;

        // start
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // prepare top-k result
        std::multimap<unsigned int, unsigned int> result;
        // std::vector<bool> result_(size);

        // get stabbed intervals
        stabbing(&nodes[0], query, result, count);

        // for (unsigned int i = 0; i < size; ++i)
        // {
        //     if (result_[i]) update_result(i, result);
        // }

        /* for debug (show top-k smallest weight) */
        // auto itr = result.begin();
        // while (itr != result.end())
        // {
        //     std::cout << " weight: " << itr->first << "\t idx: " << itr->second << "\n";
        //     ++itr;
        // }

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