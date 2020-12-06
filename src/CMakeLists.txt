cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(remote_fork_distributed_project)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

include_directories(${BOOST_INCLUDE_DIRS})

add_executable(client lib.cpp remote_proc.hpp remote_proc.cpp)
add_executable(server daemon.cpp remote_proc.hpp remote_proc.cpp)

target_link_libraries(client ${Boost_LIBRARIES})
target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(client ./libcriu.so)
target_link_libraries(server ./libcriu.so)
