cmake_minimum_required(VERSION 3.10)
project(GeneralUtils)

set(CMAKE_CXX_STANDARD 17)

# Define the header-only library
add_library(general_utils INTERFACE)

# Specify the include directories for the library
target_include_directories(general_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Add the source files
# add_executable(test_sensor test_sensor.cpp)

# Include directories if needed
# target_include_directories(test_sensor PRIVATE ${CMAKE_SOURCE_DIR})

# Link the MOOS libraries and other dependencies
target_link_libraries(general_utils INTERFACE
  ${MOOS_LIBRARIES}
  geometry
  apputil
  mbutil
  m
  pthread)