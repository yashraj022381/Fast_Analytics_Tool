#include "csv_parser.h"
#include <fstream>
#include <sstream>
#include<iostream>

bool CsvParser::read(const std::string& filename,
                     std::vector<std::vector<std::string>>& data,
                     std::vector<std::string>& headers) 
{
    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        std::cerr << "😢 Error: Cannot find " << filename << "\n";
        return false;
    }

    std::string line;
    // Read first line = headers
    std::getline(file, line);
    headers = split(line);

    // Read all other lines
    while (std::getline(file, line)) 
    {
        if (!line.empty()) 
        {
            data.push_back(split(line));
        }
    }
    return true;
}

std::vector<std::string> CsvParser::split(const std::string& line) 
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}