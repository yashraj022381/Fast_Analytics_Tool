#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>

#include "csv_parser.h"
#include "analytics.h"
#include "thread_pool.h"
#include "exporter.h"

void print_banner() {
    std::cout << "\n======================================================\n";
    std::cout << "       Fast Data Analytics Tool  (Modern C++20)       \n";
    std::cout << "======================================================\n\n";
}

void show_progress(int current, int total) {
    int percent = (current * 100) / total;
    std::cout << "\r[";
    for (int i = 0; i < 50; ++i) std::cout << (i < percent/2 ? "█" : "░");
    std::cout << "] " << percent << "% (" << current << "/" << total << ")" << std::flush;
}

int main(int argc, char* argv[]) {
    print_banner();

    std::string input_file = "data/sample.csv";
    std::string group_by = "City";
    int top_n = 0;   // 0 = show all

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--group" && i+1 < argc) group_by = argv[++i];
        if (arg == "--top"   && i+1 < argc) top_n    = std::stoi(argv[++i]);
        if (arg == "--input" && i+1 < argc) input_file = argv[++i];
    }

    // Load data
    CsvParser parser;
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> headers;

    if (!parser.read(input_file, data, headers)) {
        std::cout << "Error: Cannot load " << input_file << "\n";
        std::cout << "Make sure sample.csv is in data/ folder.\n";
        return 1;
    }

    std::cout << "Loaded " << data.size() << " rows successfully.\n";

    // Analytics with progress
    ThreadPool pool(4);
    Analytics analytics;
    analytics.run(data, headers, pool, group_by);

    auto results = analytics.get_results();

    // Apply Top N
    size_t total_groups = results.size();
    if (top_n > 0 && !results.empty()) {
        std::vector<std::pair<std::string, GroupStats>> sorted(results.begin(), results.end());
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
            return a.second.sum_total > b.second.sum_total;
        });
        results.clear();
        for (int i = 0; i < std::min(top_n, (int)sorted.size()); ++i) {
            results[sorted[i].first] = sorted[i].second;
        }
    }

    // Export
    Exporter exporter;
    exporter.save_json(results, group_by, "results.json");
    exporter.save_csv(results, group_by, "results.csv");
    exporter.save_html(results, group_by, "results.html");

    // Final Summary
    std::cout << "\n=== Analysis Completed Successfully ===\n";
    std::cout << "Grouped by : " << group_by << "\n";
    std::cout << "Total groups found : " << total_groups << "\n";
    std::cout << "Showing : " << results.size() << " groups\n";
    if (top_n > 0 && (int)total_groups > top_n)
        std::cout << "Note: Only top " << top_n << " groups shown (out of " << total_groups << ")\n";
    std::cout << "\nFiles saved: results.csv, results.json, results.html\n";
    std::cout << "Open results.html in browser for beautiful report!\n";

    return 0;
}








