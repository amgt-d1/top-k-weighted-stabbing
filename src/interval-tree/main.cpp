#include "interval_tree.hpp"


int main()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // make queries
    make_queries();


    // build an interval tree
    interval_tree I;
    I.build();

    // top-k stabbing
    for (unsigned int i = 0; i < query_size; ++i) I.stabbing_query(query_set[i]);
    time_search_average /= query_size;
    std::cout << " Average search time: " << time_search_average << "[microsec]\n";

    result_size_aggregate /= query_size;
    std::cout << " Average access size: " << result_size_aggregate << "\n\n";

    // output total result
    output_result(0);

    return 0;
}
