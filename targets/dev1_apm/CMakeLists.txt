include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/EVT-core/cmake/evt-core_build.cmake)

project(dev1_apm)
cmake_minimum_required(VERSION 3.15)

make_exe(${PROJECT_NAME} dev1_apm.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC APM)