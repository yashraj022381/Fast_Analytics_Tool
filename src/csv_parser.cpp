#include "csv_parser.h"
#include <fstream>
#include <iostream>

bool CsvParser::read(const std::string& filename,
                     std::vector<std::vector<std::string>>& data,
                     std::vector<std::string>& headers)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open '" << filename << "'\n";
        std::cerr << "Make sure data/sample.csv exists next to fast_analytics.exe\n";
        return false;
    }

    std::string line;

    // First line = column headers
    if (!std::getline(file, line)) {
        std::cerr << "Error: File is empty: " << filename << "\n";
        return false;
    }
    headers = split(line);

    // Reserve space upfront (speed trick)
    data.reserve(1000000);

    while (std::getline(file, line)) {
        if (line.empty() || line == "\r") continue;
        data.push_back(split(line));
    }
    return true;
}

// Correctly splits CSV lines including quoted fields
// e.g. "New York","Smith, John",45  ->  [New York] [Smith, John] [45]
std::vector<std::string> CsvParser::split(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            tokens.push_back(token);
            token.clear();
        } else if (c == '\r') {
            // skip Windows carriage return
        } else {
            token += c;
        }
    }
    tokens.push_back(token); // last field
    return tokens;
}

void CsvParser::print_columns(const std::vector<std::string>& headers) {
    std::cout << "\nAvailable columns (" << headers.size() << " total):\n";
    std::cout << std::string(40, '-') << "\n";
    for (size_t i = 0; i < headers.size(); ++i)
        std::cout << "  [" << i << "] " << headers[i] << "\n";
    std::cout << "\n";
}
