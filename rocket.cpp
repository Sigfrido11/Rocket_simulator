#include "rocket.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "simulation.h"

namespace rocket {
class Rocket;
using Vec = std::array<double, 2>;


// funzioni Rocket


inline Vec const Rocket::drag(double rho) const {
  if (pos_[0] < 51'000) {
    double z =
        0.5 * rho * upper_area_ * std::cos(theta_) * std::pow(velocity_[0], 2);
    double y = 0.5 * rho * lateral_area_ * std::sin(theta_) *
               std::pow(velocity_[1], 2);
    return {z, y};
  } else {
    return {0., 0.};
  }
}

inline void set_state(std::string file_name) {
  Rocket::improve_theta(file_name);
  Rocket::mass_lost();
  Rocket::stage_release();
}
inline double Rocket::get_theta() const { return theta_; }

inline double Rocket::get_mass() const { return total_mass_; }

inline void Rocket::mass_lost(double liq_lost, double solid_lost){
  assert(liq_lost > 0 && solid_lost >0);
  total_mass_ -= liq_lost - solid_lost;
  mass_liq_prop_[0] -= liq_lost;
  mass_solid_prop_[0] -= solid_lost;
}

inline void Rocket::improve_theta(std::string name_f){
  std::ifstream file(name_f);
  assert(file.is_open());
  std::string line;
  int n_tab{0};
  double old_altitude{0.};
  double altitude;
  double angle;
  double old_ang{91.};
  std::streampos start_pos = file.tellg();
  file.seekg(start_pos);
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    iss >> altitude >> angle;
    if (altitude >= pos_[0]) {
      double delta1 = altitude - pos_[0];
      double delta2 = old_altitude - pos_[0];
      if (std::abs(delta1) < std::abs(delta2)) {
        theta_ = angle * orbital_h_ / 170'000;
        start_pos = file.tellg();
      } else {
        theta_ = old_ang * orbital_h_ / 170'000;
      }
    }
    old_altitude = altitude;
    old_ang = angle;
    start_pos = file.tellg();
  }
}

inline Vec Rocket::get_velocity() const { return velocity_; }

inline Vec Rocket::get_pos() const { return pos_; }

inline double Rocket::get_altitude() const { return pos_[0]; }

inline double Rocket::get_delta_altitude() const {return delta_altitude_;}

inline Vec const Rocket::total_force(double rho, double p_ext) const {
  Vec centrip = rocket::centripetal(total_mass_, pos_[0], velocity_[1]);
  Vec gra = rocket::g_force(pos_[0], total_mass_, theta_);
  Vec drag = Rocket::drag(rho);
  Vec eng = Rocket::Engine::eng_force(p_ext);
  double z = centrip[0]+ gra[0]+drag[0]+eng[0];
  double y = centrip[1]+ gra[1]+drag[1]+eng[1];
  return {z,y};
}

inline void Rocket::move(double time, Vec force) {
  double z = pos_[0] + velocity_[0] * time +
             0.5 * (force[0] / total_mass_) * std::pow(time, 2);
  delta_altitude_ = z - pos_[0];
  pos_[0] = z;
  pos_[1] = pos_[1] + velocity_[1] * time +
            0.5 * (force[1] / total_mass_) * std::pow(time, 2);
  assert(pos_[0] >= 0 && pos_[1] >= 0);  // non ha un sistema di riferimento
                                         // cartesiano quindi no orbita completa
}

inline void Rocket::change_vel(Vec force, double time) {
  velocity_[0] = velocity_[0] + (force[0] * (1 / total_mass_) * time);
  velocity_[1] = velocity_[1] + (force[1] * (1 / total_mass_) * time);
}

inline double const opt_aceleration(double altitude) {}

inline void Rocket::stage_release() {
  if (mass_solid_cont_ == 0) {
    assert(current_stage_ != 0);
    if (mass_liq_prop_[0] <= 20.) {
      current_stage_ -= 1;
      total_mass_ -= mass_liq_cont_[0] - mass_liq_prop_[0];
      mass_liq_cont_.erase(mass_liq_cont_.begin());
      mass_liq_prop_.erase(mass_liq_prop_.begin());
    }
  } else {
    assert(current_stage_ != 0);
    assert(mass_liq_prop_[0] != 0);
    assert(mass_liq_prop_.size() == total_stage_);
    if (mass_solid_prop_ <= 20.) {
      current_stage_ -= 1;
      total_mass_ -= mass_solid_cont_ - mass_solid_prop_;
      mass_solid_cont_ = 0;
      mass_solid_prop_ = 0;
    }
  }
}



// funzioni di engine



/*
  inline Vec const Rocket::Engine::v_exit(double pe_ext) const {

    double exponent = (s::costant::gamma_ - 1) / s::costant::gamma_;
    double p_ratio = std::pow(pe_ext / pres_0_, exponent);
    double v_e = std::sqrt(2 * temp_0_ * (s::costant::R_ / molecular_weight_)
  * (s::costant::gamma_ / s::costant::gamma_ - 1) * (1 - p_ratio)); return
  {v_e * std::cos(theta_), v_e * std::sin(theta_)};
  }
*/
inline void Rocket::Engine::int_pression(double time) {
  double fac1 = a_coef_ * grain_rho_ * grain_dim_ / nozzle_as_;
  double exponent = sim::cost::gamma_ + 1 / (sim::cost::gamma_ - 1);
  double fac2 = std::pow(2 / sim::cost::gamma_ + 1, exponent);
  double fac3 = std::sqrt((sim::cost::gamma_ * fac2) / sim::cost::R_ * t_0_);
  double new_pres = std::pow(fac1 / fac3, 1 / (1 - n_coef_));
  delta_pres_ = new_pres - p_0_;
  p_0_ = new_pres;
    grain_rho_= delta_pres_/(sim::cost::R_*t_0_);
    v_0_ += burn_a_*r_coef_*time;
}

inline void Rocket::Engine::r_coef() {
  r_coef_ = a_coef_ * std::pow(p_0_, n_coef_);
}

  inline double const rocket::Rocket::Engine::delta_m(double time, double p_ext) const {
  double fac1 = p_0_ * nozzle_as_;
  double fac2 = std::sqrt(sim::cost::gamma_ / (sim::cost::R_ * t_0_));
  double fac3 = 2 / (sim::cost::gamma_ + 1);
  double fac4 = (sim::cost::gamma_ + 1) / 2 * (sim::cost::gamma_ - 1);
  return fac1 * fac2 * std::pow(fac3, fac4);
}

inline Vec const Rocket::Engine::eng_force(double p_ext) const {
  double fac1 = nozzle_as_ * p_0_;
  double fac2 = (2*std::pow(sim::cost::gamma_,2)/(sim::cost::gamma_-1));
  double fac3 = 2/(sim::cost::gamma_-1);
  double exp = (sim::cost::gamma_+1)/(sim::cost::gamma_-1);
  double fac4= 1-std::pow((p_ext/p_0_), (sim::cost::gamma_-1/sim::cost::gamma_));
  double force = fac1 * std::sqrt(fac2*std::pow(fac3,exp)*fac4);
  double theta = Rocket::get_theta();
  return {force * std::cos(theta), force * std::sin(theta)};
}

inline void Rocket::Engine::set_state(double time, double p_ext) {
  Rocket::Engine::r_coef();
  Rocket::Engine::int_pression(time);
  Rocket::mass_lost(0., Rocket::Engine::delta_m(time, p_ext));
}





// funzioni del namespace





inline Vec const centripetal(double total_mass, double altitude,
                                     double x_vel) {
  double f_z = total_mass *
               std::pow((x_vel + sim::cost::earth_speed_), 2) /
               (sim::cost::earth_radius_ + altitude);
  return {f_z, 0};
}

inline Vec const g_force(double altitude, double mass, double theta){
  double force = sim::cost::earth_mass_*sim::cost::G_*mass/std::pow(sim::cost::earth_radius_+altitude,2);
  return {force*std::cos(theta), force*std::sin(theta)};
}

};  // namespace rocket
