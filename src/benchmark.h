#pragma once
// benchmark.h — Feature 4: --benchmark flag
// Runs the same analytics job N times and reports:
//   min time, max time, average time, throughput (rows/second)
// Great for your resume: shows concrete performance numbers.

#include <string>
#include <vector>

struct BenchResult {
    long long min_ms     = 0;
    long long max_ms     = 0;
    double    avg_ms     = 0;
    double    rows_per_s = 0;
    int       runs       = 0;
    size_t    row_count  = 0;
};

class Benchmark {
public:
    static BenchResult run(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& headers,
        const std::string& group_col,
        int runs       = 5,
        int n_threads  = 4);

    static void print(const BenchResult& r,
                      const std::string& group_col,
                      int n_threads);
};
