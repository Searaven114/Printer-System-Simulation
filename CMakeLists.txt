cmake_minimum_required(VERSION 3.30)
project(Printer_System_Simulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(Printer_System_Simulation main.cpp
        PrintQueue.cpp
        PrintQueue.h)
