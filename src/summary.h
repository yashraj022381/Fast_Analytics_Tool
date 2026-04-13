#pragma once
// summary.h — Feature 5: --summary flag
// Dataset overview similar to pandas df.info() + df.describe():
//   - total rows, total columns
//   - per column: type (numeric/text), non-empty count,
//     empty count, unique values, top value or mean/min/max

#include <string>
#include <vector>

class Summary {
public:
    static void print(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& headers);
};
