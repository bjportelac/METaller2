cmake_minimum_required(VERSION 3.28)
project(METaller2)

set(CMAKE_CXX_STANDARD 14)

add_executable(METaller2
        lcgrand.cpp
        lcgrand.h
        MM1Simulation.cpp
        erlangf.cpp
        erlangf.h
        CustomerData.h)
