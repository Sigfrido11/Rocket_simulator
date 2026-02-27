cmake_minimum_required(VERSION 3.16)
project(rocket VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(FetchContent)

# =============================
# Static Linking Configuration
# =============================

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ROCKET_ENABLE_SANITIZERS "Enable ASAN/UBSAN in Debug builds" ON)
option(ROCKET_FETCH_SFML "Fetch SFML automatically if not found" ON)

# =============================
# SFML
# =============================

set(SFML_COMPONENTS graphics window system audio)

find_package(SFML 2.5 COMPONENTS ${SFML_COMPONENTS} QUIET)

if(NOT SFML_FOUND AND ROCKET_FETCH_SFML)

    message(STATUS "SFML not found, fetching via FetchContent")

    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(sfml)

endif()

# =============================
# Common settings
# =============================

function(rocket_apply_common_settings target_name)

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(ROCKET_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
            $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        )

        target_link_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
            $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        )
    endif()

    target_compile_definitions(${target_name}
        PRIVATE ROCKET_ASSETS_DIR="assets"
    )

    if(MSVC)
        set_target_properties(${target_name}
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

endfunction()

# =============================
# SFML Linking
# =============================

function(rocket_link_sfml target_name enable_audio)

    target_link_libraries(${target_name} PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
    )

    if(${enable_audio})
        target_link_libraries(${target_name} PRIVATE sfml-audio)
    endif()

endfunction()

# =============================
# Executables
# =============================

add_executable(rocket.t
    main.cpp
    rocket.cpp
    simulation.cpp
    interface.cpp
    vector_math.cpp
    engine.cpp
)

rocket_apply_common_settings(rocket.t)
rocket_link_sfml(rocket.t TRUE)

# =============================
# Windows DLL Copy
# =============================

if(WIN32)
    add_custom_command(TARGET rocket.t POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE_DIR:rocket.t>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE_DIR:rocket.t>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:rocket.t>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-audio>
        $<TARGET_FILE_DIR:rocket.t>
    )
endif()

# =============================
# Tests
# =============================

if(BUILD_TESTING)

    add_executable(graph_test.t
        graph_test.cpp
        simulation.cpp
        interface.cpp
    )

    rocket_apply_common_settings(graph_test.t)
    rocket_link_sfml(graph_test.t FALSE)

    add_test(NAME graph_test COMMAND graph_test.t)

    add_executable(unit_tests.t
        unit_tests.cpp
        rocket.cpp
        simulation.cpp
        vector_math.cpp
        engine.cpp
    )

    rocket_apply_common_settings(unit_tests.t)

    add_test(NAME unit_tests COMMAND unit_tests.t)

endif()

# =============================
# Installation & Packaging
# =============================

install(TARGETS rocket.t RUNTIME DESTINATION .)
install(DIRECTORY assets/ DESTINATION assets)

# CPack
set(CPACK_PACKAGE_NAME "rocket")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "GitHub")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rocket simulator")

set(CPACK_PACKAGE_FILE_NAME
    "rocket-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include(CPack)