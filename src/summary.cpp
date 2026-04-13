#include "summary.h"
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <numeric>
#include <cmath>
#include <algorithm>
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

void Summary::print(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& headers)
{
    size_t nrows = data.size();
    size_t ncols = headers.size();

    std::cout << "\n";
    std::cout << "+==========================================================+\n";
    std::cout << "|              Dataset Summary                             |\n";
    std::cout << "+==========================================================+\n";
    std::cout << "|  Rows    : " << std::left << std::setw(47) << nrows << "|\n";
    std::cout << "|  Columns : " << std::left << std::setw(47) << ncols << "|\n";
    std::cout << "+==========================================================+\n\n";

    // Header row for column table
    std::cout << std::left
              << std::setw(22) << "Column"
              << std::setw(10) << "Type"
              << std::setw(10) << "NonEmpty"
              << std::setw(8)  << "Empty"
              << std::setw(10) << "Unique"
              << "Details\n"
              << std::string(78, '-') << "\n";

    for (size_t c = 0; c < ncols; ++c) {
        std::string col_name = strip(headers[c]);

        std::set<std::string>        unique_vals;
        std::map<std::string, size_t> freq;
        std::vector<double>           nums;
        size_t non_empty = 0, empty = 0;

        for (const auto& row : data) {
            if (c >= row.size()) { empty++; continue; }
            std::string v = trim(row[c]);
            if (v.empty()) { empty++; continue; }
            non_empty++;
            unique_vals.insert(v);
            freq[v]++;
            try { nums.push_back(std::stod(v)); } catch (...) {}
        }

        // Decide type: if >80% of non-empty values parse as numbers → numeric
        bool is_numeric = (non_empty > 0) &&
                          ((double)nums.size() / non_empty > 0.8);

        std::string type_str = is_numeric ? "numeric" : "text";

        // Top value (most frequent)
        std::string top_val;
        size_t      top_cnt = 0;
        if (!freq.empty()) {
            auto it = std::max_element(freq.begin(), freq.end(),
                [](const auto& a, const auto& b){
                    return a.second < b.second;
                });
            top_val = it->first;
            top_cnt = it->second;
        }

        // Truncate long column names
        std::string display_name = col_name;
        if (display_name.size() > 20)
            display_name = display_name.substr(0, 17) + "...";

        std::cout << std::left
                  << std::setw(22) << display_name
                  << std::setw(10) << type_str
                  << std::setw(10) << non_empty
                  << std::setw(8)  << empty
                  << std::setw(10) << unique_vals.size();

        if (is_numeric && !nums.empty()) {
            double mean = std::accumulate(nums.begin(), nums.end(), 0.0)
                        / nums.size();
            double mn   = *std::min_element(nums.begin(), nums.end());
            double mx   = *std::max_element(nums.begin(), nums.end());
            std::cout << std::fixed << std::setprecision(2)
                      << "mean=" << mean
                      << "  min=" << mn
                      << "  max=" << mx;
        } else {
            // Show top value, truncated
            std::string tv = top_val;
            if (tv.size() > 16) tv = tv.substr(0, 13) + "...";
            std::cout << "top='" << tv << "' (" << top_cnt << "x)";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
