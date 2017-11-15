cmake_minimum_required(VERSION 3.5)
project(globalwl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -Wextra -pedantic")

set(SOURCE_FILES main.cpp src/Graph.h src/Graph.cpp src/ColorRefinementKernel.h src/ColorRefinementKernel.cpp src/ShortestPathKernel.h src/ShortestPathKernel.cpp src/AuxiliaryMethods.h src/AuxiliaryMethods.cpp src/GraphletKernel.h src/GraphletKernel.cpp
        src/WeisfeilerLehmanThreeLocal.h src/WeisfeilerLehmanThreeLocal.cpp src/WeisfeilerLehmanThreeLocal.cpp src/WeisfeilerLehmanThreeLocal.cpp src/WeisfeilerLehmanTwoLocal.h src/WeisfeilerLehmanTwoLocal.cpp src/WeisfeilerLehmanThreeGlobal.h src/WeisfeilerLehmanThreeGlobal.cpp src/WeisfeilerLehmanTwoGlobal.h src/WeisfeilerLehmanTwoGlobal.cpp)
add_executable(globalwl ${SOURCE_FILES})


