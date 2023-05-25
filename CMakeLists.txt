cmake_minimum_required(VERSION 3.21)
project(parallel_computing_project)

set(CMAKE_C_STANDARD 99)

# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(parallel_computing_project main.c)

# Add libraries for code completion and compiling
target_link_libraries(parallel_computing_project PUBLIC MPI::MPI_CXX)

FILE(COPY resources/dataset_1.txt DESTINATION "${CMAKE_BINARY_DIR}/resources")