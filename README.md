# Rocket Simulator

A C++20 orbital launch simulator with:
- physically inspired ascent dynamics,
- modular propulsion models (`base`, `advanced solid`, `advanced liquid`),
- SFML visualization,
- JSON-driven runtime configuration.

The project is prepared for Linux, macOS, and Windows builds through CMake.

---

## 1. What This Program Does

This simulator models a multistage launch vehicle from lift-off to orbital insertion.
At each simulation step it computes:
- atmospheric conditions (`temperature`, `pressure`, `density`),
- engine thrust and propellant mass consumption,
- resultant forces (gravity, drag, thrust contributions),
- updated kinematic state (position, velocity, attitude proxy).

The graphical UI displays:
- altitude and velocity telemetry,
- stage/fuel progress,
- trajectory evolution in multiple views.

Outputs are persisted to text files for offline analysis:
- `assets/output_rocket.txt`
- `assets/output_air.txt`

---

## 2. Core Files and Structure

### Engine and Dynamics

- `engine.h`, `engine.cpp`
  - propulsion interface and concrete engine implementations.
- `rocket.h`, `rocket.cpp`
  - vehicle state, staging logic, force aggregation.
- `simulation.h`, `simulation.cpp`
  - atmospheric model and physical constants.
- `vector_math.h`, `vector_math.cpp`
  - vector helpers used by force/kinematic calculations.

### Interface and Runtime

- `main.cpp`
  - production executable entrypoint.
- `graph_test.cpp`
  - graphics behavior test executable.
- `interface.h`, `interface.cpp`
  - SFML text style, countdown, and runtime support utilities.

### Configuration and Input Data

- `assets/input_data/simulation_params.json`
  - main runtime configuration.
- `assets/input_data/simulation_params.schema.json`
  - schema describing valid configuration structure.
- `assets/input_data/theta_data.txt`
  - data used by trajectory angle shaping logic.

### Assets

- `assets/img/`
- `assets/font/`
- `assets/music/`

---

## 3. Build Requirements

- CMake >= 3.16
- C++20 compiler:
  - Linux/macOS: `g++` or `clang++`
  - Windows: MSVC (Visual Studio 2022) or MinGW

Optional but recommended:
- `ninja-build` for faster builds

---

## 4. Installation (Per Operating System)

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

If SFML is not installed system-wide, CMake can fetch it automatically (see section 6).

---

## 5. Build and Run

### Configure and Build

```bash
cmake -S . -B build
cmake --build build
```

Main executables:
- `build/rocket.t`
- `build/graph_test.t`

### Run

```bash
./build/rocket.t
```

On Windows (PowerShell):

```powershell
.\build\Debug\rocket.t.exe
```

Note: output and input files are under `assets/`, so run from repository root or ensure working directory is set correctly.

---

## 6. Dependency Management (SFML and Toolchain Checks)

The CMake configuration supports two modes:

1. **System SFML mode**  
   CMake tries `find_package(SFML ...)`.

2. **Auto-fetch mode**  
   If SFML is missing and `ROCKET_FETCH_SFML=ON`, CMake downloads SFML via `FetchContent`.

Useful options:
- `-DROCKET_FETCH_SFML=ON|OFF`
- `-DROCKET_ENABLE_SANITIZERS=ON|OFF`

Examples:

```bash
cmake -S . -B build -DROCKET_FETCH_SFML=ON
cmake -S . -B build -DROCKET_ENABLE_SANITIZERS=OFF
```

Sanitizers are enabled only on GCC/Clang Debug builds to avoid MSVC incompatibilities.

---

## 7. Configuration File: Detailed Parameter Guide

The primary runtime config is:
- `assets/input_data/simulation_params.json`

### Top-level keys

- `input_mode`
  - expected source style for runtime parameters.
- `orbit_altitude_m`
  - target orbital altitude in meters.
- `rocket`
  - vehicle geometry, masses, stages, engines-per-stage.
- `engine`
  - propulsion family and detailed parameters.
- `allowed_values`
  - valid enum-like values for selected fields.

### `rocket` section

- `name` (string): rocket name.
- `stage_num` (int): number of liquid stages.
- `mass_structure_kg` (double): dry structural mass.
- `upper_area_m2` (double): reference frontal area for drag.
- `solid_propellant_mass_kg` (double): propellant mass for solid stage.
- `solid_container_mass_kg` (double): mass of solid stage container.
- `solid_engine_count` (int): number of solid engines.
- `liquid_propellant_masses_kg` (double or array): per-stage propellant mass.
- `liquid_container_masses_kg` (array): per-stage container masses.
- `liquid_engines_per_stage` (array): engine count for each liquid stage.

Normalization logic in code expands scalar/single-value arrays when needed to match `stage_num`.

### `engine` section

- `family`:
  - `base`
  - `advanced_solid`
  - `advanced_liquid`
- `base`:
  - `isp_s`
  - `cm`
  - `chamber_pressure_pa`
  - `burn_area_m2`
- `advanced_solid`:
  - `burn_area_m2`
  - `nozzle_throat_area_m2`
  - `chamber_temperature_k`
  - `grain_dimension_m`
  - `grain_density_kg_m3`
  - `burn_rate_a`
  - `burn_rate_n`
  - `propellant_molar_mass_g_mol`
- `advanced_liquid`:
  - `chamber_pressure_pa`
  - `burn_area_m2`
  - `nozzle_throat_area_m2`
  - `chamber_temperature_k`

---

## 8. Typical Usage Workflow

1. Configure `assets/input_data/simulation_params.json`.
2. Build with CMake.
3. Launch `rocket.t`.
4. Provide console inputs requested at startup (engine family selection, orbit altitude if prompted).
5. Inspect generated output files:
   - `assets/output_rocket.txt`
   - `assets/output_air.txt`

For repeatable experiments:
- keep multiple JSON presets,
- copy the selected preset into `simulation_params.json` before each run.

---

## 9. Technical Difficulties Encountered and How They Were Solved

This section documents the concrete engineering issues addressed during the cross-platform update.

### 9.1 Non-portable compiler/linker flags

**Issue:** global flags were injected directly into `CMAKE_CXX_FLAGS*`, including sanitizer options incompatible with some toolchains.  
**Risk:** build failures on MSVC and brittle multi-config behavior.  
**Solution:** moved to target-scoped compile/link options and enabled sanitizers only for GCC/Clang Debug configurations.

### 9.2 SFML linkage differences between environments

**Issue:** environments may expose SFML either as modern targets (`SFML::Graphics`) or legacy libs (`sfml-graphics`).  
**Risk:** configuration succeeds on one OS and fails on another.  
**Solution:** added a compatibility linking function in CMake that supports both target naming styles.

### 9.3 Missing dependencies on fresh machines

**Issue:** first-time setup often fails because SFML is not installed locally.  
**Risk:** blocked onboarding and broken CI reproducibility.  
**Solution:** introduced optional automatic SFML fetch through `FetchContent` when system package discovery fails.

### 9.4 Runtime asset path fragility

**Issue:** relative paths depended on current working directory assumptions.  
**Risk:** executable launches but cannot find images/fonts/audio on IDE/OS variations.  
**Solution:** introduced `ROCKET_ASSETS_DIR` compile definition and centralized asset path resolution using `std::filesystem`.

### 9.5 `M_PI` portability problem

**Issue:** `M_PI` is not guaranteed by all standard library implementations (notably common MSVC setups).  
**Risk:** compile errors on Windows.  
**Solution:** replaced with C++20 `std::numbers::pi_v<double>`.

### 9.6 API declaration inconsistency

**Issue:** function declaration mismatch for `create_ad_eng_minim` in `interface.h`.  
**Risk:** compilation or linkage failures on stricter toolchains.  
**Solution:** updated declaration to match implementation signature.

---

## 10. Troubleshooting

### CMake not found

Install CMake (see section 4), then verify:

```bash
cmake --version
```

### SFML not found

Try:

```bash
cmake -S . -B build -DROCKET_FETCH_SFML=ON
```

### Program starts but no assets are loaded

- Run from repository root, or
- ensure working directory points to the project root where `assets/` exists.

### Push/pull rejected on Git

- fetch and integrate remote branch tip before pushing:

```bash
git fetch origin ultimate
git merge origin/ultimate
git push origin HEAD:ultimate
```

---

## 11. Notes on Visual and Documentation Quality

Special care was applied to:
- coherent section hierarchy,
- operational examples near each concept,
- explicit naming of files used in runtime and build phases,
- actionable troubleshooting steps rather than generic guidance.

This README is intended to be both onboarding material and technical reference.
