#pragma once
// describe.h — Feature 2: --describe flag
// Deep statistical analysis on any numeric column.
// Outputs: count, sum, mean, median, mode, stddev,
//          variance, skewness, min, P25, P75, P90, P95, P99, max

#include <string>
#include <vector>

struct DeepStats {
    long long count    = 0;
    double sum         = 0;
    double mean        = 0;
    double median      = 0;
    double mode        = 0;
    double stddev      = 0;
    double variance    = 0;
    double skewness    = 0;
    double p25         = 0;
    double p75         = 0;
    double p90         = 0;
    double p95         = 0;
    double p99         = 0;
    double min_val     = 0;
    double max_val     = 0;
};

class Describe {
public:
    static DeepStats compute(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& headers,
        const std::string& column);

    static void print(const DeepStats& s, const std::string& col);
};
