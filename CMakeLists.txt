cmake_minimum_required(VERSION 3.23)
project(Steganografia)

set(CMAKE_CXX_STANDARD 14)

#set(OpenCV_DIR "C:\\opencv\\build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Steganografia main.cpp steganography/steganography.cpp utils/utils.cpp utils/config_utils.cpp steganalysis/steganalysis.cpp)

target_link_libraries(Steganografia ${OpenCV_LIBS})