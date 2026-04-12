cmake_minimum_required(VERSION 3.14)
project(fast_analytics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)   # makes it 5-10x faster!

if(MSVC)
    add_compile_options(/W4 /std::c++20)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(fast_analytics
    src/main.cpp
    src/csv_parser.cpp
    src/analytics.cpp
    src/thread_pool.cpp
    src/exporter.cpp
)

if(NOT WIN32)
    target_link_libraries(fast_analytics PRIVATE pthread)  # robot army needs this
endif()