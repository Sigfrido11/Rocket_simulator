#include "rocket.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "simulation.h"

namespace rocket {
class Rocket;
using Vec = std::array<double, 2>;

// costruttori di Rocket

Rocket::Rocket(std::string name, double mass_structure, double Up_Ar,
               double s_p_m, double m_s_cont, std::vector<double> l_p_m,
               std::vector<double> l_c_m, std::unique_ptr<Engine>& eng,
               int n_solid_eng, std::vector<int> n_liq_eng)
    : name_{name},
      upper_area_{Up_Ar},
      m_sol_cont_{m_s_cont},
      m_sol_prop_{s_p_m},
      m_liq_prop_{l_p_m},
      m_liq_cont_{l_c_m},
      total_mass_{mass_structure +
                  std::accumulate(l_p_m.begin(), l_p_m.end(), 0.) + s_p_m +
                  m_s_cont + std::accumulate(l_c_m.begin(), l_c_m.end(), 0.)},

      total_stage_{static_cast<int>(l_p_m.size()) + 1},
      n_sol_eng_{n_solid_eng},
      n_liq_eng_{n_liq_eng} {
  assert(m_liq_cont_.size() == m_liq_prop_.size() &&
         n_liq_eng_.size() == m_liq_prop_.size());
  assert(upper_area_ > 0 && m_sol_cont_ > 0 && m_sol_prop_ > 0);
  current_stage_ = total_stage_;
  std::for_each(n_liq_eng.begin(), n_liq_eng.end(),
                [](int value) { assert(value >= 0); });
  std::for_each(m_liq_cont_.begin(), m_liq_cont_.end(),
                [](double value) mutable { assert(value > 0.); });
  std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                [](double value) mutable { assert(value > 0.); });
  eng_ = std::move(eng);
}

// funzioni Rocket

double Rocket::get_theta() const { return theta_; }

double Rocket::get_mass() const { return total_mass_; }

void Rocket::mass_lost(double solid_lost, double liq_lost) {
  assert(liq_lost >= 0 && solid_lost >= 0);
  total_mass_ -= (liq_lost + solid_lost);
  m_liq_prop_[0] -= liq_lost;
  m_sol_prop_ -= solid_lost;
  if (total_mass_ <= 0) {
    std::cout
        << "wrong input fuel distribution between liquid and solid stages";
    throw std::runtime_error(
        "wrong input fuel distribution between liquid and solid stages");
  }
}

Vec const Rocket::get_velocity() const { return velocity_; }

Vec const Rocket::get_pos() const { return pos_; }

void Rocket::move(double time, Vec force) {
  pos_[0] = pos_[0] + velocity_[0] * time +
            0.5 * (force[0] / total_mass_) * std::pow(time, 2);
  pos_[1] = pos_[1] + velocity_[1] * time +
            0.5 * (force[1] / total_mass_) * std::pow(time, 2);
  if (pos_[0] >= 0 && pos_[1] >= 0) {
    throw std::runtime_error("not enough thrust");
  }
}

void Rocket::change_vel(double time, Vec force) {
  velocity_[0] = velocity_[0] + (force[0] / total_mass_) * time;
  velocity_[1] = velocity_[1] + (force[1] / total_mass_) * time;
}

void Rocket::set_state(std::string file_name, double orbital_h, double time,
                       bool is_orbiting, std::streampos& file_pos) {
  // if ((velocity_[0] < 0 || velocity_[1] < 0) && (is_orbiting == false)) {
  //   std::cout << "sorry guy, no orbit avaible for your rocket";
  //   assert(false);
  // }
  double const old_theta{theta_};
  theta_ = improve_theta(file_name, theta_, pos_[0], orbital_h, file_pos);
  if ((pos_[0] > 20'000) && old_theta != 0) {
    double const speed =
        std::sqrt(std::pow(velocity_[0], 2) + std::pow(velocity_[0], 2));
    velocity_[0] = speed * std::sin(theta_);
    velocity_[1] = speed * std::cos(theta_);
  }
  double const ms{eng_->delta_m(time, is_orbiting) * n_sol_eng_};
  double const ml{eng_->delta_m(time, is_orbiting) * n_liq_eng_[0]};
  mass_lost(ms, ml);
  stage_release(ms, ml);
}

int Rocket::get_rem_stage() const { return current_stage_; };

double Rocket::get_fuel_left() const { return (m_liq_prop_[0] + m_sol_prop_); }

void Rocket::stage_release(double delta_ms, double delta_ml) {
  if (m_liq_prop_[0] < 0) {
    std::cout << "error in the input distribution of the propellant" << '\n';
    throw std::runtime_error("error in the input distribution of the propellant");
  }
  if (m_sol_cont_ == 0) {
    int const len{static_cast<int>(m_liq_prop_.size())};
    assert(current_stage_ < len + 1 && current_stage_ >= 0);
    if (m_liq_prop_[0] <= delta_ml) {
      std::cout << "stage released"
                << "\n";
      current_stage_ -= 1;
      total_mass_ -= m_liq_cont_[0] - m_liq_prop_[0];
      m_liq_cont_.erase(m_liq_cont_.begin());
      m_liq_prop_.erase(m_liq_prop_.begin());
      n_liq_eng_.erase(n_liq_eng_.begin());
      if (current_stage_ == 0) {
        eng_->release();
      }
    }
  } else {
    assert(current_stage_ != 0);
    assert(m_liq_prop_[0] != 0);
    int const len{static_cast<int>(m_liq_prop_.size())};
    assert(len + 1 == total_stage_);
    if (m_sol_prop_ <= delta_ms) {
      current_stage_ -= 1;
      total_mass_ -= m_sol_cont_ - m_sol_prop_;
      m_sol_cont_ = 0;
      m_sol_prop_ = 0;
      n_sol_eng_ = 0;
      std::cout << "stage released"
                << "\n";
    }
  }
}

double Rocket::get_up_ar() const { return upper_area_; }

Vec const Rocket::thrust(double time, bool is_orbiting) const {
  Vec engs;
  Vec engl;
  if (eng_->is_ad_eng()) {
    std::vector<double> par{theta_};
    engs = eng_->eng_force(par, is_orbiting);
  } else {
    std::vector<double> par{time, theta_, pos_[0]};
    engs = eng_->eng_force(par, is_orbiting);
  }
  if (eng_->is_ad_eng()) {
    std::vector<double> par{theta_};
    engl = eng_->eng_force(par, is_orbiting);
  } else {
    std::vector<double> par{time, theta_, pos_[0]};
    engl = eng_->eng_force(par, is_orbiting);
  }
  double const z{engs[0] * n_sol_eng_ + engl[0] * n_liq_eng_[0]};
  double const y{engs[1] * n_sol_eng_ + engl[1] * n_liq_eng_[0]};
  return {z, y};
}

// funzioni di engine

// costruttori di Base_engine

Rocket::Base_engine::Base_engine(double isp, double cm, double p0,
                                 double burn_a)
    : isp_{isp}, cm_{cm}, p_0_{p0}, burn_a_{burn_a} {
  assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0 && burn_a_ >= 0);
}

// funzioni Base_engine

double Rocket::Base_engine::delta_m(double time, bool is_orbiting) const {
  if (!is_orbiting && !released_) {
    return p_0_ * burn_a_ * time * cm_;
  } else {
    return 0.;
  }
}

Vec const Rocket::Base_engine::eng_force(std::vector<double> par,
                                         bool is_orbiting) const {
  double const time{par[0]};
  double const theta{par[1]};
  double const pos{par[2]};
  double const delta_m = Rocket::Base_engine::delta_m(time, is_orbiting);
  if (!is_orbiting && !released_) {
    double const force{isp_ * delta_m * sim::cost::G_ * sim::cost::earth_mass_ /
                       std::pow((sim::cost::earth_radius_ + pos), 2)};
    return {force * std::sin(theta), force * std::cos(theta)};
  } else {
    return {0., 0.};
  }
}
bool Rocket::Base_engine::is_ad_eng() const { return false; }

void Rocket::Base_engine::release() { released_ = true; }

bool Rocket::Base_engine::is_released() const { return released_; }

double Rocket::Base_engine::get_pression() const { return p_0_; }
// Ad_engine

// costruttori Ad_Engine

Rocket::Ad_engine::Ad_engine(double burn_a, double nozzle_as, double t_0,
                             double grain_dim, double grain_rho, double a_coef,
                             double burn_rate_n, double prop_mm)
    : burn_a_{burn_a},
      nozzle_as_{nozzle_as},
      t_0_{t_0},
      grain_rho_{grain_rho},
      grain_dim_{grain_dim},
      burn_rate_a_{a_coef},
      burn_rate_n_{burn_rate_n},
      prop_mm_{prop_mm} {
  assert(p_0_ >= 0 && burn_a_ >= 0 && nozzle_as_ >= 0 && t_0_ >= 0 &&
         grain_dim_ >= 0 && grain_rho_ >= 0 && burn_rate_a_ >= 0 &&
         burn_rate_n_ >= 0 && prop_mm_ >= 0);

  double fac1 = burn_rate_a_ * grain_rho_ * grain_dim_ / nozzle_as_;
  double exponent = sim::cost::gamma_ + 1 / (sim::cost::gamma_ - 1);
  double fac2 = std::pow(2 / sim::cost::gamma_ + 1, exponent);
  double fac3 = std::sqrt((sim::cost::gamma_ * fac2) / sim::cost::R_ * t_0_);
  p_0_ = std::pow(fac1 * 2.2173e5 / fac3, 1 / (1 - burn_rate_n_));
}
Rocket::Ad_engine::Ad_engine(double p_0, double burn_a, double nozzle_as,
                             double t_0)
    : p_0_{p_0}, burn_a_{burn_a}, nozzle_as_{nozzle_as}, t_0_{t_0} {
  assert(p_0_ >= 0 && burn_a_ >= 0 && nozzle_as_ >= 0 && t_0_ >= 0);
}

bool Rocket::Ad_engine::is_ad_eng() const { return true; }

Vec const Rocket::Ad_engine::eng_force(std::vector<double> par,
                                       bool is_orbiting) const {
  double const theta{par[0]};
  if (!is_orbiting && !released_) {
    double const fac1{nozzle_as_ * p_0_};
    double const fac2{
        (2 * std::pow(sim::cost::gamma_, 2) / (sim::cost::gamma_ - 1))};
    double const fac3{2 / (sim::cost::gamma_ - 1)};
    double const exp{(sim::cost::gamma_ + 1) / (sim::cost::gamma_ - 1)};
    double const fac4{1 -
                      std::pow((sim::cost::sea_pression_ / p_0_),
                               (sim::cost::gamma_ - 1 / sim::cost::gamma_))};
    double const force{
        fac1 * std::sqrt(fac2 * std::pow(fac3, exp) * fac4 * 2.2173e-5)};
    return {force * std::sin(theta), force * std::cos(theta)};
  } else {
    return {0., 0.};
  }
}

double Rocket::Ad_engine::delta_m(double time, bool is_orbiting) const {
  if (!is_orbiting && !released_) {
    double const fac1{p_0_ * nozzle_as_};
    double const fac2{
        std::sqrt(sim::cost::gamma_ * prop_mm_ / (sim::cost::R_ * t_0_))};
    double const fac3{2 / (sim::cost::gamma_ + 1)};
    double const fac4{(sim::cost::gamma_ + 1) / (2 * (sim::cost::gamma_ - 1))};
    double const mass{fac1 * fac2 * std::pow(fac3, fac4) * time * 2.2173e-1};
    assert(mass >= 0.);
    return mass;
  } else {
    return 0.;
  }
}

double Rocket::Ad_engine::get_pression() const { return p_0_; }

void Rocket::Ad_engine::release() { released_ = true; }

bool Rocket::Ad_engine::is_released() const { return released_; }

// funzioni del namespace

bool is_orbiting(double pos, double velocity) {
  assert(pos >= 0 && velocity >= 0);
  double const inf_speed{std::sqrt(sim::cost::G_ * sim::cost::earth_mass_ /
                                   (pos + sim::cost::earth_radius_))};
  return ((velocity + sim::cost::earth_speed_) > inf_speed) ? true : false;
}

inline double centripetal(double total_mass, double altitude, double y_vel) {
  double const f_z{total_mass * std::pow(y_vel, 2) /
                   (sim::cost::earth_radius_ + altitude)};
  return f_z;
}

inline double g_force(double altitude, double mass) {
  double const force{sim::cost::earth_mass_ * sim::cost::G_ * mass /
                     (std::pow(sim::cost::earth_radius_ + altitude, 2))};
  return force;
}
inline Vec const drag(double rho, double altitude, double theta,
                      double upper_area, Vec velocity) {
  if (altitude <= 51'000) {
    double const speed2{std::pow(velocity[0], 2) + std::pow(velocity[1], 2)};
    double drag{0.37 * rho * upper_area * speed2};
    int dir{1};
    velocity[0] <= 0 ? dir = -1 : dir = 1;
    return {drag * std::sin(theta) * dir, drag * std::cos(theta)};
  } else {
    return {0., 0.};
  }
}

inline double improve_theta(std::string name_f, double theta, double pos,
                            double orbital_h, std::streampos& file_pos) {
  std::ifstream file(name_f);
  assert(file.is_open());
  std::string line;
  double old_altitude{0.};
  double altitude;
  double angle;
  pos = (pos * 170'000) / orbital_h;
  double old_ang;
  bool found{false};
  file.seekg(file_pos);
  while (std::getline(file, line) && !found) {
    std::istringstream iss(line);
    iss >> altitude >> angle;
    if (altitude >= pos) {
      found = true;
      double const delta1{altitude - pos};
      double const delta2{old_altitude - pos};
      if (std::abs(delta1) <= std::abs(delta2)) {
        return angle > theta ? theta : angle;
      } else {
        return old_ang > theta ? theta : old_ang;
      }
    }
    if (!found) {
      old_altitude = altitude;
      old_ang = angle;
      file_pos = file.tellg();
    }
  }
  return 0.;
}

Vec const total_force(double rho, double theta, double total_mass, double pos,
                      double upper_area, Vec velocity, Vec eng) {
  double const centrip{centripetal(total_mass, pos, velocity[1])};
  double const gra{g_force(pos, total_mass)};
  Vec const drag_f{drag(rho, pos, theta, upper_area, velocity)};
  double const z{eng[0] + centrip - gra - drag_f[0]};
  double const y{eng[1] - drag_f[1]};
  if (y <= 0) {
    return {z, 0};
  } else {
    return {z, y};
  }
}

};  // namespace rocket