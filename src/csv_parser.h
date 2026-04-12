#pragma once
#include <vector>
#include <string>

class CsvParser {
public:
    // This reads the whole CSV file super fast
    bool read(const std::string& filename,
              std::vector<std::vector<std::string>>& data,
              std::vector<std::string>& headers);
private:
    // Helper: splits one line by commas (simple version for our Kaggle file)
    std::vector<std::string> split(const std::string& line);
};