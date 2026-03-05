# Rocket Launch Phase Simulator

The purpose of this project is to simulate a launch vehicle from liftoff, occurring at the equatorial line, up to the possible achievement of orbit. This project required the coherent coupling of aerodynamics, thermodynamics, and orbital mechanics within a numerically stable scheme.

This simulator, developed in **C++20** with **SFML**, aims to model a physically consistent ascent while avoiding purely kinematic simplifications.

The project repository can be viewed and downloaded by clicking the button below.

<div class="rocket_repo">
  <a class="btn" href="https://github.com/Sigfrido11/Rocket_simulator" target="_blank" rel="noopener">
    Visit the repository
  </a>
</div>

## Build and Dependencies (macOS, Windows, Linux)

This project uses **CMake** and requires a **C++20** compiler.

### Required Tools

- **CMake 3.21+**
- A **C++20 compiler**:
  - GCC 10+ or Clang 12+ on Linux/macOS
  - MSVC (Visual Studio 2022 or newer) on Windows
- A build backend:
  - `Ninja` (recommended), or
  - platform-native tools (`make`, Visual Studio/MSBuild, etc.)
- **Git** (required only if SFML is fetched automatically)

### SFML Dependency Model

The CMake configuration supports two valid approaches:

1. **Use system-installed SFML (preferred when available)**  
   CMake tries `find_package(SFML 2.5 COMPONENTS graphics window system audio)`.
2. **Fetch SFML automatically from source**  
   If SFML is not found and `ROCKET_FETCH_SFML=ON` (default), CMake downloads SFML `2.6.2` via `FetchContent`.

### Platform Setup

Install the required tools with your preferred package manager.

- **macOS (Homebrew)**
  - `brew install cmake ninja git sfml`
- **Ubuntu/Debian**
  - `sudo apt update`
  - `sudo apt install -y cmake ninja-build build-essential git libsfml-dev`
- **Fedora**
  - `sudo dnf install -y cmake ninja-build gcc-c++ git SFML-devel`
- **Arch Linux**
  - `sudo pacman -S --needed cmake ninja base-devel git sfml`
- **Windows**
  - Install **Visual Studio 2022** with *Desktop development with C++*
  - Install **CMake** and optionally **Ninja**
  - Optional (system SFML): `winget install SFML.SFML`
  - If SFML is not installed system-wide, keep `ROCKET_FETCH_SFML=ON` (default)

### Configure and Build

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

Notes:
- `-DCMAKE_BUILD_TYPE=Release` is used by single-config generators (Ninja/Makefiles).
- `--config Release` is used by multi-config generators (Visual Studio, Xcode).

### Run

- **Linux/macOS**
  - `./build/rocket.t`
- **Windows (Visual Studio generator)**
  - `.\build\Release\rocket.t.exe`

Run from the project root so the executable can resolve the `assets/` directory correctly.

### Build Options

- `-DBUILD_TESTING=OFF` disables test targets (`graph_test.t`, `unit_tests.t`).
- `-DROCKET_ENABLE_SANITIZERS=ON` enables ASan/UBSan in **Debug** builds for GCC/Clang (non-MSVC).
- `-DROCKET_FETCH_SFML=OFF` requires SFML to be already installed.
- `-DROCKET_FORCE_FETCH_SFML=ON` always builds SFML from source.
- `-DROCKET_FETCH_SFML_SHARED=ON` fetches SFML as shared libraries (default is static).

### Run Tests

```bash
ctest --test-dir build -C Release --output-on-failure
```

## How to Use the Program

The considerable complexity of the program required the use of several input parameters that the user must provide at the start of the simulation.  
To prevent the software from becoming inaccessible to non-expert users and to simplify data entry, at startup the user is asked whether to use the **base engine model**, a simplified version with few parameters, or an **advanced version** that requires knowledge of many parameters.

These parameters can be conveniently edited by the user in the file:

`assets/input_data/simulation_params.json`

For convenience, reasonable default values are already set.

Below is a brief description of all configuration parameters.  
**Warning:** if advanced engines are selected, modifying the base engine section will have no effect and viceversa.

---

## Configuration Parameters

| Parameter | Description |
|------------|-------------|
| Rocket |
| `rocket.name` | Identifier name of the launch vehicle. |
| `rocket.stage_num` | Total number of stages (1 initial solid stage + N liquid stages). |
| `rocket.mass_structure_kg` | Main structural mass of the vehicle, excluding propellant and tanks. |
| `rocket.upper_area_m2` | Equivalent frontal area used in aerodynamic drag calculations. |
| `rocket.solid_propellant_mass_kg` | Total propellant mass of the solid propellant stage. |
| `rocket.solid_container_mass_kg` | Structural mass of the solid propellant booster, discarded at separation. |
| `rocket.solid_engine_count` | Number of solid propellant  engines firing simultaneously. |
| `rocket.liquid_propellant_masses_kg` | List of propellant masses for each liquid propellant  stage. |
| `rocket.liquid_container_masses_kg` | List of structural tank masses for each liquid propellant stage. |
| `rocket.liquid_engines_per_stage` | Number of liquid propellant engines active in each stage. |
| Base Engines |
| `engine.base.isp_s` | Specific impulse expressed in seconds. |
| `engine.base.cm` | Mass flow coefficient. |
| `engine.base.chamber_pressure_pa` | Combustion chamber pressure (Pa). |
| `engine.base.burn_area_m2` | Effective propellant burning area in the base model. |
| Advanced Solid Engines |
| `engine.advanced_solid.burn_area_m2` | Initial burning surface area of the solid grain. |
| `engine.advanced_solid.nozzle_throat_area_m2` | Nozzle throat area of the solid propellant  engine (controls mass flow). |
| `engine.advanced_solid.nozzle_exit_area_m2` | Nozzle exit area of the solid propellant engine (affects gas expansion). |
| `engine.advanced_solid.chamber_temperature_k` | Combustion chamber gas temperature (K). |
| `engine.advanced_solid.grain_dimension_m` | Characteristic grain dimension affecting burn evolution. |
| `engine.advanced_solid.grain_density_kg_m3` | Solid propellant density. |
| `engine.advanced_solid.burn_rate_a` | Solid propellant burn rate coefficient. |
| `engine.advanced_solid.burn_rate_n` | Pressure exponent in the solid propellant regression law. |
| `engine.advanced_solid.propellant_molar_mass_g_mol` | Molar mass of solid propellant engine combustion products (g/mol). |
| Advanced Liquid Engines |
| `engine.advanced_liquid.chamber_pressure_pa` | Combustion chamber pressure of the liquid engine (Pa). |
| `engine.advanced_liquid.nozzle_throat_area_m2` | Nozzle throat area of the liquid propellant engine. |
| `engine.advanced_liquid.nozzle_exit_area_m2` | Nozzle exit area of the liquid propellant engine. |
| `engine.advanced_liquid.chamber_temperature_k` | Combustion chamber gas temperature of the liquid propellant engine (K). |
| `engine.advanced_liquid.propellant_molar_mass_g_mol` | Molar mass of combustion gases in the liquid propellant engine (g/mol). |

The user will also be asked to specify the altitude at which the rocket will attempt orbital insertion.  
This altitude **must** be greater than 60,000 m.

---

# Main File Organization

* Rocket → rocket management, dynamics, and kinematics  
* Engine → physics of all selectable engine types  
* Atmosphere → simulation of atmospheric parameters at different altitudes  

---

# Core Dynamic Implementation: `rocket.cpp`

The file `rocket.cpp` represents the dynamic and kinematic core of the simulator:  
it integrates **propulsion, aerodynamics, gravity, stage management, and numerical integration**, thus governing the time evolution of the simulation.

---

## Choice of Reference Frame

The simulation uses an **inertial frame centered at the Earth's center**, expressed in polar coordinates:

$$
(r, \psi)
$$

where:

* ( r ) = distance from Earth's center  
* ( $\psi$ ) = polar angle  
* ( $v_r$ ) = radial velocity  
* ( $v_t$ ) = tangential velocity  

### Why not a Cartesian system?

A Cartesian system would require:

* Explicit handling of Earth curvature  
* Continuous normalization of the gravity vector  
* Greater numerical instability at high altitude  
* Increased difficulty in graphical visualization of the rocket position  

---

# Numerical Integration: Runge–Kutta 2 (Midpoint Method)

The file uses an **RK2 (midpoint method)** integrator to update velocity and position.

### Why not explicit Euler?

The classical Euler integrator:

$$
x_{n+1} = x_n + v_n \Delta t
$$

introduces:

* Poorer energy conservation during time evolution  
* Instability in orbital regimes  
* Amplified errors in centrifugal terms ( $\frac{v_t^2}{r}$ )

### Implemented Equations

In inertial polar coordinates:

$$
\dot{v_r} = \frac{F_r}{m} - \frac{v_t^2}{r}
$$

and

$$
\dot{v_t} = \frac{F_t}{m} + \frac{v_r v_t}{r}
$$

The term:

$$
\frac{v_t^2}{r}
$$

is the geometric centripetal acceleration.

The term:

$$
\frac{v_r v_t}{r}
$$

is the geometric coupling term.

---

## Advantages of RK2

* Reduces error during time evolution ($O(\Delta t^3)$ compared to $O(\Delta t^2)$ for Euler)  
* Maintains good orbital stability  
* Balanced compromise between accuracy and computational cost  

---

## Stage Management

The program can handle stage separation. However, note that due to the current design, **the solid fuel stage must be detached before the first liquid stage**.

Separation is handled by predicting how much propellant will be consumed in the next iteration and attempting to use as much propellant as possible.

In the current version, it is not possible to manually control the thrust level of each engine during the simulation.  
This makes orbit insertion significantly more challenging and reduces interactivity.  

A future version is planned in which the user will have greater control over the launch phase.

---

## Thrust Direction Control

To achieve an optimal pitch angle evolution, rather than defining a regression law analytically, real mission telemetry data was used as reference.

Specifically, telemetry data from several missions, especially Falcon 9, was analyzed, and an average pitch profile as a function of altitude was derived.

```cpp
improve_theta(...)
````

Reads an altitude–angle guidance file.

Design choice:

* Data-driven pitch profile
* Complete separation between guidance and dynamics

Challenges encountered:

* Need to store `file_pos`
* Avoid continuous file rewind
* Stabilize angle changes above 20 km

---

## Mach-Dependent Aerodynamics and Transonic Regime

Drag force is modeled as:

$$
F_d = \frac{1}{2} \rho v^2 C_d(M) A
$$

The main complexity lies in the dependence of the drag coefficient $$C_d(M)$$ on Mach number.

### Drag Divergence

Near Mach 1:

* Shock waves form
* Wave drag increases sharply
* $C_d$ becomes strongly nonlinear

A piecewise definition introduces undesirable numerical discontinuities.

### Smooth Regime Transitions

To avoid numerical artifacts, the following function is implemented:

```cpp
double Cd_from_Mach(double M);
```

The function uses smooth transitions based on hyperbolic tangents:

$$
C_d(M) =
C_{sub}

* \frac{C_{trans} - C_{sub}}{2} \left(1 + \tanh(k_1(M - M_1))\right)
* \frac{C_{sup} - C_{trans}}{2} \left(1 + \tanh(k_2(M - M_2))\right)
* \dots
  $$

This guarantees:

* At least $C^1$ continuity
* Smooth acceleration profiles
* Numerical stability

The modeled regimes are:

* Subsonic
* Transonic
* Supersonic
* Hypersonic

---

# Modeling of Main Atmospheric Parameters `simulation.cpp`

A simple exponential decay of density is insufficient for a realistic ascent simulation.
Aerodynamic drag and propulsion performance critically depend on the local thermodynamic state.

### International Standard Atmosphere (ISA)

The simulator implements the **ISA model**, computing atmospheric properties layer by layer:

* Troposphere
* Tropopause
* Stratosphere (up to ~51 km, extendable)

Each layer is modeled using:

Hydrostatic equilibrium: $\frac{dP}{dh} = -\rho g$

Ideal gas law: $ P = \rho R T $

From these relations, altitude-dependent quantities are derived:

* Pressure $P(h)$
* Density $\rho(h)$
* Temperature $T(h)$
* Local speed of sound $a(h)$

For more technical details, see: <a href="https://agodemar.github.io/FlightMechanics4Pilots/mypages/international-standard-atmosphere/">ISA model</a>.

---
# Engine Modeling (`engine.cpp`)

The file `engine.cpp` implements different levels of propulsion complexity:

1. **Base Engine** – constant specific impulse model  
2. **Advanced Solid Engine** – full internal ballistics coupled with chamber pressure  
3. **Advanced Liquid Engine** – choked flow model with isentropic relations  

The **architecture is polymorphic**: each engine exposes the same public interface, allowing the user complete freedom of choice:

- `delta_m(dt)` → mass lost consumption  
- `eng_force(pa, time, theta)` → thrust vector  

For the mathematical modeling, the following references were used:

- <a href="https://www.grc.nasa.gov/www/k-12/airplane/rktthsum.html"> NASA Glenn Research Center – Rocket Thrust Equations </a>  
- <a href="https://www.grc.nasa.gov/www/k-12/airplane/isentrop.html"> NASA Glenn – Isentropic Flow Relations </a>  
- Sutton & Biblarz, *Rocket Propulsion Elements*, 9th Edition  

---

## Base Engine

This model assumes:

- Constant chamber pressure  
- Constant mass loss  
- Constant specific impulse  

Mass Flow Rate

$$
\dot{m} = p_0 \cdot A_{burn} \cdot c_m
$$

where:

- \( $p_0$ \) = nominal combustion chamber pressure  
- \( $A_{burn}$ \) = burning surface area  
- \( $c_m$ \) = empirical mass loss coefficient  

Fuel consumption per timestep:

$$
\Delta m = \dot{m} \Delta t
$$

Thrust

$$
F = \dot{m} I_{sp} g_0
$$

where:

- \( $I_{sp}$ \) = specific impulse  
- \( $g_0$ \) = standard gravity acceleration (m/s²)  

---

## Advanced Solid Engine

This model implements the internal physics of a solid propellant rocket motor.

Propellant Regression Law (Saint-Robert’s Law):

$$
\dot{r} = a P_c^n
$$

where:

- \( a \) = burn rate coefficient  
- \( n \) = pressure exponent  
- \( $P_c$ \) = chamber pressure  

Generated Mass Flow Rate

$$
\dot{m}_{gen} = \rho_p A_b \dot{r}
$$

where:

- \( \$rho_p$ \) = propellant density  
- \( $A_b$ \) = burning area  

Mass Flow Through the Nozzle (Choked Flow)

$$
\dot{m}_{noz} = \frac{P_c A_t}{c^*}
$$

where:

- \( $A_t$ \) = throat area  
- \( $c^*$ \) = characteristic velocity  

Mass Balance

$$
\frac{dP_c}{dt} =
\frac{R_{spec} T_c}{V_c}
(\dot{m}_{gen} - \dot{m}_{noz})
$$

where:

$$
R_{spec} = \frac{R}{M}
$$

Characteristic Velocity

$$
c^* =
\sqrt{
\left(\frac{2}{\gamma + 1}\right)^{\frac{\gamma + 1}{\gamma - 1}}
R_{spec} T_c
}
$$

Exit Mach Number Calculation

Numerical solution of the area–Mach equation:

$$
\frac{A_e}{A_t}
=
\frac{1}{M_e}
\left(
\frac{2}{\gamma+1}
\left(1 + \frac{\gamma-1}{2} M_e^2\right)
\right)^{\frac{\gamma+1}{2(\gamma-1)}}
$$

The equation is solved numerically using the Newton–Raphson method to obtain the supersonic exit Mach number \( $M_e$ \).

Exit Pressure

$$
\frac{P_e}{P_c}
=
\left(
1 + \frac{\gamma-1}{2} M_e^2
\right)^{-\frac{\gamma}{\gamma-1}}
$$

Exhaust Velocity

$$
v_e =
\sqrt{
\frac{2\gamma}{\gamma - 1}
R_{spec} T_c
\left(
1 -
\left(\frac{P_e}{P_c}\right)^{\frac{\gamma - 1}{\gamma}}
\right)
}
$$

Total Thrust

$$
F =
\dot{m} v_e
+
(P_e - P_a) A_e
$$

The second term represents the pressure contribution due to the difference between exit pressure and ambient pressure.

---

## Advanced Liquid Engine

Assumptions:

- Constant chamber pressure  
- Choked flow at the throat  
- Isentropic expansion  

Mass Flow Rate

$$
\dot{m} =
\frac{P_c A_t}{c^*}
$$

Exit Mach Number

Determined by numerically solving the same area–Mach equation:

$$
\frac{A_e}{A_t}
=
\frac{1}{M_e}
\left(
\frac{2}{\gamma+1}
\left(1 + \frac{\gamma-1}{2} M_e^2\right)
\right)^{\frac{\gamma+1}{2(\gamma-1)}}
$$

Exit Pressure

$$
P_e =
P_c
\left(
1 + \frac{\gamma - 1}{2} M_e^2
\right)^{-\frac{\gamma}{\gamma - 1}}
$$

Exhaust Velocity

$$
v_e =
\sqrt{
\frac{2\gamma}{\gamma - 1}
\frac{R T_c}{M}
\left(
1 -
\left(\frac{P_e}{P_c}\right)^{\frac{\gamma - 1}{\gamma}}
\right)
}
$$

Thrust

$$
F =
\dot{m} v_e
+
(P_e - P_a) A_e
$$

---

# Differences Between Models

| Model | Complexity | Pressure Evolution | Realism |
|----------|------------|----------------------|----------|
| Base | Low | Constant | Approximate |
| Advanced Solid | High | Dynamic | High |
| Advanced Liquid | Medium/High | Constant (assumed) | Realistic |

The file `engine.cpp` represents the most sophisticated modeling layer of the simulator.

The objective is not merely to compute thrust,  
but to reproduce the coupling between:

- Combustion gas thermodynamics  
- Chamber pressure dynamics  
- Compressible flow in the nozzle  
- Interaction with ambient pressure  

---

# Conclusion

The project evolved from a simple ascent simulator into a structured implementation of:

- Layered atmospheric thermodynamics  
- Internal propulsion modeling  
- Orbital mechanics  
- Numerical integration  

Particular importance was given to the rigorous separation between propulsion, aerodynamics, and kinematics through polymorphism, ensuring improved architectural scalability.

The result is a numerically consistent representation of the real engineering and physical challenges involved in reaching orbit.

## 👥 Contributors

This project was made possible thanks to the contribution of:

- **Sigfrido11**  🔗 [GitHub](https://github.com/Sigfrido11)  
- **SierraTangoEcho** 🔗 [GitHub](https://github.com/SierraTangoEcho)
