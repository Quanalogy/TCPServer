cmake_minimum_required(VERSION 3.5.2)
project(file_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp TCPServer.cpp TCPServer.h)
add_executable(file_server ${SOURCE_FILES})