#ifndef SIMULATION_H
#define SIMULATION_H

#include <array>
#include <cmath>
#include <vector>

namespace sim {
using Vec = std::array<double, 2>;

struct Air_var {
  double p_{101'325};  // pression
  double t_{293.0};   // temperature
  double rho_{1.225}; // density

  void set_state(double);

  double const sound_speed() const;
};

struct cost {
  static constexpr double sea_pression_{101325};
  static constexpr double R_{8.314462618};
  static constexpr double molar_mass_{0.0289644}; // kilo su mole
  static constexpr double G_{6.6743e-11};
  static constexpr double earth_radius_{6.378e6};
  static constexpr double earth_mass_{5.9726e24};
  static constexpr double earth_speed_{6.0048e3};
  static constexpr double gamma_{1.4};
};

};     // namespace sim
#endif // SIMULATION_H

