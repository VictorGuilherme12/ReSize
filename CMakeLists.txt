cmake_minimum_required(VERSION 3.30.5)
project(ReSize)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_language(RC)
set(APP_ICON_RESOURCE_WINDOWS "resource/resource.rc")

# Força linkagem estática
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

include_directories(.)

add_executable(ReSize
        src/main.cpp
        include/MonitorInfo.h
        src/MonitorUtils.cpp
        include/MonitorUtils.h)

# Adiciona bibliotecas Windows necessárias
target_link_libraries(ReSize PRIVATE
        user32
        gdi32
        shell32)