#include "simulation.h"

#include <cmath>
#include <assert.h>

namespace sim
{ // sono stato costretto a fare così perchè entravano in
  // conflitto le definizioni

  void Air_var::set_state(double altitude)
  {
    assert(altitude >= 0);
    double const h {sim::cost::earth_radius_ * altitude /(1'000*(sim::cost::earth_radius_ + altitude))};
     if (h <= 11)
    {
      t_ = 288.15 - (6.5 * h); // valori sperimentali
      p_ = sim::cost::sea_pression_ * std::pow(288.15 / t_, -0.255877);
      rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
   }
    if (11 < h && h <= 20)
    {
      t_ = 216.65;
      p_ = 22'632 * std::exp(-0.1577 * (h - 11));
      rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
      rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
    }
    if (20 < h && h <= 32)
    {
      t_ = 216.65 + h - 20;
      p_ = 5'474.9 * std::pow(216.65 / t_, 34.16319);
      rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
    }
    if (32 < h && h <= 47)
    {
      t_ = 228.65 + 2.8 * (h - 32);
      p_ = 868 * std::pow((228.65 / t_), 12.2011);
      rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
    }
    if (47 < h && h <= 51)
    { 
      t_ = 270.65;
      p_ = 110.9 * exp(-0.1262 * (h - 47));
      rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
    }
    if(h > 51'000) {
      t_ = 0.;
      p_ = 0.;
      rho_ =0.;
    }

    rho_ = p_ * sim::cost::molar_mass_ / (t_ * sim::cost::R_);
  }
} // namespace sim
