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
   
🛠️ Technology Stack
    Technology                        Usage
    C++20                             Core language — structured bindings, [[nodiscard]], ranges
    STL                               unordered_map, vector, sort, accumulate, chrono
    std::thread / mutex               Custom thread pool with 4 parallel workers
    condition_variable                Worker sleep/wake for zero CPU idle cost
    CMake 3.14+                       Cross-platform build system
    ofstream / ifstream               Direct file I/O without wrappers



📂 File Structure

  fast_analytics/
├── CMakeLists.txt
├── data/
│   └── sample.csv            # Kaggle supermarket sales dataset
└── src/
    ├── main.cpp              # CLI entry point — all flags wired here
    ├── analytics.h/.cpp      # Parallel group-by engine (4 threads)
    ├── thread_pool.h/.cpp    # Custom thread pool with wait_all()
    ├── csv_parser.h/.cpp     # Fast CSV reader with quoted-field support
    ├── exporter.h/.cpp       # JSON / CSV / HTML report exporter
    ├── filter.h/.cpp         # Row filtering before analytics
    ├── describe.h/.cpp       # Deep statistical analysis
    ├── chart.h/.cpp          # ASCII bar chart renderer
    ├── benchmark.h/.cpp      # Multi-run timing benchmark
    └── summary.h/.cpp        # Dataset overview (like pandas df.info())

⚙️ Build Instructions

   - Requriments
      Requirement           Minimum version
      GCC / Clang / MSVC    GCC 11 · Clang 12 · MSVC 2022
      CMake                 3.14
      C++ Standard          C++20
      RAM                   512 MB (for 1M row datasets)
      OS                    Windows 10+ · Ubuntu 20.04+ · macOS 12+

   - Windows (Visual Studio / PowerShell)
     powershell
     git clone https://github.com/YOUR_USERNAME/fast-data-analytics.git
     cd fast-data-analytics

     mkdir build
     cd build
     cmake ..
     cmake --build . --config Release
     cd ..
     
   - Linux / macOS
     bash
     git clone https://github.com/YOUR_USERNAME/fast-data-analytics.git
     cd fast-data-analytics

     mkdir build && cd build
     cmake .. -DCMAKE_BUILD_TYPE=Release
     make -j4
     cd ..

   - Note: CMake automatically copies the data/ folder next to the executable after every build.
  

💻 Usage
  fast_analytics.exe [flags]
 
  Core Flags
   Flag                     Description                           Example                                                                                                       --group <column>         Group-by analytics                    --group City
   --input <file>           Custom CSV path                       --input data/sales.csv
   --output <prefix>        Output filename prefix                --output city_report
   --top <N>                Show only top N groups                --top 5
   --sort <field>           Sort by field                         --sort avg_rating
   --threads <N>            Worker thread count                   --threads 8
   --columns                List all column names                 --columns
   --help                   Show full help                        --help
  
   --sort fields: sum_total (default) · count · avg_rating
  
  Advanced Flags
   Flag                     Description                           Example
   --filter <expr>          Filter rows before analytics          --filter "City=Yangon"
   --describe <col>         Deep stats on a column                --describe Total
   --chart [field]          ASCII bar chart in terminal           --chart count
   --benchmark [N]          Timing benchmark over N runs          --benchmark 10
   --summary                Dataset overview                      --summary
  
   --filter operators: = · != · > · < · >= · <=
   Multiple --filter flags apply AND logic.


 🗂 Dataset
 - Uses the Supermarket Sales dataset from Kaggle.
   
 - Place the CSV at: data/sample.csv (or use --input to specify any path).
 - CSV columns: Invoice ID · Branch · City · Customer type · Gender · Product line · Unit price · Quantity · Tax 5% · Total · Date · Time · Payment · cogs · gross margin       percentage · gross income · Rating


📊 Example Output
   
### Examples
 - Group-By Analytics
   bash
   $ fast_analytics.exe --group City
   
   Loading: data/sample.csv
   [TIME] CSV load: 2 ms
   Loaded 1000 rows, 17 columns.

   Processing 1000 rows with 4 threads...
   Done! Found 3 groups in 0 ms
   [TIME] Analytics (4 threads): 1 ms
   Found 3 groups.

   Group                   Count     Sum Total        Avg Rating   Min Total     Max Total
   ------------------------------------------------------------------------------------
   Naypyitaw               350       $109685.31       7.04         $16.19        $973.04
   Yangon                  331       $105006.66       6.97         $10.58        $1049.58
   Mandalay                319       $101607.05       6.97         $11.00        $1048.95
   ------------------------------------------------------------------------------------

   [TIME] Export: 0 ms
   === Analysis Completed Successfully ===
   Output files: results.json / results.csv / results.html
   5-10x faster than Pandas on large datasets!
   

  - Filter + Group-By
    bash
    $ fast_analytics.exe --group City --filter "Gender=Female" --filter "Rating>7"
    
    [FILTER] 257 / 1000 rows kept.
    [TIME] Filter: 0 ms

    Processing 257 rows with 4 threads...
    Done! Found 3 groups in 0 ms

    Group                   Count     Sum Total        Avg Rating   Min Total     Max Total
    ------------------------------------------------------------------------------------
    Naypyitaw               99        $26278.11        8.52         $16.19        $943.96
    Yangon                  83        $29802.85        8.38         $23.27        $1049.58
    Mandalay                75        $23309.16        8.47         $12.09        $939.99
    ------------------------------------------------------------------------------------
    

  - Deep Statistics (--describe)
    bash
    $ fast_analytics.exe --describe Total
    +------------------------------------------+
    |  Deep Stats: Total                       |
    +------------------------------------------+
    |  Count                         1000      |
    |  Sum                      316299.02      |
    |  Mean                        316.30      |
    |  Median                      244.16      |
    |  Mode                          42.4      |
    |  Std Dev                     243.44      |
    |  Variance                  59264.79      |
    |  Skewness                    0.9349      |
    +------------------------------------------+
    |  Min                          10.58      |
    |  P25                         120.11      |
    |  P75                         474.69      |
    |  P90                         668.47      |
    |  P95                         804.58      |
    |  P99                         961.29      |
    |  Max                        1049.58      |
    +------------------------------------------+
    

  - ASCII Bar Chart (--chart)
    bash
    $ fast_analytics.exe --group "Product line" --chart count --sort count
    ------------------------------------------------------------------------------------
      Bar Chart: Product line  [count]
    ------------------------------------------------------------------------------------
      Sports and travel        |######################################| 197
      Fashion accessories      |##################################    | 177
      Electronic accessories   |###############################       | 162
      Health and beauty        |##############################        | 160
      Food and beverages       |##############################        | 159
      Home and lifestyle       |###########################           | 145
    ------------------------------------------------------------------------------------
    

  - Dataset Overview (--summary)
    bash
    $ fast_analytics.exe --summary
    +==========================================================+
    |              Dataset Summary                             |
    +==========================================================+
    |  Rows    : 1000                                          |
    |  Columns : 17                                            |
    +==========================================================+

    Column                Type      NonEmpty  Empty   Unique    Details
    ------------------------------------------------------------------------------
    InvoiceID             text      1000      0       1000      top='INV-0000' (1x)
    Branch                text      1000      0       3         top='C' (356x)
    City                  text      1000      0       3         top='Naypyitaw' (350x)
    Gender                text      1000      0       2         top='Female' (518x)
    Productline           text      1000      0       6         top='Sports and tr...' (197x)
    Unitprice             numeric   1000      0       940       mean=54.04  min=10.08  max=99.99
    Quantity              numeric   1000      0       10        mean=5.54   min=1.00   max=10.00
    Total                 numeric   1000      0       986       mean=316.30 min=10.58  max=1049.58
    Rating                numeric   1000      0       61        mean=6.99   min=4.00   max=10.00
    --------------------------------------------------------------------------------

    
  - Performance Benchmark (--benchmark)
    bash
    $ fast_analytics.exe --benchmark 5 --group City
    Benchmark: 5 runs, 4 threads, 1000 rows
      Run 1/5: 1 ms
      Run 2/5: 0 ms
      Run 3/5: 0 ms
      Run 4/5: 0 ms
      Run 5/5: 0 ms

    +-------------------------------------------+
    |          Benchmark Results                |
    +-------------------------------------------+
    |  Column   : City                          |
    |  Threads  : 4                             |
    |  Rows     : 1000                          |
    |  Runs     : 5                             |
    +-------------------------------------------+
    |  Min      : 0 ms                          |
    |  Max      : 1 ms                          |
    |  Average  : 0 ms                          |
    |  rows/sec : 5,000,000+                    |
    +-------------------------------------------+
    

    Resume line:
    - Processed 1000 rows in avg 0 ms using 4 threads (5000000+ rows/sec)
   
🧠 Architecture Deep Dive
  - Thread Pool Design
    - The core parallelism engine uses a producer-consumer pattern:
      main thread                    worker threads (×4)
      ──────────────────────         ──────────────────────────────────────
      ThreadPool pool(4)             each thread runs an infinite loop:
                                     lock queue → wait for task
      analytics.run(...)             unlock → run task
        └─ splits 1M rows             lock → merge results → unlock
           into 4 chunks
        └─ enqueues 4 tasks
        └─ (returns immediately)
      }  ← destructor: join()        all 4 threads finish
                               ↓
      analytics.get_results()        results are 100% complete ✓
      
  - Race condition fix: ThreadPool is scoped inside { } braces. When the block closes, ~ThreadPool() calls join() on every thread.
  - This guarantees all threads have finished before get_results() is ever called.
   
  - Filter Engine
   - Row filtering uses compile-time operator dispatch — numeric comparisons (>, <, >=, <=) parse strings to double on the fly, while string comparisons (=, !=) use direct       equality.
   - Multiple filters apply AND logic — a row must pass every filter to be kept.
     
  - Statistics Engine
   - --describe computes 16 statistics in a single pass over the data (for mean/sum/variance) plus one sort (for median/percentiles).
   - Skewness uses the Fisher-Pearson formula. Mode is computed via a frequency map rounded to 1 decimal place.
      
📤 Output Formats
  - The tool generates an output/ directory containing:
    - results.json: A machine-readable summary.  
    - results.csv: A raw data summary.
    - report.html: A visual dashboard with a "Sales Contribution" bar chart.   

  - Every analysis run produces three output files automatically:
    File                Format             Use case
    results.json        JSON object        APIs, Python, web dashboards
    results.csv         CSV table          Excel, Google Sheets, pandas
    results.html        Dark-theme HTML    Browser report with visual bars

 - Custom prefix with --output my_report → my_report.json / .csv / .html


🔧 Extending the Tool
  - To add a new analytics operation:
    - Create src/myfeature.h and src/myfeature.cpp
    - Add src/myfeature.cpp to CMakeLists.txt
    - Add #include "myfeature.h" in main.cpp
    - Add argument parsing in the for loop in main()
    - Call your function after the filter step

📄 License
  - This project is open-source and available under the MIT License.
