#ifndef ROCKET_H
#define ROCKET_H

#include <algorithm>
#include <fstream>
#include <cmath>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "assert.h"
#include "simulation.h"
#include "vector_math.h"
#include "engine.h"

namespace rocket {

class Rocket {
  // Rocket basic configuration and state parameters

// Rocket name identifier
std::string name_{"my_rocket"};

// Cross-sectional upper area of the rocket (m^2)
double upper_area_{80.0};

// Solid propulsion system parameters

// Mass of the solid fuel container (kg)
double m_sol_cont_{8000.0};

// Mass of the solid propellant (kg)
double m_sol_prop_{60000.0};

// Liquid propulsion system parameters (per stage or per tank)

// Mass of liquid propellant (kg)
// Stored as a vector to support multi-stage rockets
std::vector<double> m_liq_prop_{15000.0};

// Mass of liquid fuel container/tank (kg)
// Stored as a vector to support multi-stage rockets
std::vector<double> m_liq_cont_{40000.0};

// Total initial mass of the rocket (kg)
double total_mass_{68000.0};

// Total number of rocket stages
int total_stage_{1};

// Current velocity vector (m/s) (r/psi)
Vec velocity_{0.0, 0.0};

// Current position vector (m) (r,psi)
Vec pos_{sim::cost::earth_radius_, 0.0};

// Index of the currently active stage
int current_stage_{1};

// Rocket inclination angle (radians)
// Initialized to pi/2 ≈ 1.57079632 (vertical launch)
double theta_{1.57079632};

// Pointer to the engine currently associated with the rocket
engine::Engine* engs_;

engine::Engine* engl_;

// Number of solid engines
int n_sol_eng_{1};

// Number of liquid engines per stage
// Stored as a vector to support multi-stage configurations
std::vector<int> n_liq_eng_;

 public:
  // costruttore con tutto
  explicit Rocket(std::string const& name, double mass_structure, double Up_Ar,
               double s_p_m, double m_s_cont, std::vector<double> const& l_p_m,
               std::vector<double> const& l_c_m, engine::Engine* engs, engine::Engine* engl,
               int n_solid_eng, std::vector<int> const& n_liq_eng);

  Rocket() = default;

  /* -------------------------------------------------------------------------- */
/*                                Rocket getters methods                              */
/* -------------------------------------------------------------------------- */

  Vec const get_velocity() const;

  Vec const get_pos() const;

  void mass_lost(double solid_lost, double liq_lost);

  double get_theta() const;

  void move(double time, Vec const& force);

  double get_up_ar() const;

  int get_rem_stage() const;

  double get_fuel_left() const;

  double get_mass() const;

  double get_altitude() const;

/* -------------------------------------------------------------------------- */
/*                               kinematics rocket                              */
/* -------------------------------------------------------------------------- */

  void set_state(std::ifstream& theta_file, double orbital_h, double time,
                       bool is_orbiting, std::streampos& file_pos);

  void stage_release(double delta_ms, double delta_ml);  

  void change_vel(double time, Vec const& force);

  Vec const thrust(double time, double pe, double pa, bool is_orbiting) const; //engine thrust
};


/* -------------------------------------------------------------------------- */
/*                               Force computation                              */
/* -------------------------------------------------------------------------- */

Vec const total_force(double rho, double total_mass, double altitude,
                      double upper_area, Vec const& velocity, Vec const& eng, double a);


double improve_theta(std::ifstream& file, double theta, double pos,
                     double orbital_h, std::streampos& file_pos);

bool is_orbiting(double r, Vec velocity);

Vec g_force(double r, double mass, double vr);

double Cd_from_Mach(double M);

Vec drag(double rho, double altitude,
               double upper_area, Vec const& velocity, double a);


                      
};  // namespace rocket
#endif