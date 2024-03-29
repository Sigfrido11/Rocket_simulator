#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "simulation.h"
#include "rocket.h"

//
// asse x componente [1]
// asse y componente [0]
//

namespace rocket {
class Rocket;
using Vec = std::array<double, 2>;

// costruttori di Rocket

Rocket::Rocket(std::string const& name, double mass_structure, double Up_Ar,
               double s_p_m, double m_s_cont, std::vector<double> const& l_p_m,
               std::vector<double> const& l_c_m, Engine* eng, int n_solid_eng,
               std::vector<int> const& n_liq_eng)
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
                eng_=eng;
}

// funzioni Rocket

double Rocket::get_theta() const { return theta_; }

double Rocket::get_mass() const { return total_mass_; }

void Rocket::mass_lost(double solid_lost, double liq_lost) {
  assert(liq_lost >= 0 && solid_lost >= 0);
  //sottrazione carburante perso
  total_mass_ -= (liq_lost + solid_lost);
  m_liq_prop_[0] -= liq_lost;
  m_sol_prop_ -= solid_lost;
  if (total_mass_ <= 0) {
    std::cout
        << "wrong input fuel distribution between liquid and solid stages";
    throw std::runtime_error(
        "wrong input fuel distribution");
  }
}

Vec const Rocket::get_velocity() const { return velocity_; }

Vec const Rocket::get_pos() const { return pos_; }

void Rocket::move(double time, Vec const& force) {
  //equazioni moto uniformemente acelerato
  pos_[0] = pos_[0] + velocity_[0] * time +
            0.5 * (force[0] / total_mass_) * std::pow(time, 2);
  pos_[1] = pos_[1] + (velocity_[1]+sim::cost::earth_speed_) * time +
            0.5 * (force[1] / total_mass_) * std::pow(time, 2);
  
  if (pos_[0] <= 0 && pos_[1] <= 0) {
    throw std::runtime_error("not enough thrust");
  }
}

void Rocket::change_vel(double time, Vec const& force) {
  velocity_[0] = velocity_[0] + (force[0] / total_mass_) * time;
  velocity_[1] = velocity_[1] + (force[1] / total_mass_) * time;
}

void Rocket::set_state(std::string const& file_name, double orbital_h,
                       double time, bool is_orbiting,
                       std::streampos& file_pos) {
  // cambia l'angolo, ridistribuisce la velocità in modo che sia
  // tangente alla traiettoria, rimuove il carburante consumato 
  // precedentemente e valuta la rimozione degli stadi
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
  if(current_stage_!=0){
  stage_release(ms, ml);
  }
}

int Rocket::get_rem_stage() const { return current_stage_; };

double Rocket::get_fuel_left() const { 
  if(current_stage_==0){
    return 0;
  } else{
  return (m_liq_prop_[0] + m_sol_prop_); 
  }}

void Rocket::stage_release(double delta_ms, double delta_ml) {
  if (m_liq_prop_[0] < 0) {
    std::cout << "error in the input distribution of the propellant" << '\n';
    throw std::runtime_error(
        "error in the input distribution of the propellant");
  }
  if (m_sol_cont_ == 0) { //caso con stadio solido espulso
    int const len{static_cast<int>(m_liq_prop_.size())};
    assert(current_stage_ < len + 1 && current_stage_ >= 0);
    if (m_liq_prop_[0] <= delta_ml) { 
      //se prevedo di bruciare più carburante di quello che rimane
      std::cout << "stage released"
                << "\n";
      current_stage_ -= 1;
      total_mass_ -= (m_liq_cont_[0] + m_liq_prop_[0]);
      m_liq_cont_.erase(m_liq_cont_.begin());
      m_liq_prop_.erase(m_liq_prop_.begin());
      n_liq_eng_.erase(n_liq_eng_.begin());
      if (current_stage_ == 0) {
        eng_->release(); //se ho consumato tutto il carburante 
                        // all'ultimo stadio stacco i motori
      m_liq_prop_.resize(1);
      m_liq_prop_[0];
      }
    }
  } else {
    assert(current_stage_ != 0);
    assert(m_liq_prop_[0] != 0);
    int const len{static_cast<int>(m_liq_prop_.size())};
    assert(len + 1 == total_stage_);
    if (m_sol_prop_ <= delta_ms) {
      current_stage_ -= 1;
      total_mass_ -= (m_sol_cont_ + m_sol_prop_);
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
  eng_par par{theta_, time, pos_[0]};
  if (eng_->is_ad_eng()) {
    engs = eng_->eng_force(par, is_orbiting);
  } else {
    engs = eng_->eng_force(par, is_orbiting);
  }
  if (eng_->is_ad_eng()) {
    engl = eng_->eng_force(par, is_orbiting);
  } else {
    engl = eng_->eng_force(par, is_orbiting);
  }
  double const z{engs[0] * n_sol_eng_ + engl[0] * n_liq_eng_[0]};
  double const y{engs[1] * n_sol_eng_ + engl[1] * n_liq_eng_[0]};
  return {z, y};
}

// funzioni di engine

// costruttori di Base_engine

Base_engine::Base_engine(double isp, double cm, double p0, double burn_a)
    : isp_{isp}, cm_{cm}, p_0_{p0}, burn_a_{burn_a} {
  assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0 && burn_a_ >= 0);
}

// funzioni Base_engine

double Base_engine::delta_m(double time, bool is_orbiting) const {
  if (!is_orbiting && !released_) {
    return p_0_ * burn_a_ * time * cm_;
  } else {
    return 0.;
  }
}

Vec const Base_engine::eng_force(eng_par const& par, bool is_orbiting) const {
  double const delta_m = Base_engine::delta_m(par.time, is_orbiting);
  if (!is_orbiting && !released_) {
    double const force{isp_ * delta_m * sim::cost::G_ * sim::cost::earth_mass_ /
                       std::pow((sim::cost::earth_radius_ + par.pos), 2)}; 
                       //isp * delta_m * g
    return {force * std::sin(par.theta), force * std::cos(par.theta)};
  } else {
    return {0., 0.};
  }
}
bool Base_engine::is_ad_eng() const { return false; }

void Base_engine::release() { released_ = true; }

bool Base_engine::is_released() const { return released_; }

double Base_engine::get_pression() const { return p_0_; }

// Ad_engine

// costruttori Ad_Engine

Ad_engine::Ad_engine(double burn_a, double nozzle_as, double t_0,
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

  double const fac1 {burn_rate_a_ * grain_rho_ * grain_dim_ / nozzle_as_};
  double const exponent {sim::cost::gamma_ + 1 / (sim::cost::gamma_ - 1)};
  double const fac2 = {std::pow(2 / sim::cost::gamma_ + 1, exponent)};
  double const fac3 {std::sqrt((sim::cost::gamma_ * fac2) / sim::cost::R_ * t_0_)};
  //equazioni trovate online
  p_0_ = std::pow(fac1 * 2.2173e5 / fac3, 1 / (1 - burn_rate_n_));
}
Ad_engine::Ad_engine(double p_0, double burn_a, double nozzle_as, double t_0)
    : p_0_{p_0}, burn_a_{burn_a}, nozzle_as_{nozzle_as}, t_0_{t_0} {
  assert(p_0_ >= 0 && burn_a_ >= 0 && nozzle_as_ >= 0 && t_0_ >= 0);
}

bool Ad_engine::is_ad_eng() const { return true; }

Vec const Ad_engine::eng_force(eng_par const& par, bool is_orbiting) const {
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
    return {force * std::sin(par.theta), force * std::cos(par.theta)};
  } else {
    return {0., 0.};
  }
}

double Ad_engine::delta_m(double time, bool is_orbiting) const {
  if (!is_orbiting && !released_) {
    double const fac1{p_0_ * nozzle_as_};
    double const fac2{
        std::sqrt(sim::cost::gamma_ * prop_mm_ / (sim::cost::R_ * t_0_))};
    double const fac3{2 / (sim::cost::gamma_ + 1)};
    double const fac4{(sim::cost::gamma_ + 1) / (2 * (sim::cost::gamma_ - 1))};
    double const mass{fac1 * fac2 * std::pow(fac3, fac4) * time * 2.2173e-2};
    assert(mass >= 0.);
    return mass;
  } else {
    return 0.;
  }
}

double Ad_engine::get_pression() const { return p_0_; }

void Ad_engine::release() { released_ = true; }

bool Ad_engine::is_released() const { return released_; }

// funzioni del namespace

bool is_orbiting(double pos, double velocity) {
  assert(pos >= 0 && velocity >= 0);
  //dice se il razzo sta orbitando o meno
  double const inf_speed{std::sqrt(sim::cost::G_ * sim::cost::earth_mass_ /
                                   (pos + sim::cost::earth_radius_))};
                                   //energia cinetica + spinta rotazione terrrestre
  return ((velocity + sim::cost::earth_speed_) > inf_speed) ? true : false;
}

inline double centripetal(double total_mass, double altitude, double x_vel) {
  //f=m v^2/r
  double const f_z{total_mass * std::pow(x_vel+sim::cost::earth_speed_, 2) /
                   (sim::cost::earth_radius_ + altitude)};
  return f_z;
}

inline double g_force(double altitude, double mass) {
  //legge di newton
  double const force{sim::cost::earth_mass_ * sim::cost::G_ * mass /
                     (std::pow(sim::cost::earth_radius_ + altitude, 2))};
  return force;
}

inline Vec const drag(double rho, double altitude, double theta,
                      double upper_area, Vec const& velocity) {
  if (altitude <= 51'000) { //sopra atmosfera troppo rada
    double const speed2{std::pow(velocity[0], 2) + std::pow(velocity[1], 2)}; 
    //modulo quadro della velocità
    double const drag{0.37 * rho * upper_area * speed2};
    //presto attenzione alla direzione della velocità
    int direction{1};
    velocity[0] <= 0 ? direction = -1 : direction = 1;
    return {drag * std::sin(theta) * direction, drag * std::cos(theta)};
  } else {
    return {0., 0.};
  }
}

inline double improve_theta(std::string const& name_f, double theta, double pos,
                            double orbital_h, std::streampos& file_pos) {
  std::ifstream file(name_f);
  assert(file.is_open());
  std::string line;
  double old_altitude{0.};
  double altitude;
  double angle;
  pos = std::max(0., (pos * 170'000) / orbital_h - 5e5 / pos);
  //rallento un po' la velocità a cui cambia l'angolo
  double old_ang;
  bool found{false};
  file.seekg(file_pos);
  while (std::getline(file, line) && !found) {
    std::istringstream iss(line); // estrae i valori 
    iss >> altitude >> angle; //li attribuisce alle variabili
    if (altitude >= pos) { //altitude=letta da file
      found = true;
      double const delta1{std::abs(altitude - pos)};
      double const delta2{std::abs(old_altitude - pos)}; 
      //calcolo se ero più vicino prima al valore o adesso
      if (delta1 <= delta2) {
        return angle > theta ? theta : angle;
      } else {
        return old_ang > theta ? theta : old_ang;
      }
    }
    if (!found) {
      old_altitude = altitude; //old_atitude quella del ciclo prima
      old_ang = angle;
      file_pos = file.tellg(); //sposto il segnalibro
    }
  }
  return 0.;
}

Vec const total_force(double rho, double theta, double total_mass, double pos,
                      double upper_area, Vec const& velocity, Vec const& eng) {
  double const centrip{centripetal(total_mass, pos, velocity[1])};
  double const gra{g_force(pos, total_mass)};
  Vec const drag_f{drag(rho, pos, theta, upper_area, velocity)};
  double const y_force{eng[0] + centrip - gra - drag_f[0]};
  double const x_force{eng[1] - drag_f[1]};
  if (x_force <= 0) {
    return {y_force, 0};
  } else {
    return {y_force, x_force};
  }
}
};  // namespace rocket