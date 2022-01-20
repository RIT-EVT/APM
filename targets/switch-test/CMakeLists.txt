include(${EVT_CORE_DIR}/cmake/evt-core_build.cmake)

project(switch-test)
cmake_minimum_required(VERSION 3.15)

make_exe(${PROJECT_NAME} switch_test.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ${BOARD_LIB_NAME})
