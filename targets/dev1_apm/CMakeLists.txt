include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/evt-core_build.cmake)

project(dev1_apm)
cmake_minimum_required(VERSION 3.15)

make_exe(${PROJECT_NAME} dev1_apm.cpp)