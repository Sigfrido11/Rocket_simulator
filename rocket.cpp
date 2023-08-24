#include "rocket.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include "simulation.h"

namespace rocket {
class Rocket;
using Vec = std::array<double, 2>;

// funzioni Rocket

void Rocket::set_state(std::string file_name, double orbital_h, double time,
                       bool is_orbiting) {
  improve_theta(file_name, orbital_h);
  double ms{eng_s_->delta_m(time, is_orbiting)} * n_sol_eng_;
  double ml{liq_eng_[0].delta_m(time, is_orbiting)} * n_liq_eng_[0];
  mass_lost(ms, ml);
  stage_release(ms, ml);
}

double Rocket::get_theta() const { return theta_; }

double Rocket::get_mass() const { return total_mass_; }

void Rocket::mass_lost(double solid_lost, double liq_lost) {
  assert(liq_lost >= 0 && solid_lost >= 0);
  total_mass_ -= liq_lost - solid_lost;
  mass_liq_prop_[0] -= liq_lost;
  mass_solid_prop_ -= solid_lost;
}

void Rocket::improve_theta(std::string name_f, double orbital_h) {
  std::ifstream file(name_f);
  assert(file.is_open());
  if (pos_[0] != 0) {
    std::string line;
    double old_altitude{0.};
    double altitude;
    double angle;
    double old_ang{91.};
    bool found{false};
    file.seekg(start_pos_);
    while (std::getline(file, line) && !found) {
      std::istringstream iss(line);
      iss >> altitude >> angle;
      if (altitude >= pos_[0]) {
        found = true;
        double delta1 = altitude - pos_[0];
        double delta2 = old_altitude - pos_[0];
        if (std::abs(delta1) < std::abs(delta2)) {
          theta_ = angle * (orbital_h + 100'000) / 170'000;
        } else {
          theta_ = old_ang * (orbital_h + 120'000) / 170'000;
        }
      }
      if (!found) {
        old_altitude = altitude;
        old_ang = angle;
        start_pos_ = file.tellg();
      }
    }
  }
}

Vec Rocket::get_velocity() const { return velocity_; }

Vec Rocket::get_pos() const { return pos_; }

// double Rocket::get_delta_altitude() const { return delta_altitude_; }

Vec const Rocket::total_force(double rho, double p_ext, double time,
                              bool is_orbiting) const {
  Vec centrip = centripetal(total_mass_, pos_[0], velocity_[1]);
  Vec gra = g_force(pos_[0], total_mass_, theta_);
  Vec engs;
  Vec engl;
  if (eng_s_->is_ad_eng()) {
    std::vector<double> par{p_ext, theta_};
    Vec engs = eng_s_->eng_force(par, is_orbiting);
  } else {
    std::vector<double> par{time, theta_, pos_[0]};
    Vec engs = eng_s_->eng_force(par, is_orbiting);
  }
  if (liq_eng_[0].is_ad_eng()) {
    std::vector<double> par{p_ext, theta_};
    Vec engl = liq_eng_[0].eng_force(par, is_orbiting)* n;
  } else {
    std::vector<double> par{time, theta_, pos_[0]};
    Vec engl = liq_eng_[0].eng_force(par, is_orbiting);
  }
  Vec drag_f = drag(rho, pos_[0], theta_, upper_area_, lateral_area_, velocity_);
  double z = engs[0] *n_sol_eng + engl[0] * n_liq_eng[0] - gra[0] - drag_f[0];
  double y = engs[1] * n_sol_eng + engl[1] * n_liq_eng[0] + centrip[1] - gra[1] - drag_f[1];
  return {z, y};
}

void Rocket::move(double time, Vec force) {
  pos_[0] = pos_[0] + velocity_[0] * time +
            0.5 * (force[0] / total_mass_) * std::pow(time, 2);
  pos_[1] = pos_[1] + velocity_[1] * time +
            0.5 * (force[1] / total_mass_) * std::pow(time, 2);
  // assert(pos_[0] >= 0 && pos_[1] >= 0);  // non ha un sistema di riferimento
  //  cartesiano quindi no orbita completa
}

void Rocket::change_vel(Vec force, double time) {
  velocity_[0] = velocity_[0] + (force[0] * (1 / total_mass_) * time);
  velocity_[1] = velocity_[1] + (force[1] * (1 / total_mass_) * time);
}

// inline double const opt_aceleration(double altitude) {}

void Rocket::stage_release(double delta_ms, double delta_ml) {
  assert(mass_liq_prop_[0] >= 0 && mass_solid_prop_ >= 0);
  if (mass_solid_cont_ == 0) {
    assert(current_stage_ != 0);
    if (mass_liq_prop_[0] <= delta_ml) {
      std::cout << "distacco avvenuto"
                << "\n";
      current_stage_ -= 1;
      total_mass_ -= mass_liq_cont_[0] - mass_liq_prop_[0];
      mass_liq_cont_.erase(mass_liq_cont_.begin());
      mass_liq_prop_.erase(mass_liq_prop_.begin());
      liq_eng_.erase(liq_eng_.begin());
      n_liq_eng.erase(n_liq_eng.begin());
    }
  } else {
    assert(current_stage_ != 0);
    assert(mass_liq_prop_[0] != 0);
    int len = static_cast<int>(mass_liq_prop_.size());
    assert(len == total_stage_);
    if (mass_solid_prop_ <= delta_ms) {
      current_stage_ -= 1;
      total_mass_ -= mass_solid_cont_ - mass_solid_prop_;
      mass_solid_cont_ = 0;
      mass_solid_prop_ = 0;
      eng_s_->release();
      n_sol_eng =0;
    }
  }
}

double const Rocket::Base_engine::delta_m(double time, bool is_orbiting) const {
  if (!is_orbiting && !released_) {
    return p_0_ * burn_a_ * time * cm_;
  } else {
    return 0.;
  }
}

Vec const Rocket::Base_engine::eng_force(std::vector<double> par,
                                         double is_orbiting) const {
  double time{par[0]};
  double theta{par[1]};
  double pos{par[2]};
  bool delta_m = Rocket::Base_engine::delta_m(time, is_orbiting);
  if (!is_orbiting && !released_) {
    double force = isp_ * delta_m * sim::cost::G_ * sim::cost::earth_mass_ /
                   std::pow((sim::cost::earth_radius_ + pos), 2);
    return {force * std::sin(theta), force * std::cos(theta)};
  } else {
    return {0., 0.};
  }
}
bool const Rocket::Base_engine::is_ad_eng() const { return false; }

void Rocket::Base_engine::release() { released_ = true; }

bool const Rocket::Base_engine::is_released() const { return released_; }

bool const Rocket::Ad_engine::is_ad_eng() const { return true; }

Vec const Rocket::Ad_engine::eng_force(std::vector<double> par,
                                       bool is_orbiting) const {
  double p_ext{par[0]};
  double theta{par[1]};
  if (!is_orbiting && !released_) {
    double fac1 = nozzle_as_ * p_0_;
    double fac2 =
        (2 * std::pow(sim::cost::gamma_, 2) / (sim::cost::gamma_ - 1));
    double fac3 = 2 / (sim::cost::gamma_ - 1);
    double exp = (sim::cost::gamma_ + 1) / (sim::cost::gamma_ - 1);
    double fac4 = 1 - std::pow((p_ext / p_0_),
                               (sim::cost::gamma_ - 1 / sim::cost::gamma_));
    double force = fac1 * std::sqrt(fac2 * std::pow(fac3, exp) * fac4);
    return {force * std::sin(theta), force * std::cos(theta)};
  } else {
    return {0., 0.};
  }
}

double const Rocket::Ad_engine::delta_m(double time, bool is_orbiting) const {
  if (!is_orbiting && !released_) {
    double fac1 = p_0_ * nozzle_as_;
    double fac2 = std::sqrt(sim::cost::gamma_ / (sim::cost::R_ * t_0_));
    double fac3 = 2 / (sim::cost::gamma_ + 1);
    double fac4 = (sim::cost::gamma_ + 1) / 2 * (sim::cost::gamma_ - 1);
    double mass = fac1 * fac2 * std::pow(fac3, fac4) * time;
    assert(mass >= 0.);
    return mass;
  } else {
    return 0.;
  }
}

void Rocket::Ad_engine::release() { released_ = true; }

bool const Rocket::Ad_engine::is_released() const { return released_; }

// funzioni di engine
/*
inline double const Rocket::Engine::v_exit(double pe_ext) const {
  double exponent = (sim::cost::gamma_ - 1) / sim::cost::gamma_;
  double p_ratio = std::pow(pe_ext / p_0_, exponent);
  double v_e =
      std::sqrt(2 * t_0_ * (sim::cost::R_ / molecular_weight_) *
                (sim::cost::gamma_ / sim::cost::gamma_ - 1) * (1 - p_ratio));
  return {v_e};
}

void Rocket::Engine::int_pression(double p_ext, double mac) {
      double fac1 = a_coef_ * grain_rho_ * grain_dim_ / nozzle_as_;
      double exponent = sim::cost::gamma_ + 1 / (sim::cost::gamma_ - 1);
      double fac2 = std::pow(2 / sim::cost::gamma_ + 1, exponent);
      double fac3 =
          std::sqrt((sim::cost::gamma_ * fac2) / sim::cost::R_ * t_0_);
      double new_pres = std::pow(fac1 / fac3, 1 / (1 - n_coef_));
      delta_pres_ = (new_pres - p_0_);
      p_0_ = new_pres;
      grain_rho_ = delta_pres_ / (sim::cost::R_ * t_0_);
    }
  }

}

void Rocket::Engine::r_coef() { r_coef_ = a_coef_ * std::pow(p_0_, n_coef_); }

double rocket::Rocket::Engine::delta_m(double time) const {
  double fac1 = p_0_ * nozzle_as_;
  double fac2 = std::sqrt(sim::cost::gamma_ / (sim::cost::R_ * t_0_));
  double fac3 = 2 / (sim::cost::gamma_ + 1);
  double fac4 = (sim::cost::gamma_ + 1) / 2 * (sim::cost::gamma_ - 1);
  double mass = fac1 * fac2 * std::pow(fac3, fac4) * time;
  assert(mass >= 0.);
  return mass;
}


*/

// funzioni del namespace

inline bool const is_orbiting(double pos, double velocity) {
  double inf_speed = std::sqrt(sim::cost::G_ * sim::cost::earth_mass_ /
                               (pos + sim::cost::earth_radius_));
  return ((velocity + sim::cost::earth_speed_) > inf_speed) ? true : false;
}

inline Vec const centripetal(double total_mass, double altitude, double x_vel) {
  double f_z =
      total_mass * std::pow((x_vel), 2) / (sim::cost::earth_radius_ + altitude);
  return {f_z, 0};
}

inline Vec const g_force(double altitude, double mass, double theta) {
  double force = sim::cost::earth_mass_ * sim::cost::G_ * mass /
                 (std::pow(sim::cost::earth_radius_ + altitude, 2));
  return {force * std::sin(theta), force * std::cos(theta)};
}
inline Vec const drag(double rho, double altitude, double theta,
                      double upper_area, double lateral_area, Vec velocity) {
  if (altitude <= 51'000) {
    double z =
        0.5 * rho * upper_area * std::cos(theta) * std::pow(velocity[0], 2);
    double y =
        0.5 * rho * lateral_area * std::sin(theta) * std::pow(velocity[1], 2);
    return {z, y};
  } else {
    return {0., 0.};
  }
}
};  // namespace rocket
