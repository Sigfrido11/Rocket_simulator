#ifndef SIMULATION_H
#define SIMULATION_H

#include <array>
#include <cmath>
#include <vector>

#include "rocket.h"

namespace simulation {
  using Vec = std::array<double,2>;

struct Air_var{
  double p_ {101325}; // pression
  double t_ {293.0}; //temperature
  double rho_{1.225}; //density
  double air_gradient_{0.};

  void Condition(double, double); 
};

struct costant {
  static constexpr double air_gradient_{0.0065};
  static constexpr double sea_pression_{101325};
  static constexpr double R_{8.314462618};
  static constexpr double molar_mass_{0.0289644};  // kilo su mole 
  static constexpr double G_{6.6743e-11};
  static constexpr double earth_radius_{6.378e6};
  static constexpr double earth_mass_{5.9726e24};
};


};  // namespace simulation
#endif  // SIMULATION_H
