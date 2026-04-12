#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "thread_pool.h"

struct GroupStats {
    long long count = 0;
    double sum_total = 0.0;
    double sum_rating = 0.0;
    double min_total = 1e9;
    double max_total = 0.0;
};

class Analytics {
public:
    void run(const std::vector<std::vector<std::string>>& data,
             const std::vector<std::string>& headers,
             ThreadPool& pool,
             const std::string& group_column);

    std::unordered_map<std::string, GroupStats> get_results() const { return results; }

private:
    std::unordered_map<std::string, GroupStats> results;
    std::mutex results_mutex;
};