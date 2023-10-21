#include "../utils/utils.hpp"
#include <map>


class sequential_scan
{
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


public:

    /* constructor */
    sequential_scan()
    {
        // sort intervals
        std::sort(intervals.begin(), intervals.end());
    }

    /* function for independent range sampling */
    void scan(const unsigned int query)
    {
        const unsigned int size = intervals.size();
        unsigned int count = 0;

        // start
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // prepare top-k result
        std::multimap<unsigned int, unsigned int> result;

        // get stabbed intervals
        for (unsigned int i = 0; i < size; ++i)
        {
            ++count;

            // check stabbing
            if (intervals[i].second.first <= query && query <= intervals[i].second.second)
            {
                update_result(i, result);
            }

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