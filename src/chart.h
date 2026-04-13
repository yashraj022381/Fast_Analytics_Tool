#pragma once
// chart.h — Feature 3: --chart flag
// Prints a horizontal ASCII bar chart of group results in the terminal.
// Example output:
//   City  (sum_total)
//   Naypyitaw  ████████████████████  $109,685
//   Yangon     ████████████████      $105,006
//   Mandalay   ████████████████      $101,607

#include <string>
#include <vector>
#include <utility>
#include "analytics.h"

class Chart {
public:
    // field = "sum_total" | "count" | "avg_rating"
    static void print(
        const std::vector<std::pair<std::string, GroupStats>>& sorted,
        const std::string& title,
        const std::string& field = "sum_total",
        int bar_width = 38);
};
