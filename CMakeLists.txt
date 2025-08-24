cmake_minimum_required(VERSION 3.10)
project(StatsCollector)

# Make include directory available globally
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the library (from subdirectory lib/)
add_subdirectory(data_io)

# Define executable
add_executable(stat_collector src/stat_collector.c)

# Link the executable with the library we made
target_link_libraries(stat_collector PRIVATE data_io)
