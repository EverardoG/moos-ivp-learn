cmake_minimum_required(VERSION 3.10)
project(NeuralNetwork)

set(CMAKE_CXX_STANDARD 17)

# Define the library
add_library(sector_sensor sector_sensor.cpp)

# Specify the include directories for the library
target_include_directories(sector_sensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add the include directory for general_utils using CMAKE_CURRENT_SOURCE_DIR
# target_include_directories(sector_sensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../general_utils)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Add the source files
add_executable(test_sensor test_sensor.cpp)

# Include directories for test_sensor
target_include_directories(test_sensor PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../general_utils)

# Link the MOOS libraries and other dependencies
target_link_libraries(sector_sensor PUBLIC
  general_utils
  ${MOOS_LIBRARIES}
  geometry
  apputil
  mbutil
  m
  pthread)

# Link the sector_sensor library to the test_sensor executable
target_link_libraries(test_sensor PRIVATE sector_sensor)