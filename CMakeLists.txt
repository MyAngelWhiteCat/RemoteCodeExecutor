cmake_minimum_required(VERSION 3.11)

project(RemoteCodeExecutor CXX)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

add_executable(RemoteCodeExecutor
    src/remote_code_executor.h
    src/remote_code_executor.cpp
    src/main.cpp 
)
