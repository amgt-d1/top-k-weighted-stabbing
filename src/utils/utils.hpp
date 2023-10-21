#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>


// dataset info
unsigned int dataset_id = 0;
std::vector<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>> intervals;
unsigned int domain_size = 0;
long double domain_max = 0;
long double domain_min = 10000000000;
double scalability = 1.0;


// memory
double mem = 0;

// running time
double time_preprocess = 0;
double time_search = 0;
double time_search_average = 0;
std::vector<double> time_vec;


// queries
unsigned int k = 0;
const unsigned int query_size = 1000;
unsigned int result_size = 0;
double result_size_aggregate = 0;
double selectivity = 0;
std::vector<unsigned int> query_set;


// compute memory usage
double process_mem_usage()
{
    double resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    resident_set = rss * page_size_kb;

	return resident_set / 1000;
}

// parameter input
void input_parameter()
{
    std::ifstream ifs_dataset_id("parameter/dataset_id.txt");
    std::ifstream ifs_k("parameter/k.txt");
    std::ifstream ifs_scalability("parameter/scalability.txt");

    if (ifs_dataset_id.fail())
    {
        std::cout << " dataset_id.txt does not exist." << std::endl;
        std::exit(0);
    }
    if (ifs_k.fail())
    {
        std::cout << " k.txt does not exist." << std::endl;
        std::exit(0);
    }
    if (ifs_scalability.fail())
    {
        std::cout << " scalability.txt does not exist." << std::endl;
        std::exit(0);
    }

    while (!ifs_dataset_id.eof()) { ifs_dataset_id >> dataset_id; }
    while (!ifs_k.eof()) { ifs_k >> k; }
    while (!ifs_scalability.eof()) { ifs_scalability >> scalability; }
}

// data input
void input_data()
{
    std::mt19937 mt;
    std::uniform_real_distribution<> rnd_prob(0, 1.0);

    // interval variable
    std::pair<unsigned int, std::pair<unsigned int, unsigned int>> interval;

    // dataset input
    std::string f_name = "../../dataset/";
    if (dataset_id == 0) f_name += "demo.csv";

    // file input
    std::ifstream ifs_file(f_name);
    std::string full_data;

    // error check
    if (ifs_file.fail())
    {
        std::cout << " data file does not exist." << std::endl;
        std::exit(0);
    }

    // read data
    while (std::getline(ifs_file, full_data))
    {
        std::string meta_info;
        std::istringstream stream(full_data);
        std::string type = "";

        for (unsigned int i = 0; i < 2; ++i)
        {
            std::getline(stream, meta_info, ',');
            std::istringstream stream_(meta_info);
            long double val = std::stold(meta_info);
            if (i == 0) interval.second.first = (unsigned int)val;
            if (i == 1) interval.second.second = (unsigned int)val;
        }

        if (interval.second.first < interval.second.second)
        {
            if (domain_max < interval.second.second) domain_max = interval.second.second;
            if (domain_min > interval.second.first) domain_min = interval.second.first;
            if (rnd_prob(mt) <= scalability) intervals.push_back(interval);
        }
    }

    /* show input parameters */
    std::cout << " -------------------------------\n";
    std::cout << " dataset ID: " << dataset_id << "\n";
    std::cout << " scalability [%]: " << scalability * 100 << "\n";
    std::cout << " cardinality: " << intervals.size() << "\n";
    std::cout << " domain (w/o shifting): [" << (unsigned int)domain_min << ", " << (unsigned int)domain_max << "]\n";
    domain_size = (unsigned int)(domain_max - domain_min);
    std::cout << " domain size: " << domain_size << "\n";
    std::cout << " k: " << k << "\n";
    std::cout << " -------------------------------\n\n";

    // normalization
    const unsigned int size = intervals.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        intervals[i].second.first -= domain_min;
        intervals[i].second.second -= domain_min;
    }

    // assign weight
    const unsigned int weight_max = 10000;
    std::normal_distribution<> norm(5000, 1500);
    // std::uniform_int_distribution<> rnd_weight(1, weight_max);
    for (unsigned int i = 0; i < size; ++i) 
    {
        double temp = -1;
        while (1)
        {
            if (temp >= 0) break;
            temp = norm(mt);
        }
        intervals[i].first = int(temp);
    }
}

// query making function
void make_queries()
{
    std::mt19937 mt;
    std::uniform_int_distribution<> rnd_val(1, domain_size - 1);
    for (unsigned int i = 0; i < query_size; ++i) query_set.push_back(rnd_val(mt));    
}

// output performance result
void output_result(bool f)
{
    std::string file_name = "result/";
    if (dataset_id == 0) file_name += "0_book/";
    if (dataset_id == 1) file_name += "1_BTC/";
    if (dataset_id == 2) file_name += "2_renfe/";
    if (dataset_id == 3) file_name += "3_taxi/";
    if (dataset_id == 4) file_name += "4_demo/";
    file_name += "id(" + std::to_string(dataset_id) + ")_k(" + std::to_string(k) + ")_scalability(" + std::to_string(scalability) + ").csv";

    std::ofstream file;
    file.open(file_name.c_str(), std::ios::out | std::ios::app);

    if (file.fail())
    {
        std::cerr << " cannot open the output file." << std::endl;
        file.clear();
        return;
    }

    if (f)
    {
        file    << " search time [microsec]: " << time_search << ","
                << " access size: " << result_size << "\n";
    }
    else
    {
        std::sort(time_vec.begin(), time_vec.end());
        file    << " avg. time [microsec]: " << time_search_average << ","
                << " med. time [microsec]: " << time_vec[query_size/2] << ","
                << " memory [MB]: " << mem << ","
                << " pre-processing time [msec]: " << time_preprocess << ","
                << " avg access size: " << result_size_aggregate << "\n\n";
    }

    file.close();
}

#endif
