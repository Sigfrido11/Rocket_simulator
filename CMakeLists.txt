cmake_minimum_required(VERSION 3.16)
project(rocket VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(FetchContent)

# =============================
# Configuration options
# =============================

option(ROCKET_ENABLE_SANITIZERS "Enable ASAN/UBSAN in Debug builds" ON)
option(ROCKET_FETCH_SFML "Fetch SFML automatically if not found" ON)
option(ROCKET_USE_STATIC_SFML "Link SFML statically (no DLLs on Windows)" ON)

# prefer static runtime on MSVC so we don't pull in the dynamic CRT
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Define SFML_STATIC when we are linking the library statically.  This must be
# exported before we pull in SFML headers (either via find_package or
# FetchContent) so that users of the library see the correct __declspec
# attributes.
if(ROCKET_USE_STATIC_SFML)
    add_compile_definitions(SFML_STATIC)
endif()

# globally default to static libraries to avoid accidental dynamic linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# when we fetch SFML locally we also ask it to build its static variants
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

# when we request fetching we set the same options that the old CMakeLists
# used; in particular we propagate the static/shared choice through
if(ROCKET_FETCH_SFML)
    message(STATUS "ROCKET_FETCH_SFML=ON: configuring FetchContent options for SFML")
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_LIBS ${ROCKET_USE_STATIC_SFML} CACHE BOOL "" FORCE)
    # prevent upstream project from trying to install files to system
    set(SFML_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(sfml)
endif()

# look for whatever version of SFML we have available (fetched or system)
find_package(SFML 2.5 COMPONENTS ${SFML_COMPONENTS} QUIET)

if(NOT SFML_FOUND)
    message(STATUS "SFML not found on system")
    if(NOT TARGET sfml-graphics AND NOT TARGET SFML::Graphics)
        message(FATAL_ERROR "SFML could not be located. Try -DROCKET_FETCH_SFML=ON")
    endif()
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

    # prefer modern imported targets from find_package/fetchcontent
    if(TARGET SFML::Graphics)
        target_link_libraries(${target_name} PRIVATE
            SFML::Graphics
            SFML::Window
            SFML::System
        )
        if(${enable_audio})
            target_link_libraries(${target_name} PRIVATE SFML::Audio)
        endif()
    else()
        # fall back to legacy names (should only happen when SFML built in-tree)
        target_link_libraries(${target_name} PRIVATE
            sfml-graphics
            sfml-window
            sfml-system
        )
        if(${enable_audio})
            target_link_libraries(${target_name} PRIVATE sfml-audio)
        endif()
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
# Windows DLL Copy (only when linking dynamically)
# =============================

if(WIN32 AND NOT ROCKET_USE_STATIC_SFML)
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

# if we are linking dynamically we also need to ship the SFML binaries (DLLs
# on Windows, frameworks on macOS) alongside the executable so that the end
# user can run the program without manually installing SFML.
if(NOT ROCKET_USE_STATIC_SFML)
    if(WIN32)
        install(FILES
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-audio>
            DESTINATION .
        )
    elseif(APPLE)
        # frameworks are treated as directories; copy them so the bundle is
        # self-contained.  CPack must be run with a generator that packages
        # directories (e.g. ZIP, TGZ).
        install(FILES
            $<TARGET_FILE:SFML::Graphics>
            $<TARGET_FILE:SFML::Window>
            $<TARGET_FILE:SFML::System>
            $<TARGET_FILE:SFML::Audio>
            DESTINATION .
        )
    endif()
endif()

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