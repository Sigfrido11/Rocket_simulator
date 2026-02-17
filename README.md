# Rocket Simulator (Cross-Platform Setup)

This project is configured to run on Linux, macOS, and Windows.
It uses CMake and SFML, with automatic SFML download if the library is not already installed.

## Project Files You Asked To Keep

- Engine implementation:
  - `engine.h`
  - `engine.cpp`
- Simulation config:
  - `assets/input_data/simulation_params.json`
  - `assets/input_data/simulation_params.schema.json`
  - `assets/input_data/theta_data.txt`

## Requirements

- CMake >= 3.16
- C++20 compiler
  - Linux/macOS: `g++` or `clang++`
  - Windows: MSVC (Visual Studio 2022) or MinGW

## Install Toolchain

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y cmake g++ ninja-build libsfml-dev
```

### macOS (Homebrew)

```bash
brew install cmake ninja sfml
```

### Windows (PowerShell + winget)

```powershell
winget install Kitware.CMake
winget install Microsoft.VisualStudio.2022.BuildTools
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

Main executables:
- `build/rocket.t`
- `build/graph_test.t`

## SFML Handling

By default:
- CMake first tries `find_package(SFML ...)`
- If SFML is missing, it auto-downloads SFML with `FetchContent`

Useful options:
- `-DROCKET_FETCH_SFML=ON|OFF`
- `-DROCKET_ENABLE_SANITIZERS=ON|OFF`

## Runtime Assets

Assets are loaded from `assets/` through a compile definition (`ROCKET_ASSETS_DIR`), so paths are consistent across operating systems and IDEs.
