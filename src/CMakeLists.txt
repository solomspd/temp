cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(remote_fork_distributed_project)

find_package(Threads REQUIRED)
find_package(SFML COMPONENTS network REQUIRED)

include_directories(/usr/local/lib64)
link_directories(/usr/local/lib64)

add_executable(client main.cpp)
add_executable(server daemon.cpp)

target_link_libraries(client Threads::Threads)
target_link_libraries(server Threads::Threads)
target_link_libraries(client criu)
target_link_libraries(server criu)
target_link_libraries(server sfml-network)
target_link_libraries(client sfml-network)

#### Previous libraries attempted to use with project ####

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#set(SFML_STATIC_LIBRARIES TRUE)
#find_package(Boost REQUIRED)
#find_package(CURL REQUIRED)
#find_package(gRPC CONFIG REQUIRED)
#find_package(ZeroMQ REQUIRED)
#include_directories(${BOOST_INCLUDE_DIRS})
#include_directories(${CURL_INCLUDE_DIR})
#target_link_libraries(client ${Boost_LIBRARIES})
#target_link_libraries(server ${Boost_LIBRARIES})
#target_link_libraries(client ${CURL_LIBRARIES})
#target_link_libraries(server ${CURL_LIBRARIES})
#target_link_libraries(client gRPC::grpc++)
#target_link_libraries(server gRPC::grpc++)
#target_link_libraries(client zmq)
#target_link_libraries(server zmq)
