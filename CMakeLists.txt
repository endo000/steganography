cmake_minimum_required(VERSION 3.22)
project(Steganography)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Boost 1.81.0 REQUIRED COMPONENTS mpi serialization filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# -fopenmp flag (enables OpenMP)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

find_package(MPI REQUIRED)

add_executable(Steganography main.cpp steganography/steganography.cpp utils/utils.cpp utils/config_utils.cpp steganalysis/steganalysis.cpp openmpi/utils/utils.cpp)

target_link_libraries(Steganography PUBLIC MPI::MPI_CXX ${OpenCV_LIBS} ${Boost_LIBRARIES})