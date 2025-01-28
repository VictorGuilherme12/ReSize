cmake_minimum_required(VERSION 3.30)
project(ReSize)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(ReSize
    main.cpp
    MonitorInfo.h
    MonitorUtils.cpp
    MonitorUtils.h)
