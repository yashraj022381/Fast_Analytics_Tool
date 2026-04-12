// exporter.cpp — COMPLETE FIXED FILE
#include "exporter.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream>

// ── JSON ─────────────────────────────────────────────────────────
void Exporter::save_json(const std::unordered_map<std::string, GroupStats>& results,
                         const std::string& group_name,
                         const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) { std::cout << "Error writing " << filename << "\n"; return; }

    file << "{\n"
         << "  \"group_by\": \"" << group_name << "\",\n"
         << "  \"results\": {\n";

    bool first = true;
    for (const auto& p : results) {
        if (!first) file << ",\n";
        double avg = p.second.count > 0 ? p.second.sum_rating / p.second.count : 0.0;
        file << "    \"" << p.first << "\": {\n"
             << "      \"count\": "      << p.second.count << ",\n"
             << "      \"sum_total\": "  << std::fixed << std::setprecision(2) << p.second.sum_total  << ",\n"
             << "      \"avg_rating\": " << std::fixed << std::setprecision(2) << avg                 << ",\n"
             << "      \"min_total\": "  << std::fixed << std::setprecision(2) << p.second.min_total  << ",\n"
             << "      \"max_total\": "  << std::fixed << std::setprecision(2) << p.second.max_total  << "\n"
             << "    }";
        first = false;
    }
    file << "\n  }\n}\n";
    file.close();
    std::cout << "Saved JSON: " << filename << "\n";
}

// ── CSV ──────────────────────────────────────────────────────────
void Exporter::save_csv(const std::unordered_map<std::string, GroupStats>& results,
                        const std::string& group_name,
                        const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) { std::cout << "Error writing " << filename << "\n"; return; }

    file << group_name << ",Count,Sum_Total,Avg_Rating,Min_Total,Max_Total\n";
    for (const auto& p : results) {
        double avg = p.second.count > 0 ? p.second.sum_rating / p.second.count : 0.0;
        file << p.first                                               << ","
             << p.second.count                                        << ","
             << std::fixed << std::setprecision(2) << p.second.sum_total  << ","
             << std::fixed << std::setprecision(2) << avg                 << ","
             << std::fixed << std::setprecision(2) << p.second.min_total  << ","
             << std::fixed << std::setprecision(2) << p.second.max_total  << "\n";
    }
    file.close();
    std::cout << "Saved CSV:  " << filename << "\n";
}

// ── HTML ─────────────────────────────────────────────────────────
void Exporter::save_html(const std::unordered_map<std::string, GroupStats>& results,
                         const std::string& group_name,
                         const std::string& filename)
{
    // Sort by sum_total descending for the table
    std::vector<std::pair<std::string, GroupStats>> sorted(results.begin(), results.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.second.sum_total > b.second.sum_total;
    });

    std::ofstream file(filename);
    if (!file.is_open()) { std::cout << "Error writing " << filename << "\n"; return; }

    // ── Compute summary totals for the cards ──
    long long total_count = 0;
    double    total_sum   = 0.0, total_rating = 0.0;
    for (const auto& p : sorted) {
        total_count  += p.second.count;
        total_sum    += p.second.sum_total;
        total_rating += p.second.sum_rating;
    }
    double overall_avg = total_count > 0 ? total_rating / total_count : 0.0;
    double max_sum     = sorted.empty() ? 1.0 : sorted[0].second.sum_total;

    // ── HTML head ──
    file << R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Fast Analytics Report</title>
<style>
  *    { box-sizing:border-box; margin:0; padding:0; }
  body { font-family:'Segoe UI',Arial,sans-serif; background:#0f172a;
         color:#e2e8f0; padding:36px 40px; }
  h1   { color:#60a5fa; font-size:30px; margin-bottom:6px; }
  .sub { color:#94a3b8; font-size:14px; margin-bottom:32px; }
  .sub strong { color:#93c5fd; }

  .cards { display:flex; gap:16px; flex-wrap:wrap; margin-bottom:32px; }
  .card  { background:#1e293b; border:1px solid #1e3a5f;
           border-radius:12px; padding:18px 26px; min-width:160px; }
  .card-label { font-size:11px; color:#94a3b8; margin-bottom:8px;
                text-transform:uppercase; letter-spacing:.06em; }
  .card-value { font-size:30px; font-weight:700; color:#60a5fa; }

  .table-wrap { border-radius:12px; overflow:hidden; border:1px solid #1e3a5f; }
  table { width:100%; border-collapse:collapse; }
  th { background:#0f2744; padding:14px 18px; text-align:left;
       font-size:11px; color:#93c5fd; text-transform:uppercase;
       letter-spacing:.06em; }
  td { padding:13px 18px; border-bottom:1px solid #1e3a5f; font-size:14px; }
  tr:last-child td { border-bottom:none; }
  tr:hover td      { background:#1a3050; }
  .rank  { color:#fbbf24; font-weight:700; }
  .group-name { font-weight:600; }

  .bar-cell  { display:flex; align-items:center; gap:10px; }
  .bar-track { background:#1e3a5f; border-radius:4px;
               height:10px; width:140px; flex-shrink:0; }
  .bar-fill  { background:#3b82f6; height:10px; border-radius:4px; }
  .bar-pct   { font-size:12px; color:#64748b; min-width:34px; }

  footer { margin-top:28px; color:#475569; font-size:12px; }
</style>
</head>
<body>
<h1>Fast Analytics Report</h1>
)";

    file << "<p class=\"sub\">Grouped by: <strong> " << group_name << "</strong>"
         << " &nbsp;|&nbsp; " << sorted.size() << " groups"
         << " &nbsp;|&nbsp; Generated by Fast Analytics C++ Tool</p>\n";

    // ── Summary cards ──
    file << "<div class=\"cards\">\n"
         << "  <div class=\"card\"><div class=\"card-label\">Total Groups</div>"
         << "<div class=\"card-value\">" << sorted.size() << "</div></div>\n"
         << "  <div class=\"card\"><div class=\"card-label\">Total Rows</div>"
         << "<div class=\"card-value\">" << total_count << "</div></div>\n"
         << "  <div class=\"card\"><div class=\"card-label\">Total Revenue</div>"
         << "<div class=\"card-value\">$"
         << std::fixed << std::setprecision(0) << total_sum << "</div></div>\n"
         << "  <div class=\"card\"><div class=\"card-label\">Avg Rating</div>"
         << "<div class=\"card-value\">"
         << std::fixed << std::setprecision(2) << overall_avg << "</div></div>\n"
         << "</div>\n";

    // ── Table header ──
    file << "<div class=\"table-wrap\">\n<table>\n<thead><tr>\n"
         << "  <th>#</th>\n"
         << "  <th>" << group_name << "</th>\n"
         << "  <th>Count</th>\n"
         << "  <th>Sum Total</th>\n"
         << "  <th>Avg Rating</th>\n"
         << "  <th>Min Total</th>\n"
         << "  <th>Max Total</th>\n"
         << "  <th>Revenue Share</th>\n"
         << "</tr></thead>\n<tbody>\n";

    // ── DATA ROWS ── (this is the part that was completely missing before)
    int rank = 1;
    for (const auto& p : sorted) {
        double avg_rating = p.second.count > 0
                          ? p.second.sum_rating / p.second.count : 0.0;
        int bar_pct = static_cast<int>(p.second.sum_total / max_sum * 100);

        file << "<tr>\n"
             << "  <td class=\"rank\">"      << rank++          << "</td>\n"
             << "  <td class=\"group-name\">" << p.first         << "</td>\n"
             << "  <td>"                      << p.second.count  << "</td>\n"
             << "  <td>$" << std::fixed << std::setprecision(2)
                          << p.second.sum_total  << "</td>\n"
             << "  <td>"  << std::fixed << std::setprecision(2)
                          << avg_rating           << "</td>\n"
             << "  <td>$" << std::fixed << std::setprecision(2)
                          << p.second.min_total   << "</td>\n"
             << "  <td>$" << std::fixed << std::setprecision(2)
                          << p.second.max_total   << "</td>\n"
             << "  <td>\n"
             << "    <div class=\"bar-cell\">\n"
             << "      <div class=\"bar-track\">"
             <<           "<div class=\"bar-fill\" style=\"width:"
                          << bar_pct << "%\"></div></div>\n"
             << "      <span class=\"bar-pct\">" << bar_pct << "%</span>\n"
             << "    </div>\n"
             << "  </td>\n"
             << "</tr>\n";
    }

    file << "</tbody>\n</table>\n</div>\n"
         << "<footer>Fast Analytics C++ Tool &mdash; 5-10x faster than Pandas</footer>\n"
         << "</body>\n</html>\n";

    file.close();
    std::cout << "Saved HTML: " << filename << "\n";
}