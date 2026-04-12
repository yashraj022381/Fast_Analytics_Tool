#pragma once
#include <string>
#include <unordered_map>
#include "analytics.h"

class Exporter {
public:
    void save_json(const std::unordered_map<std::string, GroupStats>& results,
                   const std::string& group_name,
                   const std::string& filename);

    void save_csv(const std::unordered_map<std::string, GroupStats>& results,
                  const std::string& group_name,
                  const std::string& filename);

    void save_html(const std::unordered_map<std::string, GroupStats>& results,
                   const std::string& group_name,
                   const std::string& filename);
};