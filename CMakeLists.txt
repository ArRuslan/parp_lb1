cmake_minimum_required(VERSION 3.25)
project(parp_lb1)

set(CMAKE_CXX_STANDARD 20)

add_executable(parp_lb1.exe main.cpp)
target_link_libraries(parp_lb1.exe PRIVATE gomp)
target_compile_options(parp_lb1.exe PRIVATE "-std=c++20")

#set(CMAKE_CXX_COMPILER /usr/bin/g++)
#add_executable(test test.cpp)
#target_link_libraries(test PRIVATE parp_lb1.exe.so)