#include "filter.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>

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

bool Filter::parse(const std::string& expr, FilterRule& out) {
    // Try longest operators first so ">=" is found before ">"
    for (const std::string op : {"!=", ">=", "<=", "=", ">", "<"}) {
        size_t pos = expr.find(op);
        if (pos == std::string::npos) continue;
        out.column = strip(expr.substr(0, pos));
        out.op     = op;
        out.value  = trim(expr.substr(pos + op.size()));
        if (out.column.empty() || out.value.empty()) {
            std::cout << "Bad filter: '" << expr << "'\n";
            return false;
        }
        return true;
    }
    std::cout << "Cannot parse filter '" << expr
              << "'. Use format Column=Value or Column>Number\n";
    return false;
}

bool Filter::matches(const std::vector<std::string>& row,
                     const std::vector<std::string>& headers,
                     const FilterRule& rule)
{
    // Find column index
    size_t col = size_t(-1);
    for (size_t i = 0; i < headers.size(); ++i) {
        if (strip(headers[i]) == rule.column) { col = i; break; }
    }
    if (col == size_t(-1) || col >= row.size()) return false;

    const std::string cell = trim(row[col]);

    if (rule.op == ">" || rule.op == "<" ||
        rule.op == ">=" || rule.op == "<=") {
        try {
            double cv = std::stod(cell);
            double rv = std::stod(rule.value);
            if (rule.op == ">")  return cv >  rv;
            if (rule.op == "<")  return cv <  rv;
            if (rule.op == ">=") return cv >= rv;
            if (rule.op == "<=") return cv <= rv;
        } catch (...) { return false; }
    }

    if (rule.op == "=")  return cell == rule.value;
    if (rule.op == "!=") return cell != rule.value;
    return false;
}

std::vector<std::vector<std::string>> Filter::apply(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& headers,
    const std::vector<FilterRule>& rules)
{
    if (rules.empty()) return data;

    std::vector<std::vector<std::string>> out;
    out.reserve(data.size());

    for (const auto& row : data) {
        bool pass = true;
        for (const auto& rule : rules)
            if (!matches(row, headers, rule)) { pass = false; break; }
        if (pass) out.push_back(row);
    }

    std::cout << "[FILTER] " << out.size() << " / "
              << data.size() << " rows kept.\n";
    return out;
}
