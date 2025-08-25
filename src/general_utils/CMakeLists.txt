cmake_minimum_required(VERSION 3.10)
project(GeneralUtils)

set(CMAKE_CXX_STANDARD 17)

# Define the general_utils library
add_library(general_utils general_utils.cpp)

# Specify the include directories for the library
target_include_directories(general_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Add the test_utils executable
add_executable(test_utils test_utils.cpp)
add_executable(process_node_reports cli_process_node_reports.cpp)
add_executable(csv_filter_duplicate_rows cli_filter_duplicate_rows.cpp)
add_executable(csv_merge_files cli_merge_csv_files.cpp)

# Include directories if needed
target_include_directories(test_utils PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(process_node_reports PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(csv_filter_duplicate_rows PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(csv_merge_files PRIVATE ${CMAKE_SOURCE_DIR})

# Link the MOOS libraries and other dependencies
target_link_libraries(general_utils PUBLIC
  ${MOOS_LIBRARIES}
  geometry
  mbutil
)

# Link the general_utils library to the test_utils executable
target_link_libraries(test_utils PRIVATE general_utils)
target_link_libraries(process_node_reports PRIVATE general_utils)
target_link_libraries(csv_filter_duplicate_rows PRIVATE general_utils)
target_link_libraries(csv_merge_files PRIVATE general_utils)
