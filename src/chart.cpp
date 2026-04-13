#include "chart.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <sstream>

void Chart::print(
    const std::vector<std::pair<std::string, GroupStats>>& sorted,
    const std::string& title,
    const std::string& field,
    int bar_width)
{
    if (sorted.empty()) return;

    // Find max value for scaling
    double max_val = 0;
    for (const auto& [k, s] : sorted) {
        double v = (field == "count")      ? (double)s.count
                 : (field == "avg_rating") ? (s.count > 0 ? s.sum_rating / s.count : 0)
                 :                            s.sum_total;
        max_val = std::max(max_val, v);
    }
    if (max_val <= 0) max_val = 1;

    // Label column width
    size_t lw = title.size();
    for (const auto& [k, s] : sorted) lw = std::max(lw, k.size());
    lw = std::min(lw + 2, size_t(26));

    int total_w = (int)lw + bar_width + 22;
    std::string line(total_w, '-');

    std::cout << "\n" << line << "\n";
    std::cout << " Bar Chart: " << title << "  [" << field << "]\n";
    std::cout << line << "\n";

    for (const auto& [key, s] : sorted) {
        double val = (field == "count")      ? (double)s.count
                   : (field == "avg_rating") ? (s.count > 0 ? s.sum_rating / s.count : 0)
                   :                            s.sum_total;

        std::ostringstream vstr;
        if (field == "count")
            vstr << s.count;
        else if (field == "avg_rating")
            vstr << std::fixed << std::setprecision(2) << val;
        else
            vstr << "$" << std::fixed << std::setprecision(0) << val;

        int filled = static_cast<int>((val / max_val) * bar_width);
        int empty  = bar_width - filled;

        std::string label = key;
        if (label.size() > lw - 1) label = label.substr(0, lw - 4) + "...";

        std::cout << " " << std::left << std::setw((int)lw) << label << " |";
        for (int i = 0; i < filled; ++i) std::cout << "#";
        for (int i = 0; i < empty;  ++i) std::cout << " ";
        std::cout << "| " << vstr.str() << "\n";
    }
    std::cout << line << "\n\n";
}
