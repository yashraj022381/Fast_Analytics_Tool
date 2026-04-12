#include "analytics.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <atomic>
#include <thread>

void Analytics::run(const std::vector<std::vector<std::string>>& data,
                    const std::vector<std::string>& headers,
                    ThreadPool& pool,
                    const std::string& group_column) {
    
    std::cout << "Grouping by: " << group_column << "\n";

    // Column detection (robust)
    size_t group_col = -1, total_col = -1, rating_col = -1;
    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        h.erase(std::remove(h.begin(), h.end(), '"'), h.end());
        h.erase(std::remove_if(h.begin(), h.end(), ::isspace), h.end());

        if (h == group_column || h == "Productline") group_col = i;
        if (h == "Total") total_col = i;
        if (h == "Rating") rating_col = i;
    }

    if (group_col == size_t(-1) || total_col == size_t(-1) || rating_col == size_t(-1)) {
        std::cout << "❌ Error: Could not find required columns!\n";
        return;
    }

    std::cout << "Processing " << data.size() << " rows with 4 threads...\n";

    std::atomic<int> processed(0);
    const int total = static_cast<int>(data.size());
    const int update_every = total / 30;   // Update ~30 times

    size_t chunk_size = data.size() / 4 + 1;

    for (size_t start = 0; start < data.size(); start += chunk_size) {
        size_t end = std::min(start + chunk_size, data.size());

        pool.enqueue([this, &data, start, end, group_col, total_col, rating_col, &processed, total, update_every]() {
            std::unordered_map<std::string, GroupStats> local;

            for (size_t r = start; r < end; ++r) {
                const auto& row = data[r];
                if (group_col >= row.size() || total_col >= row.size() || rating_col >= row.size()) continue;

                std::string group = row[group_col];
                group.erase(std::remove(group.begin(), group.end(), '"'), group.end());

                try {
                    double total_val = std::stod(row[total_col]);
                    double rating = std::stod(row[rating_col]);

                    auto& stats = local[group];
                    stats.count++;
                    stats.sum_total += total_val;
                    stats.sum_rating += rating;
                    stats.min_total = std::min(stats.min_total, total_val);
                    stats.max_total = std::max(stats.max_total, total_val);
                } catch (...) {}

                processed++;
            }

            // Merge
            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& p : local) {
                auto& g = results[p.first];
                g.count += p.second.count;
                g.sum_total += p.second.sum_total;
                g.sum_rating += p.second.sum_rating;
                g.min_total = std::min(g.min_total, p.second.min_total);
                g.max_total = std::max(g.max_total, p.second.max_total);
            }
        });
    }

    pool.wait_all();

    // Final clean message
    std::cout << "\n✅ Processing completed successfully! (" << total << " rows)\n";
}
















/*#include "analytics.h"
#include <iostream>
#include <algorithm>
#include <cctype>


void Analytics::run(const std::vector<std::vector<std::string>>& data,
                    const std::vector<std::string>& headers,
                    ThreadPool& pool,
                    const std::string& group_column) {
    
    std::cout << "Grouping by: " << group_column << "\n";

    // Column detection (same robust logic as before)
    size_t group_col = -1, total_col = -1, rating_col = -1;
    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        h.erase(std::remove(h.begin(), h.end(), '"'), h.end());
        h.erase(std::remove_if(h.begin(), h.end(), ::isspace), h.end());

        if (h == group_column || h == "Productline") group_col = i;
        if (h == "Total") total_col = i;
        if (h == "Rating") rating_col = i;
    }

    if (group_col == size_t(-1) || total_col == size_t(-1) || rating_col == size_t(-1)) {
        std::cout << "Error: Could not find required columns!\n";
        return;
    }

    std::cout << "Processing " << data.size() << " rows with 4 threads...\n";

    size_t chunk_size = data.size() / 4 + 1;
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, data.size());
        pool.enqueue([this, &data, i, end, group_col, total_col, rating_col]() {
            // ... (same local map logic as before)
            std::unordered_map<std::string, GroupStats> local;
            for (size_t row = i; row < end; ++row) {
                const auto& r = data[row];
                if (group_col >= r.size() || total_col >= r.size() || rating_col >= r.size()) continue;

                std::string group = r[group_col];
                group.erase(std::remove(group.begin(), group.end(), '"'), group.end());

                try {
                    double total = std::stod(r[total_col]);
                    double rating = std::stod(r[rating_col]);

                    auto& stats = local[group];
                    stats.count++;
                    stats.sum_total += total;
                    stats.sum_rating += rating;
                    stats.min_total = std::min(stats.min_total, total);
                    stats.max_total = std::max(stats.max_total, total);
                } catch (...) {}
            }

            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& p : local) {
                auto& g = results[p.first];
                g.count += p.second.count;
                g.sum_total += p.second.sum_total;
                g.sum_rating += p.second.sum_rating;
                g.min_total = std::min(g.min_total, p.second.min_total);
                g.max_total = std::max(g.max_total, p.second.max_total);
            }
        });
    }

    pool.wait_all();   // Important: Wait for all threads
    std::cout << "✅ Processing completed!\n";
}*/




















/*#include "analytics.h"
#include <iostream>
#include <algorithm>
#include <cctype>

void Analytics::run(const std::vector<std::vector<std::string>>& data,
                    const std::vector<std::string>& headers,
                    ThreadPool& pool,
                    const std::string& group_column) {
    
    std::cout << "Grouping by: " << group_column << "\n";

    // Robust column detection
    size_t group_col = -1, total_col = -1, rating_col = -1;

    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        
        // Clean the header
        h.erase(std::remove(h.begin(), h.end(), '"'), h.end());
        h.erase(std::remove(h.begin(), h.end(), '\''), h.end());
        h.erase(std::remove_if(h.begin(), h.end(), ::isspace), h.end());

        std::string clean_group = group_column;
        clean_group.erase(std::remove_if(clean_group.begin(), clean_group.end(), ::isspace), clean_group.end());

        if (h == clean_group || h == "Productline" || h == "ProductLine") group_col = i;
        if (h == "Total" || h == "TotalSales" || h == "TotalRevenue") total_col = i;
        if (h == "Rating" || h == "rating") rating_col = i;

        std::cout << "Column " << i << ": " << headers[i] << "\n";  // Debug
    }

    std::cout << "Found group_col=" << group_col 
              << ", total_col=" << total_col 
              << ", rating_col=" << rating_col << "\n";

    if (group_col == size_t(-1) || total_col == size_t(-1) || rating_col == size_t(-1)) {
        std::cout << "❌ Error: Could not find required columns!\n";
        std::cout << "Try --columns to see available columns.\n";
        return;
    }

    std::cout << "✅ Columns found. Processing " << data.size() << " rows with 4 threads...\n";

    // Rest of the function remains same (multithreading logic)
    size_t chunk_size = data.size() / 4 + 1;
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, data.size());
        pool.enqueue([this, &data, i, end, group_col, total_col, rating_col]() {
            std::unordered_map<std::string, GroupStats> local;
            for (size_t row = i; row < end; ++row) {
                const auto& r = data[row];
                if (group_col >= r.size() || total_col >= r.size() || rating_col >= r.size()) continue;

                std::string group = r[group_col];
                group.erase(std::remove(group.begin(), group.end(), '"'), group.end());
                group.erase(std::remove_if(group.begin(), group.end(), ::isspace), group.end());

                try {
                    double total = std::stod(r[total_col]);
                    double rating = std::stod(r[rating_col]);

                    auto& stats = local[group];
                    stats.count++;
                    stats.sum_total += total;
                    stats.sum_rating += rating;
                    stats.min_total = std::min(stats.min_total, total);
                    stats.max_total = std::max(stats.max_total, total);
                } catch (...) {}
            }

            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& p : local) {
                auto& g = results[p.first];
                g.count += p.second.count;
                g.sum_total += p.second.sum_total;
                g.sum_rating += p.second.sum_rating;
                g.min_total = std::min(g.min_total, p.second.min_total);
                g.max_total = std::max(g.max_total, p.second.max_total);
            }
        });
    }

    std::cout << "✅ Successfully processed data!\n";
}
*/




















// analytics.cpp — COMPLETE FIXED FILE
/*#include "analytics.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>

void Analytics::run(const std::vector<std::vector<std::string>>& data,
                    const std::vector<std::string>& headers,
                    ThreadPool& pool,
                    const std::string& group_column)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Strip spaces and quotes from the group_column input too
    // so "--group Gender" matches " Gender" in the CSV header
    std::string clean_group = group_column;
    clean_group.erase(remove(clean_group.begin(), clean_group.end(), ' '), clean_group.end());
    clean_group.erase(remove(clean_group.begin(), clean_group.end(), '"'), clean_group.end());

    std::cout << "Grouping by: " << clean_group << "\n";

    size_t group_col = size_t(-1);
    size_t total_col = size_t(-1);
    size_t rating_col = size_t(-1);

    // Print all headers so you can see exactly what is available
    std::cout << "Available columns: ";
    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        h.erase(remove(h.begin(), h.end(), ' '), h.end());
        h.erase(remove(h.begin(), h.end(), '"'), h.end());
        std::cout << "[" << h << "] ";

        if (h == clean_group) group_col  = i;
        if (h == "Total")     total_col  = i;
        if (h == "Rating")    rating_col = i;
    }
    std::cout << "\n";

    if (group_col == size_t(-1)) {
        std::cout << "ERROR: Column '" << clean_group << "' not found in CSV!\n";
        std::cout << "Tip: run with --columns to list all column names.\n";
        return;
    }
    if (total_col == size_t(-1) || rating_col == size_t(-1)) {
        std::cout << "ERROR: Could not find 'Total' or 'Rating' columns!\n";
        return;
    }

    std::cout << "Processing " << data.size() << " rows with 4 threads...\n";

    // Split data into 4 chunks and enqueue each chunk
    size_t chunk_size = data.size() / 4 + 1;
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, data.size());

        pool.enqueue([this, &data, i, end, group_col, total_col, rating_col]() {
            // Each thread builds its OWN local map — no lock needed here
            std::unordered_map<std::string, GroupStats> local;

            for (size_t row = i; row < end; ++row) {
                const auto& r = data[row];
                if (group_col < r.size() && total_col < r.size() && rating_col < r.size()) {
                    std::string group = r[group_col];
                    group.erase(remove(group.begin(), group.end(), '"'), group.end());
                    group.erase(remove(group.begin(), group.end(), ' '), group.end());

                    try {
                        double total  = std::stod(r[total_col]);
                        double rating = std::stod(r[rating_col]);

                        auto& stats = local[group];
                        stats.count++;
                        stats.sum_total  += total;
                        stats.sum_rating += rating;
                        stats.min_total   = std::min(stats.min_total, total);
                        stats.max_total   = std::max(stats.max_total, total);
                    } catch (...) {}
                }
            }

            // Merge local into shared results — lock only for the merge step
            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& p : local) {
                auto& g = results[p.first];
                g.count      += p.second.count;
                g.sum_total  += p.second.sum_total;
                g.sum_rating += p.second.sum_rating;
                g.min_total   = std::min(g.min_total,  p.second.min_total);
                g.max_total   = std::max(g.max_total,  p.second.max_total);
            }
        });
    }

    // *** THE CRITICAL FIX ***
    // Wait here until ALL 4 threads have finished their chunks
    // Without this, main() reads empty results immediately after run() returns
    pool.wait_all();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "Done! Found " << results.size()
              << " groups in " << ms << " ms\n";
}*/





























/*#include "analytics.h"
#include <iostream>
#include <chrono>

void Analytics::run(const std::vector<std::vector<std::string>>& data,
                    const std::vector<std::string>& headers,
                    ThreadPool& pool,
                    const std::string& group_column) {
    
    std::cout << "Grouping by: " << group_column << "\n";

    size_t group_col = -1, total_col = -1, rating_col = -1;
    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        h.erase(remove(h.begin(), h.end(), ' '), h.end());
        h.erase(remove(h.begin(), h.end(), '"'), h.end());

        if (h == group_column) group_col = i;
        if (h == "Total") total_col = i;
        if (h == "Rating") rating_col = i;
    }

    if (group_col == size_t(-1) || total_col == size_t(-1) || rating_col == size_t(-1)) {
        std::cout << "Error: Could not find required columns!\n";
        return;
    }

    std::cout << "Processing " << data.size() << " rows...\n";

    size_t chunk_size = data.size() / 4 + 1;
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, data.size());
        pool.enqueue([this, &data, i, end, group_col, total_col, rating_col]() {
            std::unordered_map<std::string, GroupStats> local;
            for (size_t row = i; row < end; ++row) {
                const auto& r = data[row];
                if (group_col < r.size() && total_col < r.size() && rating_col < r.size()) {
                    std::string group = r[group_col];
                    group.erase(remove(group.begin(), group.end(), '"'), group.end());

                    try {
                        double total = std::stod(r[total_col]);
                        double rating = std::stod(r[rating_col]);

                        auto& stats = local[group];
                        stats.count++;
                        stats.sum_total += total;
                        stats.sum_rating += rating;
                        stats.min_total = std::min(stats.min_total, total);
                        stats.max_total = std::max(stats.max_total, total);
                    } catch (...) {}
                }
            }
            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto& p : local) {
                auto& g = results[p.first];
                g.count += p.second.count;
                g.sum_total += p.second.sum_total;
                g.sum_rating += p.second.sum_rating;
                g.min_total = std::min(g.min_total, p.second.min_total);
                g.max_total = std::max(g.max_total, p.second.max_total);
            }
        });
    }

    std::cout << "Successfully processed data!\n";
}*/