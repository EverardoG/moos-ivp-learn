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

# Include directories if needed
target_include_directories(test_utils PRIVATE ${CMAKE_SOURCE_DIR})

# Link the MOOS libraries and other dependencies
target_link_libraries(general_utils PUBLIC
  ${MOOS_LIBRARIES}
  geometry
  mbutil
)

# Link the general_utils library to the test_utils executable
target_link_libraries(test_utils PRIVATE general_utils)
