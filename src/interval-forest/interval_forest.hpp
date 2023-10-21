#include "interval_tree.hpp"


class interval_forest
{
    /* member variable */
    std::vector<interval_tree> forest;

public:

    /* constructor */
    interval_forest()
    {
        mem = process_mem_usage();

        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // sort intervals
        std::sort(intervals.begin(), intervals.end());

        // get partition size
        const unsigned int size = intervals.size();
        const unsigned int partition_size = sqrt(size) * 20;

        std::vector<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>> intervals_temp;
        unsigned int count = 0;

        for (unsigned int i = 0; i < size; ++i)
        {
            if (count < partition_size)
            {
                ++count;
                intervals_temp.push_back(intervals[i]);
            }

            if (count == partition_size || i == size - 1)
            {
                // make an interval-tree instance
                interval_tree i(intervals_temp);
                forest.push_back(i);

                // init
                count = 0;
                intervals_temp.clear();
            }
        }

        // build an interval forest
        for (unsigned int i = 0; i < forest.size(); ++i) forest[i].build();

        mem = process_mem_usage() - mem;

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n";
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    /* function for stabbing query */
    void stabbing_query(const unsigned int query)
    {
        unsigned int count = 0;
        const unsigned int size = forest.size();

        // start
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        std::multimap<unsigned int, unsigned int> result;

        for (unsigned int i = 0; i < size; ++i)
        {
            forest[i].stabbing_query(query, result, count);
            if (result.size() == k) break;
        }

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