include(${EVT_CORE_DIR}/cmake/evt-core_build.cmake)

set(SIM100  sim100)
make_exe(${SIM100} main.cpp)

# Add target sources
target_sources(${SIM100} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/dev/Sim100.cpp)

# Expose headers
target_include_directories(sim100
        PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
