cmake_minimum_required(VERSION 3.21)
project(rocket VERSION 0.1.0 LANGUAGES CXX)

# abilita il supporto per i test, tra cui l'opzione BUILD_TESTING usata sotto
include(CTest)
include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# richiedi l'uso di C++20, senza estensioni non-standard offerte dal compilatore usato
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ROCKET_ENABLE_SANITIZERS "Enable ASAN/UBSAN in Debug builds (non-MSVC)" ON)
option(ROCKET_FETCH_SFML "Fetch SFML automatically if not found on the system" ON)
option(ROCKET_FORCE_FETCH_SFML "Always build SFML from source instead of using find_package" OFF)
option(ROCKET_FETCH_SFML_SHARED "Build fetched SFML as shared libraries" OFF)

set(SFML_COMPONENTS graphics window system audio)
if (NOT ROCKET_FORCE_FETCH_SFML)
  find_package(SFML 2.5 COMPONENTS ${SFML_COMPONENTS} QUIET)
endif ()

if ((NOT SFML_FOUND) AND ROCKET_FETCH_SFML)
  message(STATUS "Using FetchContent to download/build SFML")

  set(SFML_INSTALL OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
  set(BUILD_SHARED_LIBS ${ROCKET_FETCH_SFML_SHARED} CACHE BOOL "" FORCE)
  if (NOT ROCKET_FETCH_SFML_SHARED)
    set(SFML_USE_STATIC_LIBS TRUE CACHE BOOL "" FORCE)
  endif ()

  FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(sfml)
endif ()

if (NOT SFML_FOUND AND NOT TARGET sfml-graphics AND NOT TARGET SFML::Graphics)
  message(FATAL_ERROR "SFML not found. Install SFML or configure with -DROCKET_FETCH_SFML=ON")
endif ()

function(rocket_apply_common_settings target_name)
  if (MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else ()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
  endif ()

  if (ROCKET_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${target_name}
      PRIVATE
      $<$<CONFIG:Debug>:-fsanitize=address,undefined>
      $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )
    target_link_options(
      ${target_name}
      PRIVATE
      $<$<CONFIG:Debug>:-fsanitize=address,undefined>
      $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )
  endif ()

 target_compile_definitions(
  ${target_name}
  PRIVATE
  ROCKET_ASSETS_DIR="assets"
)
  if (MSVC)
    set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif ()
endfunction()

function(rocket_link_sfml target_name enable_audio)
  if (TARGET SFML::Graphics)
    target_link_libraries(${target_name} PRIVATE SFML::Graphics SFML::Window SFML::System)
    if (enable_audio)
      target_link_libraries(${target_name} PRIVATE SFML::Audio)
    endif ()
  else ()
    target_link_libraries(${target_name} PRIVATE sfml-graphics sfml-window sfml-system)
    if (enable_audio)
      target_link_libraries(${target_name} PRIVATE sfml-audio)
    endif ()
  endif ()
endfunction()

add_executable(rocket.t main.cpp rocket.cpp simulation.cpp interface.cpp vector_math.cpp engine.cpp)
rocket_apply_common_settings(rocket.t)
rocket_link_sfml(rocket.t TRUE)
# se il testing e' abilitato...
#   per disabilitare il testing, passare -DBUILD_TESTING=OFF a cmake durante la fase di configurazione
if (BUILD_TESTING)

  # aggiungi l'eseguibile per il test grafico (SFML)
  add_executable(graph_test.t graph_test.cpp simulation.cpp interface.cpp)
  rocket_apply_common_settings(graph_test.t)
  rocket_link_sfml(graph_test.t FALSE)
  add_test(NAME graph_test COMMAND graph_test.t)

  # aggiungi eseguibile per i test unitari (doctest)
  add_executable(unit_tests.t unit_tests.cpp rocket.cpp simulation.cpp vector_math.cpp engine.cpp)
  rocket_apply_common_settings(unit_tests.t)
  # no SFML needed for unit tests
  add_test(NAME unit_tests COMMAND unit_tests.t)

endif()

# --- Installation and Packaging ---

# Installa l'eseguibile nella root del pacchetto
install(
  TARGETS rocket.t
  RUNTIME DESTINATION .
  BUNDLE DESTINATION .
)

if (WIN32)
  # Copia eventuali DLL runtime (es. SFML/OpenAL) nella root del pacchetto.
  install(FILES $<TARGET_RUNTIME_DLLS:rocket.t> DESTINATION .)
endif ()

# Copia la cartella assets nella root del pacchetto
install(DIRECTORY assets/ DESTINATION assets)

# --- Configurazione CPack ---
set(CPACK_PACKAGE_NAME "rocket")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "GitHub")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple rocket simulator")
set(CPACK_PACKAGE_FILE_NAME "rocket-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

if (NOT DEFINED CPACK_GENERATOR OR CPACK_GENERATOR STREQUAL "")
  set(CPACK_GENERATOR "ZIP")
endif ()

set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${PROJECT_NAME};ALL;/")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)

include(CPack)
