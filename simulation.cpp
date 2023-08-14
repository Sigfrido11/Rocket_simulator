#include "simulation.h"

#include <cmath>

namespace simulation {  // sono stato costretto a fare così perchè entravano in conflitto le definizioni

void Air_var::Condition(double altitude, double altitude_i) {
  namespace s = simulation;
  double h = s::costant::earth_radius_ * altitude / (s::costant::earth_radius_ + altitude);
  double z = s::costant::earth_radius_ * h / (s::costant::earth_radius_ - h);
  double g =
      ( s::costant::G_ * s::costant::earth_mass_) / std::pow(s::costant::earth_radius_+ altitude, 2);

  if (h <= 11'000) {
    t_ = 288.15 - (6.5 * h);  // valori sperimentali
    p_ = s::costant::sea_pression_ *
         std::pow((1 - s::costant::air_gradient_ * altitude / t_),
                  g * s::costant::molar_mass_ / s::costant::R_ * s::costant::air_gradient_);
    rho_ = p_ / t_ * s::costant::R_;
    // uso il gradiente base
  }
  else {
    double ti = t_;
    if (11'000 < h && h <= 20'000) {
      t_ = 216.65;
    }
    if (20'000 < h && h <= 32'000) {
      t_ = 216.65 + h - 20'000;
    }
    if (32'000 < h && h <= 47'000) {
      t_ = 228.65 + 2.8 * (h - 32'000);
    }
    if (47'000 < h && h <= 51'000) {  // dopo non ha senso parlare di temperatura perchè la
                        // densità è così bassa che si può trascurare l'effetto
                        // di attrito dell'aria
      t_ = 270.65;
    }
    double delta_t = ti-t_;
    double delta_z = altitude_i- altitude;
    air_gradient_= delta_t/delta_z;
    p_ = s::costant::sea_pression_ *
         std::pow((1 - (air_gradient_ * altitude) / t_),
                  g * s::costant::molar_mass_ / s::costant::R_ * air_gradient_);
    rho_ = p_ / t_ * s::costant::R_;
  }
}
};  // namespace simulation
