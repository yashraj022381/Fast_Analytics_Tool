#pragma once
// filter.h — Feature 1: --filter flag
// Filter rows before analytics runs.
// Supports: =  !=  >  <  >=  <=
// Examples:
//   --filter "City=Yangon"
//   --filter "Rating>7"
//   --filter "Total<=500"
// Multiple --filter flags apply AND logic.

#include <string>
#include <vector>

struct FilterRule {
    std::string column;
    std::string op;
    std::string value;
};

class Filter {
public:
    // Parse "City=Yangon" or "Rating>7.0" into a FilterRule
    static bool parse(const std::string& expr, FilterRule& out);

    // Apply all rules — keeps rows that pass ALL rules
    static std::vector<std::vector<std::string>> apply(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& headers,
        const std::vector<FilterRule>& rules);

private:
    static bool matches(const std::vector<std::string>& row,
                        const std::vector<std::string>& headers,
                        const FilterRule& rule);
};
