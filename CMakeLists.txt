cmake_minimum_required(VERSION 3.16)
project(rocket VERSION 0.1.0)

# abilita il supporto per i test, tra cui l'opzione BUILD_TESTING usata sotto
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# richiedi l'uso di C++17, senza estensioni non-standard offerte dal compilatore usato 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")

# abilita l'address sanitizer e l'undefined-behaviour sanitizer in debug mode
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")
string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer -std=c++20")

add_executable(rocket.t main.cpp rocket.cpp simulation.cpp interface.cpp vector_math.cpp)
target_link_libraries(rocket.t sfml-graphics sfml-window sfml-system sfml-audio)
# se il testing e' abilitato...
#   per disabilitare il testing, passare -DBUILD_TESTING=OFF a cmake durante la fase di configurazione
if (BUILD_TESTING)

# aggiungi l'eseguibile 
#add_executable(rocket_test.t test_calc.cpp simulation.cpp rocket.cpp)
add_executable(graph_test.t graph_test.cpp simulation.cpp interface.cpp)
target_link_libraries(graph_test.t sfml-graphics sfml-window sfml-system)
add_test(NAME rocket_test COMMAND rocket_test)

endif()
