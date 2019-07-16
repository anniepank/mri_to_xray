cmake_minimum_required(VERSION 3.6)
project(drr_generation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp nifti1_io.cpp znzlib.cpp)
add_executable(drr_generation ${SOURCE_FILES})
target_link_libraries(drr_generation -lpng)
