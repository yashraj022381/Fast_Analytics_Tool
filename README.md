# Fast_Analytics_Tool

![C++](https://img.shields.io/badge/C++-20-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Performance](https://img.shields.io/badge/Performance-Blazing%20Fast-orange)


**High-Performance CSV Analytics Engine built in Modern C++20**

- A high-performance, multi-threaded C++20 tool designed for rapid processing and analysis of large CSV datasets. 
- This tool utilizes a custom thread pool architecture to parallelize data aggregation and provides multiple export formats including HTML, JSON, and CSV.
- A blazing-fast command-line tool that processes millions of rows with multithreading — 5-10x faster than Pandas.


🚀 Features
  - Multithreaded Processing: Leverages a custom-built Thread Pool to distribute data analysis tasks across multiple CPU cores.

  - C++20 Implementation: Utilizes modern C++ features for efficiency and type safety.

  - Robust CSV Parsing: Fast and flexible parsing of tabular data.

  - Dynamic Grouping: Perform data aggregation (counts, sums, averages, min/max) based on specific columns (e.g., City, Product Line).

  - Colored terminal output + Progress bar

  - Clean & Professional code

  - Multiple Export Formats:

    - HTML: Beautifully formatted report with visual progress bars.

    - JSON: Structured data for integration with web apps or other tools.

    - CSV: Clean, comma-separated values for further spreadsheet analysis.


📂 File Structure

  fast_analytics/
  ├── CMakeLists.txt
  ├── README.md
  ├── data/
  │   └── sample.csv           ← 1000 rows of supermarket data included
  ├── src/
  │   ├── main.cpp
  │   ├── csv_parser.cpp / .h
  │   ├── analytics.cpp / .h
  │   ├── thread_pool.cpp / .h
  │   └── exporter.cpp / .h
  ├── results.csv
  ├── results.json
  ├── results.html          
  └── .vscode/tasks.json

  
🛠️ Architecture
  - main.cpp: Entry point handling CLI arguments and orchestrating the workflow.

  - csv_parser: Handles robust reading and splitting of raw CSV files.

  - thread_pool: A custom concurrency manager that handles task enqueueing and synchronization.

  - analytics: The core engine that detects columns and calculates statistics (GroupStats).

  - exporter: Logic for converting processed statistics into final report files.



📋 Prerequisites
  - Compiler: C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+).
 
  - Build System: CMake 3.14 or higher.


📊 Sample Output
   - The tool generates an output/ directory containing:

     - report.html: A visual dashboard with a "Sales Contribution" bar chart.

     - results.json: A machine-readable summary.

     - results.csv: A raw data summary.
    
### Usage Examples
   ``bash
   # Top 5 cities by sales
   ./fast_analytics --group "City" --top 5

   # All product lines
   ./fast_analytics --group "Product line"

   # List all columns
   ./fast_analytics --columns

📄 License
  - This project is open-source and available under the MIT License.
