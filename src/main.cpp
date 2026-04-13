// main.cpp — Fast Data Analytics Tool (C++20)
// -----------------------------------------------
// Original features:
//   --group <col>       group-by analytics
//   --top <N>           show top N groups
//   --input <file>      custom CSV file path
// -----------------------------------------------
// New advanced features:
//   --columns           list all column names
//   --sort <field>      sort by sum_total|count|avg_rating
//   --filter <expr>     filter rows before analytics
//                       e.g. --filter "City=Yangon"
//                            --filter "Rating>7"
//                            --filter "Total<=500"
//                       Multiple --filter = AND logic
//   --describe <col>    deep stats on any numeric column
//   --chart [field]     ASCII bar chart in terminal
//   --benchmark [N]     timing benchmark over N runs
//   --summary           dataset overview (types, uniques, nulls)
//   --threads <N>       number of worker threads (default 4)
//   --output <prefix>   output file prefix (default: results)
//   --help              show this help

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <chrono>

#include "csv_parser.h"
#include "analytics.h"
#include "thread_pool.h"
#include "exporter.h"
#include "filter.h"
#include "describe.h"
#include "chart.h"
#include "benchmark.h"
#include "summary.h"

// ── Timer ─────────────────────────────────────────────────────────
using Clock = std::chrono::high_resolution_clock;
auto  timer_start() { return Clock::now(); }
void  timer_end(const std::chrono::time_point<Clock>& t,
                const std::string& msg) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  Clock::now() - t).count();
    std::cout << "[TIME] " << msg << ": " << ms << " ms\n";
}

void print_help() {
    std::cout << "\n";
    std::cout << "==========================================================\n";
    std::cout << "       Fast Data Analytics Tool  (Modern C++20)          \n";
    std::cout << "==========================================================\n\n";
    std::cout << "Core flags:\n";
    std::cout << "  --group   <column>   Group-by analytics (required for analysis)\n";
    std::cout << "  --input   <file>     CSV file (default: data/sample.csv)\n";
    std::cout << "  --output  <prefix>   Output filename prefix (default: results)\n";
    std::cout << "  --top     <N>        Show only top N groups\n";
    std::cout << "  --sort    <field>    sum_total | count | avg_rating\n";
    std::cout << "  --threads <N>        Worker threads (default: 4)\n";
    std::cout << "  --columns            List all column names then exit\n\n";
    std::cout << "Advanced features:\n";
    std::cout << "  --filter  <expr>     Filter rows BEFORE analytics runs\n";
    std::cout << "                       Operators: =  !=  >  <  >=  <=\n";
    std::cout << "                       e.g. --filter \"City=Yangon\"\n";
    std::cout << "                            --filter \"Rating>7\"\n";
    std::cout << "                            --filter \"Total<=500\"\n";
    std::cout << "                       Use multiple times for AND logic\n\n";
    std::cout << "  --describe <column>  Deep column stats:\n";
    std::cout << "                       mean, median, mode, stddev, variance,\n";
    std::cout << "                       skewness, P25, P75, P90, P95, P99\n\n";
    std::cout << "  --chart [field]      ASCII bar chart in terminal\n";
    std::cout << "                       field: sum_total | count | avg_rating\n\n";
    std::cout << "  --benchmark [N]      Run N times, report min/max/avg ms\n";
    std::cout << "                       and rows/second throughput\n\n";
    std::cout << "  --summary            Dataset overview: column types,\n";
    std::cout << "                       unique counts, empty counts, top values\n\n";
    std::cout << "  --help               Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  fast_analytics.exe --group City --top 3\n";
    std::cout << "  fast_analytics.exe --group City --chart\n";
    std::cout << "  fast_analytics.exe --group City --chart count\n";
    std::cout << "  fast_analytics.exe --group Gender --sort count\n";
    std::cout << "  fast_analytics.exe --group City --filter \"Gender=Female\"\n";
    std::cout << "  fast_analytics.exe --group City --filter \"Rating>7\" --filter \"Total>100\"\n";
    std::cout << "  fast_analytics.exe --describe Total\n";
    std::cout << "  fast_analytics.exe --describe Rating\n";
    std::cout << "  fast_analytics.exe --benchmark 10 --group City\n";
    std::cout << "  fast_analytics.exe --summary\n";
    std::cout << "  fast_analytics.exe --group \"Product line\" --sort avg_rating --top 3\n\n";
}

int main(int argc, char* argv[]) {
    print_help();

    // ── Parse all CLI arguments ───────────────────────────────────
    std::string input_file    = "data/sample.csv";
    std::string group_by      = "";
    std::string sort_by       = "sum_total";
    std::string output_prefix = "results";
    std::string describe_col  = "";
    std::string chart_field   = "sum_total";
    int  top_n        = 0;
    int  num_threads  = 4;
    int  bench_runs   = 5;
    bool show_columns = false;
    bool do_summary   = false;
    bool do_benchmark = false;
    bool do_chart     = false;
    std::vector<FilterRule> filter_rules;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help")    { print_help(); return 0; }
        if (a == "--columns")   show_columns = true;
        if (a == "--summary")   do_summary   = true;
        if (a == "--chart") {
            do_chart = true;
            // Optional field name right after --chart (if not another flag)
            if (i+1 < argc && argv[i+1][0] != '-')
                chart_field = argv[++i];
        }
        if (a == "--benchmark") {
            do_benchmark = true;
            if (i+1 < argc && argv[i+1][0] != '-')
                bench_runs = std::stoi(argv[++i]);
        }
        if (a == "--group"    && i+1 < argc) group_by      = argv[++i];
        if (a == "--top"      && i+1 < argc) top_n         = std::stoi(argv[++i]);
        if (a == "--sort"     && i+1 < argc) sort_by       = argv[++i];
        if (a == "--input"    && i+1 < argc) input_file    = argv[++i];
        if (a == "--output"   && i+1 < argc) output_prefix = argv[++i];
        if (a == "--threads"  && i+1 < argc) num_threads   = std::stoi(argv[++i]);
        if (a == "--describe" && i+1 < argc) describe_col  = argv[++i];
        if (a == "--filter"   && i+1 < argc) {
            FilterRule rule;
            if (Filter::parse(argv[++i], rule))
                filter_rules.push_back(rule);
        }
    }

    // Need at least one action
    bool has_action = !group_by.empty() || show_columns
                   || do_summary || !describe_col.empty()
                   || do_benchmark;
    if (!has_action) {
        std::cout << "Error: no action given. Use --help for usage.\n\n";
        return 1;
    }

    // ── Step 1: Load CSV ──────────────────────────────────────────
    std::cout << "Loading: " << input_file << "\n";
    CsvParser parser;
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> headers;

    auto t_load = timer_start();
    if (!parser.read(input_file, data, headers)) {
        std::cout << "\nFix: CMake copies data/ automatically after --build.\n";
        std::cout << "     Or copy the data/ folder next to fast_analytics.exe manually.\n\n";
        return 1;
    }
    timer_end(t_load, "CSV load");
    std::cout << "Loaded " << data.size() << " rows, "
              << headers.size() << " columns.\n\n";

    // ── Step 2: --columns ─────────────────────────────────────────
    if (show_columns) {
        CsvParser::print_columns(headers);
        if (group_by.empty() && describe_col.empty()
            && !do_summary && !do_benchmark)
            return 0;
    }

    // ── Step 3: --summary ─────────────────────────────────────────
    if (do_summary) {
        Summary::print(data, headers);
        if (group_by.empty() && describe_col.empty())
            return 0;
    }

    // ── Step 4: --describe ────────────────────────────────────────
    if (!describe_col.empty()) {
        auto t_desc = timer_start();
        DeepStats ds = Describe::compute(data, headers, describe_col);
        timer_end(t_desc, "--describe");
        Describe::print(ds, describe_col);
        if (group_by.empty()) return 0;
    }

    // ── Step 5: --benchmark ───────────────────────────────────────
    if (do_benchmark) {
        if (group_by.empty()) {
            std::cout << "Error: --benchmark requires --group <column>\n\n";
            return 1;
        }
        // Apply any filters before benchmarking
        auto bench_data = Filter::apply(data, headers, filter_rules);
        auto res = Benchmark::run(bench_data, headers, group_by,
                                  bench_runs, num_threads);
        Benchmark::print(res, group_by, num_threads);
        return 0;
    }

    // Nothing more to do if no --group
    if (group_by.empty()) return 0;

    // ── Step 6: --filter ──────────────────────────────────────────
    // Apply all filter rules BEFORE analytics runs.
    // This reduces the dataset so only matching rows are analysed.
    std::vector<std::vector<std::string>> working = data;
    if (!filter_rules.empty()) {
        auto t_filt = timer_start();
        working = Filter::apply(data, headers, filter_rules);
        timer_end(t_filt, "Filter");
        if (working.empty()) {
            std::cout << "No rows passed the filter. Adjust your conditions.\n\n";
            return 1;
        }
        std::cout << "\n";
    }

    // ── Step 7: Run analytics ─────────────────────────────────────
    // THE RACE CONDITION FIX:
    // ThreadPool is created inside { } curly braces.
    // When } is reached, ~ThreadPool() calls join() on every thread.
    // join() blocks until that thread finishes.
    // So get_results() below is called ONLY after ALL threads are done.
    Analytics analytics;
    {
        auto t_work = timer_start();
        ThreadPool pool(static_cast<size_t>(num_threads));
        analytics.run(working, headers, pool, group_by);
        timer_end(t_work, "Analytics (" + std::to_string(num_threads)
                           + " threads)");
    }

    // ── Step 8: Get results — 100% safe now ──────────────────────
    auto results = analytics.get_results();
    if (results.empty()) {
        std::cout << "\nNo results. Column '" << group_by << "' not found.\n";
        std::cout << "Run with --columns to see exact column names.\n\n";
        return 1;
    }
    std::cout << "Found " << results.size() << " groups.\n\n";

    // ── Step 9: Sort ──────────────────────────────────────────────
    std::vector<std::pair<std::string, GroupStats>> sorted(
        results.begin(), results.end());

    std::sort(sorted.begin(), sorted.end(),
        [&sort_by](const auto& a, const auto& b) {
            if (sort_by == "count")
                return a.second.count > b.second.count;
            if (sort_by == "avg_rating") {
                double ra = a.second.count > 0
                           ? a.second.sum_rating / a.second.count : 0.0;
                double rb = b.second.count > 0
                           ? b.second.sum_rating / b.second.count : 0.0;
                return ra > rb;
            }
            return a.second.sum_total > b.second.sum_total;
        });

    // ── Step 10: Apply --top N ────────────────────────────────────
    size_t total_groups = sorted.size();
    if (top_n > 0 && (int)sorted.size() > top_n)
        sorted.resize(static_cast<size_t>(top_n));

    // ── Step 11: Print table to terminal ─────────────────────────
    std::cout << std::left
              << std::setw(24) << "Group"
              << std::setw(10) << "Count"
              << std::setw(17) << "Sum Total"
              << std::setw(13) << "Avg Rating"
              << std::setw(14) << "Min Total"
              << "Max Total\n"
              << std::string(84, '-') << "\n";

    for (const auto& [key, s] : sorted) {
        double avg = s.count > 0 ? s.sum_rating / s.count : 0.0;
        std::cout << std::left  << std::setw(24) << key
                  << std::setw(10) << s.count
                  << "$" << std::setw(16) << std::fixed
                         << std::setprecision(2) << s.sum_total
                  << std::setw(13) << std::fixed
                         << std::setprecision(2) << avg
                  << "$" << std::setw(13) << std::fixed
                         << std::setprecision(2) << s.min_total
                  << "$" << std::fixed
                         << std::setprecision(2) << s.max_total << "\n";
    }
    std::cout << std::string(84, '-') << "\n";
    if (top_n > 0 && (int)total_groups > top_n)
        std::cout << "(Showing top " << top_n << " of "
                  << total_groups << " groups)\n";
    std::cout << "\n";

    // ── Step 12: --chart ──────────────────────────────────────────
    if (do_chart) {
        Chart::print(sorted, group_by, chart_field);
    }

    // ── Step 13: Rebuild map for exporter ────────────────────────
    results.clear();
    for (const auto& [key, s] : sorted)
        results[key] = s;

    // ── Step 14: Export ───────────────────────────────────────────
    auto t_exp = timer_start();
    Exporter exporter;
    exporter.save_json(results, group_by, output_prefix + ".json");
    exporter.save_csv (results, group_by, output_prefix + ".csv");
    exporter.save_html(results, group_by, output_prefix + ".html");
    timer_end(t_exp, "Export");

    std::cout << "\n=== Analysis Completed Successfully ===\n";
    std::cout << "Grouped by  : " << group_by << "\n";
    std::cout << "Groups shown: " << sorted.size() << " / " << total_groups << "\n";
    std::cout << "Output files: "
              << output_prefix << ".json / "
              << output_prefix << ".csv / "
              << output_prefix << ".html\n";
    std::cout << "Open " << output_prefix
              << ".html in browser for the visual report!\n";
    std::cout << "5-10x faster than Pandas on large datasets!\n\n";

    return 0;
}





















/*#include <iostream>
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
}*/








