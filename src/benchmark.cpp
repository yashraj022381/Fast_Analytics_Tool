#include "benchmark.h"
#include "analytics.h"
#include "thread_pool.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>
#include <algorithm>

using Clock = std::chrono::high_resolution_clock;

BenchResult Benchmark::run(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& headers,
    const std::string& group_col,
    int runs,
    int n_threads)
{
    BenchResult r;
    r.runs      = runs;
    r.row_count = data.size();
    r.min_ms    = std::numeric_limits<long long>::max();
    r.max_ms    = 0;
    long long total = 0;

    std::cout << "\nBenchmark: " << runs << " runs, "
              << n_threads << " threads, "
              << data.size() << " rows\n";

    for (int i = 0; i < runs; ++i) {
        auto t0 = Clock::now();
        Analytics a;
        {
            ThreadPool pool(static_cast<size_t>(n_threads));
            a.run(data, headers, pool, group_col);
        }
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      Clock::now() - t0).count();
        r.min_ms = std::min(r.min_ms, (long long)ms);
        r.max_ms = std::max(r.max_ms, (long long)ms);
        total   += ms;
        std::cout << "  Run " << (i+1) << "/" << runs << ": " << ms << " ms\n";
    }

    r.avg_ms     = (double)total / runs;
    r.rows_per_s = r.avg_ms > 0
                 ? data.size() / (r.avg_ms / 1000.0) : 0;
    return r;
}

void Benchmark::print(const BenchResult& r,
                      const std::string& group_col,
                      int n_threads)
{
    std::cout << "\n";
    std::cout << "+-------------------------------------------+\n";
    std::cout << "|          Benchmark Results                |\n";
    std::cout << "+-------------------------------------------+\n";
    std::cout << "|  Column   : " << std::left << std::setw(29)
              << group_col << "|\n";
    std::cout << "|  Threads  : " << std::left << std::setw(29)
              << n_threads << "|\n";
    std::cout << "|  Rows     : " << std::left << std::setw(29)
              << r.row_count << "|\n";
    std::cout << "|  Runs     : " << std::left << std::setw(29)
              << r.runs << "|\n";
    std::cout << "+-------------------------------------------+\n";
    std::cout << "|  Min      : " << std::left << std::setw(26)
              << (std::to_string(r.min_ms) + " ms") << "   |\n";
    std::cout << "|  Max      : " << std::left << std::setw(26)
              << (std::to_string(r.max_ms) + " ms") << "   |\n";
    std::cout << "|  Average  : " << std::left << std::setw(26)
              << (std::to_string((int)r.avg_ms) + " ms") << "   |\n";
    std::cout << "|  rows/sec : " << std::left << std::setw(26)
              << (std::to_string((long long)r.rows_per_s)) << "   |\n";
    std::cout << "+-------------------------------------------+\n\n";
    std::cout << "Resume line:\n";
    std::cout << "  Processed " << r.row_count << " rows in avg "
              << (int)r.avg_ms << " ms using " << n_threads
              << " threads (" << (long long)r.rows_per_s
              << " rows/sec)\n\n";
}
