###############################################################################
# Top level CMakeList for building the EVT APM source code
###############################################################################
cmake_minimum_required(VERSION 3.15)

set(EVT_CORE_DIR      ${CMAKE_SOURCE_DIR}/libs/EVT-core)

include(CMakeDependentOption)
include(${CMAKE_SOURCE_DIR}/libs/EVT-core/cmake/evt-core_compiler.cmake)

###############################################################################
# Project Setup
###############################################################################
project(APM
        VERSION 0.0.1
        LANGUAGES CXX C
        )

add_library(${PROJECT_NAME} STATIC)

# Add sources
target_sources(${PROJECT_NAME} PRIVATE
    ./src/startup.cpp
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
# Expose headers
target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE -D_EXPORT)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-config
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(
        EXPORT ${PROJECT_NAME}-config
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        )

###############################################################################
# Build Target Code
###############################################################################
add_subdirectory(targets)