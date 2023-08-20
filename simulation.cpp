#include "simulation.h"

#include <cmath>

namespace sim {  // sono stato costretto a fare così perchè entravano in
                        // conflitto le definizioni

inline void Air_var::set_state(double altitude, double altitude_i) {
  double h = sim::cost::earth_radius_ * altitude /
             (sim::cost::earth_radius_ + altitude);
  double z = sim::cost::earth_radius_ * h / (sim::cost::earth_radius_ - h);
  double g = (sim::cost::G_ * sim::cost::earth_mass_) /
             std::pow(sim::cost::earth_radius_ + altitude, 2);

  if (h <= 11'000) {
    t_ = 288.15 - (6.5 * h);  // valori sperimentali
    p_ = sim::cost::sea_pression_ *
         std::pow((1 - sim::cost::air_gradient_ * altitude / t_),
                  g * sim::cost::molar_mass_ / sim::cost::R_ *
                      sim::cost::air_gradient_);
    rho_ = p_ / t_ * sim::cost::R_;
    // uso il gradiente base
  } else {
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
    if (47'000 < h &&
        h <= 51'000) {  // dopo non ha senso parlare di temperatura perchè la
      // densità è così bassa che si può trascurare l'effetto
      // di attrito dell'aria
      t_ = 270.65;
    }
    double delta_t = ti - t_;
    double delta_z = altitude_i - altitude;
    air_gradient_ = delta_t / delta_z;
    p_ = sim::cost::sea_pression_ *
         std::pow((1 - (air_gradient_ * altitude) / t_),
                  g * sim::cost::molar_mass_ / sim::cost::R_ * air_gradient_);
    rho_ = p_ / t_ * sim::cost::R_;
  }
}
  double const Air_var::sound_speed() const
  {
    return std::sqrt(sim::cost::gamma_*sim::cost::R_* t_);
  }
   double const Air_var::eq_mach(double speed) const
   {
    double a = sound_speed();
    return (speed/a);
   }
};  // namespace simulation
