#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "simulation.h"
#include "rocket.h"
#include "vector_math.h"

//
// angular component [1]
// radial module component [0]
//

/* --------------------------------------------------------------------------*/
/*                           costructor                              
/* -------------------------------------------------------------------------- */

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
                [](int value) { if(value <=0){
                  throw std::runtime_error("invalid value insterted");
                }
                });
  std::for_each(m_liq_cont_.begin(), m_liq_cont_.end(),
                [](double value) { if(value <=0){
                  throw std::runtime_error("invalid value insterted");
                } });
  std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                [](double value) { if(value <=0){
                  throw std::runtime_error("invalid value insterted");
                } });
                eng_=eng;
}


/* -------------------------------------------------------------------------- */
/*                                Rocket getters methods                              */
/* -------------------------------------------------------------------------- */

double Rocket::get_theta() const { return theta_; }

double Rocket::get_mass() const { return total_mass_; }

Vec const Rocket::get_velocity() const { return velocity_; }

Vec const Rocket::get_pos() const { return pos_; }

int Rocket::get_rem_stage() const { return current_stage_; };

double Rocket::get_fuel_left() const { 
  if(current_stage_==0){
    return 0;
  } else{
  return (m_liq_prop_[0] + m_sol_prop_); 
  }}

double Rocket::get_up_ar() const { return upper_area_; }

double get_altitude() const {return pos_[0]*std::sin(pos_[1];)}

void Rocket::mass_lost(double solid_lost, double liq_lost)
{
    // Sanity checks 
    if (solid_lost < 0.0 || liq_lost < 0.0) {
        throw std::invalid_argument("Negative propellant mass loss");
    }

    // Check availability BEFORE subtracting
    if (solid_lost > m_sol_prop_) {
        throw std::invalid_argument("Solid propellant exhausted");
    }

    if (!m_liq_prop_.empty() && liq_lost > m_liq_prop_[0]) {
        throw std::runtime_error("Liquid propellant exhausted");
    }

    // Apply mass loss
    m_sol_prop_ -= solid_lost;
    if (!m_liq_prop_.empty()) {
        m_liq_prop_[0] -= liq_lost;
    }

    total_mass_ -= (solid_lost + liq_lost);

    if (total_mass_ <= 0.0) {
        throw std::runtime_error("Rocket mass became non-positive");
    }
}

/* -------------------------------------------------------------------------- */
/*                               kinematics rocket                              */
/* -------------------------------------------------------------------------- */





void Rocket::change_vel(double dt, Vec const& force) {

  // Current state in polar coordinates
  double r     = pos_[0];       // radial position
  double psi   = pos_[y];       // angular position (not directly used here)
  double vr    = velocity_[0];  // radial velocity
  double omega = velocity_[1];  // angular velocity (dψ/dt)

  if (r <= 1e-8) {
    throw std::runtime_error("Radius too small (singularity at r=0)");
  }

  // Total external force components (already includes gravity, drag, thrust, etc.)
  double Fr   = force[0];   // radial force component
  double Fpsi = force[1];   // tangential force component

  // ----- Polar coordinate accelerations -----
  // These extra terms appear because we are in a rotating coordinate system.

  // Radial acceleration:
  // ar = r*omega^2 + Fr/m
  double ar =
      r * omega * omega
      + Fr / total_mass_;

  // Angular acceleration:
  // domega/dt = (1/r)*(Fpsi/m - 2*vr*omega)
  double aomega =
      (1.0 / r) *
      (Fpsi / total_mass_ - 2.0 * vr * omega);

  // ----- Semi-implicit (symplectic) Euler velocity update -----
  // v(t+dt) = v(t) + a(t)*dt

  velocity_[0] += ar * dt;
  velocity_[1] += aomega * dt;
}


void Rocket::move(double dt, Vec const& force) {

  change_vel(dt, force);
  // Updated velocities
  double vr    = velocity_[0];
  double omega = velocity_[1];

  // ----- Position update -----
  // r(t+dt)   = r(t)   + vr(t+dt) * dt
  // psi(t+dt) = psi(t) + omega(t+dt) * dt

  pos_[0] += vr * dt;
  pos_[1] += omega * dt;

  // Prevent negative radius (impact or numerical instability)
  if (pos_.[0] <= 0.0) {
    throw std::runtime_error("Rocket reached r <= 0 (impact or instability)");
  }
}

/* -------------------------------------------------------------------------- */
/*                               stage menagement                              */
/* -------------------------------------------------------------------------- */


void Rocket::mass_lost(double solid_lost, double liq_lost)
{
    // Sanity checks 
    if (solid_lost < 0.0 || liq_lost < 0.0) {
        throw std::invalid_argument("Negative propellant mass loss");
    }

    // Check availability BEFORE subtracting
    if (solid_lost > m_sol_prop_) {
        throw std::invalid_argument("Solid propellant exhausted");
    }

    if (!m_liq_prop_.empty() && liq_lost > m_liq_prop_[0]) {
        throw std::runtime_error("Liquid propellant exhausted");
    }

    // Apply mass loss
    m_sol_prop_ -= solid_lost;
    if (!m_liq_prop_.empty()) {
        m_liq_prop_[0] -= liq_lost;
    }

    total_mass_ -= (solid_lost + liq_lost);

    if (total_mass_ <= 0.0) {
        throw std::runtime_error("Rocket mass became non-positive");
    }
}

void Rocket::set_state(std::string const& file_name,
                       double orbital_h,
                       double time,
                       bool is_orbiting,
                       std::streampos& file_pos)
{
    // ============================================================
    // 1. Update thrust angle (theta)
    // ============================================================

    // Store previous angle
    const double old_theta{theta_};

    // Improve flight angle based on guidance file
    theta_ = improve_theta(file_name, theta_, pos_[0],
                           orbital_h, file_pos);

    // ============================================================
    // 2. Re-orient velocity to be tangential to trajectory
    // ============================================================

    // Once above 20 km, align velocity with updated angle
    if (pos_[0] > 20000.0 && std::abs(old_theta) > 1e-8)
    {
        // Compute current speed magnitude
        const double speed = velocity[0];

        // Redistribute velocity components according to new angle
        velocity_[0] = speed * std::sin(theta_);
        velocity_[1] = speed * std::cos(theta_);
    }

    // ============================================================
    // 3. Compute propellant mass loss
    // ============================================================


    if (!eng_)
        throw std::runtime_error("Engine pointer is null");


    //compute mass loss
    const double delta_mass = eng_->delta_m(time, is_orbiting);

    solid_burn = delta_mass * n_sol_eng_;

    liquid_burn = delta_mass * n_liq_eng_.front();


    // Apply mass loss
    mass_lost(solid_lost, liq_lost);

    // ============================================================
    // 4. Check for stage separation
    // ============================================================

    if (current_stage_ != 0){
        stage_release(ms, ml);
    }

}



void Rocket::stage_release(double delta_ms, double delta_ml) {

  // ------------------------------------------------------------
  // Safety check:
  // If the first liquid propellant mass is negative,
  // it means something went wrong in mass bookkeeping.
  // ------------------------------------------------------------
  if (m_liq_prop_[0] < 0) {
    std::cout << "error in the input distribution of the propellant" << '\n';
    throw std::runtime_error(
        "error in the input distribution of the propellant");
  }

  // ------------------------------------------------------------
  // CASE 1:
  // Solid stage has already been released
  // (m_sol_cont_ == 0 means solid container mass is zero,
  // so the solid booster is no longer present)
  // ------------------------------------------------------------
  if (m_sol_cont_ == 0) {

    // Number of remaining liquid stages
    int const len{static_cast<int>(m_liq_prop_.size())};

    // Check that current_stage_ index is consistent
    if(!(current_stage_ < len + 1 && current_stage_ >= 0)){
      throw std::runtime_error("error in stage release");
    }

    // ------------------------------------------------------------
    // If the remaining liquid propellant is less than or equal
    // to the amount that is about to be burned (delta_ml),
    // then this stage is considered depleted.
    // ------------------------------------------------------------
    if (m_liq_prop_[0] <= delta_ml) {

      std::cout << "stage released" << "\n";

      // Decrease stage counter
      current_stage_ -= 1;

      // Remove empty tank mass and remaining propellant
      // from total rocket mass
      total_mass_ -= (m_liq_cont_[0] + m_liq_prop_[0]);

      // Physically remove first liquid stage from vectors
      m_liq_cont_.erase(m_liq_cont_.begin());
      m_liq_prop_.erase(m_liq_prop_.begin());
      n_liq_eng_.erase(n_liq_eng_.begin());

      // --------------------------------------------------------
      // If no more stages remain
      // --------------------------------------------------------
      if (current_stage_ == 0) {

        // Release engines (final shutdown)
        eng_->release();

        // Resize liquid prop vector to size 1
        // (This line does not modify content meaningfully,
        // but ensures vector is not empty)
        m_liq_prop_.resize(1);

        // Access first element (has no effect)
        m_liq_prop_[0];
      }
    }
  }

  // ------------------------------------------------------------
  // CASE 2:
  // Solid stage is still present
  // ------------------------------------------------------------
  else {

    // Ensure there is at least one stage active
    assert(current_stage_ != 0);

    // Ensure liquid propellant exists
    assert(m_liq_prop_[0] != 0);

    int const len{static_cast<int>(m_liq_prop_.size())};

    // Consistency check:
    // Number of liquid stages + 1 (solid stage)
    // must match total_stage_
    if(!(len + 1 == total_stage_)){
      throw std::runtime_error("error in stage menagement");
    }

    // ------------------------------------------------------------
    // If remaining solid propellant is less than or equal
    // to the amount that is about to be burned (delta_ms),
    // then solid stage is depleted.
    // ------------------------------------------------------------
    if (m_sol_prop_ <= delta_ms) {

      // Decrease stage counter
      current_stage_ -= 1;

      // Remove solid container + remaining solid propellant
      // from total mass
      total_mass_ -= (m_sol_cont_ + m_sol_prop_);

      // Reset solid stage values
      m_sol_cont_ = 0;
      m_sol_prop_ = 0;
      n_sol_eng_ = 0;

      std::cout << "stage released" << "\n";
    }
  }
}



double improve_theta(std::string const& name_f, double theta, double pos,
                            double orbital_h, std::streampos& file_pos) {
  // Open the input file whose name is provided
  std::ifstream file(name_f);
  
  // Ensure the file has been opened correctly
  assert(file.is_open());
  
  // Variable used to store each line read from the file
  std::string line;
  
  // Stores the altitude value from the previous iteration (initialized to 0)
  double old_altitude{0.};
  
  // Current altitude read from file
  double altitude;
  
  // Current angle read from file
  double angle;
  
  // Rescale and adjust the position value.
  // First: scale pos proportionally to orbital height.
  // Second: subtract a correction term (500,000 / pos).
  // Finally: ensure pos is not negative.
  pos = std::max(0., (pos * 170'000) / orbital_h - 5e5 / pos);
  
  // Slightly slow down the rate at which the angle changes
  // (initial previous angle set to 2)
  double old_ang{2.};
  
  // Flag used to stop the search once the correct altitude is found
  bool found{false};
  
  // Move the file read position to the previously stored position
  file.seekg(file_pos);
  
  // Read the file line by line until the desired altitude is found
  while (std::getline(file, line) && !found) {
    
    // Create a string stream to extract numerical values from the line
    std::istringstream iss(line);
    
    // Extract altitude and angle from the current line
    iss >> altitude >> angle;
    
    // Check if the current altitude is greater than or equal to the target position
    if (altitude >= pos) {
      found = true;
      
      // Compute distance between current altitude and target position
      double const delta1{std::abs(altitude - pos)};
      
      // Compute distance between previous altitude and target position
      double const delta2{std::abs(old_altitude - pos)}; 
      
      // Determine which altitude (current or previous) is closer to pos
      if (delta1 <= delta2) {
        // Return the smaller value between the new angle and theta
        // (prevents angle from exceeding theta)
        return angle > theta ? theta : angle;
      } else {
        // Return the smaller value between the previous angle and theta
        return old_ang > theta ? theta : old_ang;
      }
    }
    
    // If not yet found, update previous values for next iteration
    if (!found) {
      old_altitude = altitude;     // Store current altitude as previous
      old_ang = angle;             // Store current angle as previous
      file_pos = file.tellg();     // Update file position marker
    }
  }
  
  // If no suitable altitude was found, return 0
  return 0.;
}



/* -------------------------------------------------------------------------- */
/*                               thrust                           */
/* -------------------------------------------------------------------------- */



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




bool is_circular_equatorial_orbit(double r,
                                  double vr,
                                  double omega)
{
    assert(r > 0);

    // Earth constants
    const double mu = sim::cost::G_ * sim::cost::earth_mass_;

    // Tangential velocity (including Earth's rotation)
    const double vt =
        r * omega + sim::cost::earth_speed_;

    // Circular orbital speed at radius r
    const double v_circ = std::sqrt(mu / r);

    // Numerical tolerances
    const double eps_vr = 1e2;    // radial velocity tolerance (m/s)
    const double eps_vt = 1e2;    // tangential speed tolerance (m/s)

    // Conditions for circular equatorial orbit
    bool radial_ok = std::abs(vr) < eps_vr;
    bool tangential_ok = std::abs(vt - v_circ) < eps_vt;

    return radial_ok && tangential_ok;
}

  

 Vec const g_force(double altitude, double mass) {
  const double mu =
        sim::cost::G_ * sim::cost::earth_mass_;

    const double Omega =
        sim::cost::earth_angular_speed_;

    // Gravitational force
    double Fg = - mu * total_mass / (r * r);

    // Centrifugal force
    double Fcent = total_mass * Omega * Omega * r;

    // Coriolis force (tangential only)
    double Fcoriolis = -2.0 * total_mass * Omega * vr;

    double Fr   = Fg + Fcent;
    double Fpsi = Fcoriolis;

    return {Fr, Fpsi};
}



double const Cd_from_Mach(double M) {
   // Realistic drag coefficient model for slender rockets as a function of Mach number.
   // Inspired by experimental rocket/missile aerodynamics and NASA trends.
   // Smooth and continuous (no discontinuities in Cd or its derivative).
    // --- Baseline drag coefficients for different regimes ---
    const double Cd_subsonic = 0.18;   // typical slender rocket Cd at low Mach
    const double Cd_transonic_peak = 0.70; // drag divergence near Mach 1
    const double Cd_supersonic = 0.30; // average supersonic Cd
    const double Cd_hypersonic = 0.23; // slightly lower Cd at hypersonic speeds

    // --- Smooth transition functions using tanh ---
    // Transition from subsonic to transonic (around Mach 0.9 - 1.0)
    double t1 = 0.5 * (1.0 + std::tanh((M - 0.90) / 0.08));

    // Transition from transonic to supersonic (around Mach 1.2 - 1.5)
    double t2 = 0.5 * (1.0 + std::tanh((M - 1.30) / 0.20));

    // Transition from supersonic to hypersonic (around Mach 5)
    double t3 = 0.5 * (1.0 + std::tanh((M - 5.00) / 1.00));

    // --- Build Cd curve step by step ---
    // Subsonic -> Transonic peak
    double Cd_transonic = Cd_subsonic +
        (Cd_transonic_peak - Cd_subsonic) * t1;

    // Transonic -> Supersonic decay
    double Cd_sup = Cd_transonic +
        (Cd_supersonic - Cd_transonic_peak) * t2;

    // Supersonic -> Hypersonic asymptote
    double Cd_final = Cd_sup +
        (Cd_hypersonic - Cd_supersonic) * t3;

    return Cd_final;
}

Vec const drag(double rho, double altitude, double theta,
               double upper_area, Vec const& velocity, double a) {

    // If atmosphere is negligible, no drag
    if (altitude > 51000.0) {
        return {0.0, 0.0};
    }

  
    // Avoid division by zero at very low speed
    if (velocity[0] < 1e-6) {
        return {0.0, 0.0};
    }

    // Extract polar velocity components
    double vr    = velocity[0];      // radial velocity
    double omega = velocity[1];      // angular velocity
      // Total speed magnitude
    double v = std::sqrt(vr*vr + vpsi*vpsi);

    // Compute Mach number using your speed of sound model
    double M = velocity[0] / a;

    // Compute drag coefficient depending on Mach number
    double Cd = Cd_from_Mach(M);

    // Drag magnitude: Fd = 0.5 * rho * v^2 * Cd * A
    double Fd = 0.5 * rho * v* v * Cd * upper_area;

    double Fr   = -Fd * (vr / v);
    double Fpsi = -Fd * (vpsi / v);

    return {fr, fpsi};
}

 Vec const total_force(double rho, double theta, double total_mass, double pos,
                      double upper_area, Vec const& velocity, Vec const& eng) {
  Vec const gra{g_force(pos, total_mass)};
  Vec const drag_f{drag(rho, pos, theta, upper_area, velocity)};
  double const r_force{eng[0]  + gra[1] + drag_f[0]};
  double const x_force{eng[1] + gra[2] + drag_f[1]};
  if (x_force <= 0) {
    return {y_force, 0};
  } else {
    return {y_force, x_force};
  }
}

}

    

    






