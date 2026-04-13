#include "analytics.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <atomic>
#include <thread>

// Strip ALL spaces + quotes from a string (for column name matching)
static std::string strip(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    s.erase(std::remove(s.begin(), s.end(), '"'),  s.end());
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    return s;
}

// Trim only leading/trailing whitespace and quotes (for cell values)
static std::string trim(std::string s) {
    size_t a = s.find_first_not_of(" \t\r\n\"");
    size_t b = s.find_last_not_of(" \t\r\n\"");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

void Analytics::run(const std::vector<std::vector<std::string>>& data,
                    const std::vector<std::string>& headers,
                    ThreadPool& pool,
                    const std::string& group_column)
{
    auto t0 = std::chrono::high_resolution_clock::now();

    // Strip the requested column name exactly like we strip headers
    std::string target = strip(group_column);

    // Find all required column indices
    size_t group_col  = size_t(-1);
    size_t total_col  = size_t(-1);
    size_t rating_col = size_t(-1);

    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = strip(headers[i]);
        if (h == target)   group_col  = i;
        if (h == "Total")  total_col  = i;
        if (h == "Rating") rating_col = i;
    }

    if (group_col == size_t(-1)) {
        std::cout << "ERROR: Column '" << target << "' not found.\n";
        std::cout << "Run with --columns to see exact column names.\n";
        return;
    }
    if (total_col == size_t(-1) || rating_col == size_t(-1)) {
        std::cout << "ERROR: 'Total' or 'Rating' column not found.\n";
        return;
    }

    std::cout << "Processing " << data.size()
              << " rows with 4 threads...\n";

    size_t total_rows = data.size();
    size_t n_threads  = 4;
    size_t chunk      = (total_rows + n_threads - 1) / n_threads;

    for (size_t t = 0; t < n_threads; ++t) {
        size_t row_start = t * chunk;
        size_t row_end   = std::min(row_start + chunk, total_rows);
        if (row_start >= total_rows) break;

        pool.enqueue([this, &data, row_start, row_end,
                      group_col, total_col, rating_col]()
        {
            std::unordered_map<std::string, GroupStats> local;

            for (size_t r = row_start; r < row_end; ++r) {
                const auto& row = data[r];
                if (row.size() <= group_col)  continue;
                if (row.size() <= total_col)  continue;
                if (row.size() <= rating_col) continue;

                std::string group = trim(row[group_col]);
                if (group.empty()) continue;

                try {
                    double tv = std::stod(trim(row[total_col]));
                    double rv = std::stod(trim(row[rating_col]));

                    auto& s = local[group];
                    s.count++;
                    s.sum_total  += tv;
                    s.sum_rating += rv;
                    s.min_total   = std::min(s.min_total, tv);
                    s.max_total   = std::max(s.max_total, tv);
                } catch (...) {}
            }

            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& [k, v] : local) {
                auto& g = results[k];
                g.count      += v.count;
                g.sum_total  += v.sum_total;
                g.sum_rating += v.sum_rating;
                g.min_total   = std::min(g.min_total,  v.min_total);
                g.max_total   = std::max(g.max_total,  v.max_total);
            }
        });
    }

    pool.wait_all();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - t0).count();

    std::cout << "Done! Found " << results.size()
              << " groups in " << ms << " ms\n";
}














