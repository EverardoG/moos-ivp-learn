cmake_minimum_required(VERSION 3.10)
project(NeuralNetworkTest)

set(CMAKE_CXX_STANDARD 17)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Add the source files
add_executable(test_network test_network.cpp)
add_executable(test_node test_node.cpp)
add_executable(test_layer test_layer.cpp)

# Include directories if needed
target_include_directories(test_network PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(test_node PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(test_layer PRIVATE ${CMAKE_SOURCE_DIR})
