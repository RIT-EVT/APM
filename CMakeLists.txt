###############################################################################
# Top level CMakeList for building the EVT APMManager source code
###############################################################################
cmake_minimum_required(VERSION 3.15)

set(EVT_CORE_DIR      ${CMAKE_SOURCE_DIR}/libs/EVT-core)

include(CMakeDependentOption)

option(NUCLEO_COMPILATION
        "Set this to compile the code to run on the Nucleo development board"
        OFF
        )
if(NUCLEO_COMPILATION)
    add_definitions(-DNUCLEO_COMPILATION)
endif()
include(${EVT_CORE_DIR}/cmake/evt-core_compiler.cmake)
include(${EVT_CORE_DIR}/cmake/evt-core_install.cmake)



###############################################################################
# Project Setup
###############################################################################
set(BOARD_LIB_NAME   APM)
if("${BOARD_LIB_NAME}" STREQUAL CHANGE_ME)
    message(FATAL_ERROR
            "You must set the template project name in CmakeLists.txt")
endif()

file(STRINGS version.txt APM_VERSION)

project(APM
        VERSION ${APM_VERSION}
        LANGUAGES CXX C
        )

add_library(${PROJECT_NAME} STATIC)

# Add sources
target_sources(${PROJECT_NAME} PRIVATE
        src/APM/APMManager.cpp
        src/APM/APMUart.cpp
        src/APM/dev/SIM100.cpp
)

###############################################################################
# Handle dependencies
###############################################################################

# TODO: This should be set by the user of this library
add_compile_definitions(STM32F302x8)

# Link to the EVT-core library
add_subdirectory(libs/EVT-core/)

target_link_libraries(${PROJECT_NAME}
        PUBLIC EVT
)

###############################################################################
# Install and expose library
###############################################################################
install_and_expose(${PROJECT_NAME})

###############################################################################
# Build Test Code
###############################################################################
add_subdirectory(targets)