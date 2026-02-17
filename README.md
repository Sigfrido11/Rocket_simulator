# Rocket Simulator

A C++20 orbital launch simulator with:
- physically inspired ascent dynamics,
- modular propulsion models (`base`, `advanced solid`, `advanced liquid`),
- SFML visualization,
- JSON-driven runtime configuration.

The project is prepared for Linux, macOS, and Windows builds through CMake.

---

## 1. What This Program Does

This simulator models a multi‑stage launch vehicle from lift‑off to orbital insertion. It marries a lightweight physics core with a live SFML visualization.

Main loop (per step):
- sample atmosphere (ISA-like profile: temperature, pressure, density vs altitude),
- evaluate propulsion (solid base/advanced, liquid base/advanced),
- compute forces (gravity, centripetal, drag, thrust) and net acceleration,
- integrate motion and update attitude proxy,
- stage management and fuel bookkeeping,
- render telemetry to screen and log to files.

What you see in the UI:
- real-time altitude, speed, stage, fuel,
- trajectory in Cartesian and polar mini-views,
- countdown, launch audio, and background music.

What you get on disk:
- `assets/output_rocket.txt` (position, velocity, forces over time),
- `assets/output_air.txt` (temperature, pressure, density over time).

### 1.1 Physics model (concise)
- Atmosphere: piecewise ISA approximation (see `simulation.cpp`).
- Gravity: altitude-dependent g from Earth radius and mass.
- Drag: quadratic with reference area and air density, uses velocity vector.
- Thrust:
  - Base engine: constant-Isp, mass flow from empirical coefficient.
  - Advanced solid: pressure-dependent regression, isentropic nozzle.
  - Advanced liquid: c* / nozzle model with chamber pressure control.
- Kinematics: simple Euler integration at fixed timestep (1 s default).

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

This section documents the concrete engineering issues addressed during development and the cross-platform hardening.

### 9.1 Build and toolchain portability
- **Problem:** Global `CMAKE_CXX_FLAGS*` contained sanitizer/linker flags that break MSVC and multi-config generators.  
  **Fix:** Moved flags to target-scoped options; sanitized only Debug on GCC/Clang.

### 9.2 Dual SFML naming schemes
- **Problem:** Some distros expose `SFML::Graphics`, others only `sfml-graphics`.  
  **Fix:** A linker helper in CMake selects targets if present, else falls back to library names.

### 9.3 Missing SFML on fresh installs
- **Problem:** New environments often lack SFML, blocking first build.  
  **Fix:** Added `ROCKET_FETCH_SFML` to auto-download SFML via `FetchContent` when not found.

### 9.4 Asset lookup fragility
- **Problem:** Relative paths assumed a specific working directory; assets failed to load on IDE/Windows.  
  **Fix:** Introduced `ROCKET_ASSETS_DIR` and central `asset_path()` using `std::filesystem` so binaries locate assets reliably.

### 9.5 Constant definitions and headers
- **Problem:** `M_PI` is non-standard on MSVC.  
  **Fix:** Replaced with `std::numbers::pi_v<double>`.
- **Problem:** Signature mismatch (`create_ad_eng_minim`) between header and implementation.  
  **Fix:** Corrected declaration to match definition to satisfy stricter compilers.

### 9.6 Data-driven simulation pitfalls
- **Problem:** JSON arrays sometimes provided single values for multi-stage rockets.  
  **Fix:** Added normalization logic that expands scalars or singletons to the required stage count, with validation and clear errors.
- **Problem:** Config parsing lacked early validation.  
  **Fix:** Regex/substring extractors now throw descriptive errors when keys or structures are missing; schema file documents expected shape.

### 9.7 Runtime correctness checks
- **Problem:** Silent physics failures (negative velocity, missing thrust) were hard to spot.  
  **Fix:** Added defensive runtime checks that raise exceptions on invalid states (negative altitude/velocity, zero thrust with active stages), surfacing issues immediately.

### 9.8 Cross-platform developer experience
- **Problem:** IDEs on Windows changed working directory, breaking asset loading during debugging.  
  **Fix:** CMake sets `VS_DEBUGGER_WORKING_DIRECTORY` to project root for MSVC targets.

### 9.9 User feedback and observability
- **Problem:** Without clear logs, users could not tell when asset or audio loading failed.  
  **Fix:** Console messages for each critical load, plus on-screen error window via SFML in exception paths.

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
