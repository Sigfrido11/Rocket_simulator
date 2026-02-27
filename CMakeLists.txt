cmake_minimum_required(VERSION 3.20)
project(rocket VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(FetchContent)

# =============================
# Configuration
# =============================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static linking by default
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# MSVC static runtime
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =============================
# SFML - Always fetch (no system search)
# =============================

set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(sfml)

message(STATUS "SFML fetched and available")

# =============================
# Common compiler settings
# =============================

function(rocket_apply_settings target_name)

    # Warnings
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Sanitizers on non-MSVC debug
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        )
        target_link_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        )
    endif()

    # Assets path
    target_compile_definitions(${target_name} PRIVATE
        ROCKET_ASSETS_DIR="assets"
    )

    # VS debugger working directory
    if(MSVC)
        set_target_properties(${target_name} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endif()

endfunction()

# =============================
# Main executable
# =============================

add_executable(rocket.t
    main.cpp
    rocket.cpp
    simulation.cpp
    interface.cpp
    vector_math.cpp
    engine.cpp
)

rocket_apply_settings(rocket.t)

# Link SFML (uses legacy targets from FetchContent)
target_link_libraries(rocket.t PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# =============================
# Tests
# =============================

if(BUILD_TESTING)

    # Graph test
    add_executable(graph_test.t
        graph_test.cpp
        simulation.cpp
        interface.cpp
    )
    rocket_apply_settings(graph_test.t)
    target_link_libraries(graph_test.t PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
    )
    add_test(NAME graph_test COMMAND graph_test.t)

    # Unit tests
    add_executable(unit_tests.t
        unit_tests.cpp
        rocket.cpp
        simulation.cpp
        vector_math.cpp
        engine.cpp
    )
    rocket_apply_settings(unit_tests.t)
    add_test(NAME unit_tests COMMAND unit_tests.t)

endif()

# =============================
# Installation
# =============================

# Pre-create install directory to avoid permission issues
file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}")

install(TARGETS rocket.t
    RUNTIME DESTINATION .
)

install(DIRECTORY assets/
    DESTINATION assets
)

# =============================
# CPack
# =============================

set(CPACK_PACKAGE_NAME "rocket")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Giuseppe Luciano")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rocket Flight Simulator")

set(CPACK_PACKAGE_FILE_NAME
    "rocket-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

# Skip SFML installation
set(CPACK_INSTALL_CMAKE_SKIP_RPATH YES)

include(CPack)