#include "describe.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <cctype>

static std::string strip(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    s.erase(std::remove(s.begin(), s.end(), '"'),  s.end());
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    return s;
}
static std::string trim(std::string s) {
    size_t a = s.find_first_not_of(" \t\r\n\"");
    size_t b = s.find_last_not_of(" \t\r\n\"");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

static double percentile(const std::vector<double>& sorted, double p) {
    if (sorted.empty()) return 0;
    double idx = (p / 100.0) * (sorted.size() - 1);
    size_t lo  = static_cast<size_t>(idx);
    size_t hi  = lo + 1;
    if (hi >= sorted.size()) return sorted.back();
    return sorted[lo] * (1.0 - (idx - lo)) + sorted[hi] * (idx - lo);
}

DeepStats Describe::compute(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& headers,
    const std::string& column)
{
    std::string target = strip(column);
    size_t col = size_t(-1);
    for (size_t i = 0; i < headers.size(); ++i)
        if (strip(headers[i]) == target) { col = i; break; }

    if (col == size_t(-1)) {
        std::cout << "ERROR: Column '" << column
                  << "' not found for --describe\n";
        return {};
    }

    std::vector<double> vals;
    vals.reserve(data.size());
    for (const auto& row : data) {
        if (col >= row.size()) continue;
        try { vals.push_back(std::stod(trim(row[col]))); }
        catch (...) {}
    }
    if (vals.empty()) {
        std::cout << "ERROR: No numeric values in '" << column << "'\n";
        return {};
    }

    DeepStats s;
    s.count   = (long long)vals.size();
    s.sum     = std::accumulate(vals.begin(), vals.end(), 0.0);
    s.mean    = s.sum / s.count;
    s.min_val = *std::min_element(vals.begin(), vals.end());
    s.max_val = *std::max_element(vals.begin(), vals.end());

    std::vector<double> sorted = vals;
    std::sort(sorted.begin(), sorted.end());

    // Median
    if (s.count % 2 == 0)
        s.median = (sorted[s.count/2 - 1] + sorted[s.count/2]) / 2.0;
    else
        s.median = sorted[s.count / 2];

    // Percentiles
    s.p25 = percentile(sorted, 25);
    s.p75 = percentile(sorted, 75);
    s.p90 = percentile(sorted, 90);
    s.p95 = percentile(sorted, 95);
    s.p99 = percentile(sorted, 99);

    // Variance and stddev
    double sq = 0;
    for (double v : vals) sq += (v - s.mean) * (v - s.mean);
    s.variance = sq / s.count;
    s.stddev   = std::sqrt(s.variance);

    // Skewness (Fisher-Pearson)
    if (s.stddev > 0) {
        double sk = 0;
        for (double v : vals) sk += std::pow((v - s.mean) / s.stddev, 3);
        s.skewness = sk / s.count;
    }

    // Mode — most frequent value rounded to 1 decimal
    std::map<int, int> freq;
    for (double v : vals) freq[static_cast<int>(std::round(v * 10))]++;
    auto it = std::max_element(freq.begin(), freq.end(),
        [](const auto& a, const auto& b){ return a.second < b.second; });
    s.mode = it->first / 10.0;

    return s;
}

void Describe::print(const DeepStats& s, const std::string& col) {
    if (s.count == 0) return;
    const int L = 16, V = 18;
    std::cout << "\n";
    std::cout << "+------------------------------------------+\n";
    std::cout << "|  Deep Stats: " << std::left << std::setw(27) << col << "|\n";
    std::cout << "+------------------------------------------+\n";

    auto row = [&](const std::string& lbl, double v, int dec = 2) {
        std::cout << "|  " << std::left  << std::setw(L) << lbl
                  << std::right << std::setw(V) << std::fixed
                  << std::setprecision(dec) << v << "  |\n";
    };
    auto rowl = [&](const std::string& lbl, long long v) {
        std::cout << "|  " << std::left  << std::setw(L) << lbl
                  << std::right << std::setw(V) << v << "  |\n";
    };

    rowl("Count",      s.count);
    row ("Sum",        s.sum);
    row ("Mean",       s.mean);
    row ("Median",     s.median);
    row ("Mode",       s.mode, 1);
    row ("Std Dev",    s.stddev);
    row ("Variance",   s.variance);
    row ("Skewness",   s.skewness, 4);
    std::cout << "+------------------------------------------+\n";
    row ("Min",        s.min_val);
    row ("P25",        s.p25);
    row ("P75",        s.p75);
    row ("P90",        s.p90);
    row ("P95",        s.p95);
    row ("P99",        s.p99);
    row ("Max",        s.max_val);
    std::cout << "+------------------------------------------+\n\n";
}
