#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "simulation.h"
#include "rocket.h"
#include "vector_math.h"
#include "engine.h"


namespace rocket {
class Rocket;
//
// angular component [1]
// radial module component [0]
//

/* --------------------------------------------------------------------------*/
/*                           costructor                              
/* -------------------------------------------------------------------------- */

Rocket::Rocket(std::string const& name, double mass_structure, double Up_Ar,
               double s_p_m, double m_s_cont, std::vector<double> const& l_p_m,
               std::vector<double> const& l_c_m, engine::Engine* engs, engine::Engine* engl, int n_solid_eng,
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

      total_stage_{static_cast<int>(l_p_m.size()) + 1}, //only one solid stage, liquid stages are determined by the size of the vector
      n_sol_eng_{n_solid_eng},
      n_liq_eng_{n_liq_eng} {
  assert(m_liq_cont_.size() == m_liq_prop_.size() &&
         n_liq_eng_.size() == m_liq_prop_.size());
  assert(upper_area_ > 0 && m_sol_cont_ > 0 && m_sol_prop_ > 0);
  current_stage_ = total_stage_;
  std::for_each(n_liq_eng.begin(), n_liq_eng.end(),
                [](int value) { if(value <=0){
                  throw std::runtime_error("invalid value inserted");
                }
                });
  std::for_each(m_liq_cont_.begin(), m_liq_cont_.end(),
                [](double value) { if(value <=0){
                  throw std::runtime_error("invalid value inserted");
                } });
  std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                [](double value) { if(value <=0){
                  throw std::runtime_error("invalid value inserted");
                } });
                engl_=engl;
                engs_=engs;
}


/* -------------------------------------------------------------------------- */
/*                                Rocket getters methods                              */
/* -------------------------------------------------------------------------- */

double Rocket::get_theta() const { return theta_; }

double Rocket::get_mass() const { return total_mass_; }

Vec const Rocket::get_velocity() const { return velocity_; }

Vec const Rocket::get_pos() const { return pos_; }

int Rocket::get_rem_stage() const { return current_stage_; }

double Rocket::get_fuel_left() const { 
  if(current_stage_==0){
    return 0;
  } else{
  return (std::accumulate(m_liq_prop_ .begin(), m_liq_prop_ .end(), 0) + m_sol_prop_); 
  }}

double Rocket::get_up_ar() const { return upper_area_; }

double Rocket::get_altitude() const {return pos_[0] - sim::cost::earth_radius_;}

/* -------------------------------------------------------------------------- */
/*                               kinematics rocket                              */
/* -------------------------------------------------------------------------- */





void Rocket::change_vel(double dt, Vec const& force) {
  // ============================================================
  // Runge-Kutta 2 (Midpoint Method) Integration
  // Reference frame: Inertial, centered at Earth (non-rotating)
  // Coordinates: Polar (r, psi)
  // ============================================================
  
  double r  = pos_[0];      // radial position [m]
  double vr = velocity_[0]; // radial velocity [m/s]
  double vt = velocity_[1]; // tangential velocity [m/s]

  if (r <= 1e-8) {
    throw std::runtime_error("Radius too small (singularity at r=0)");
  }

  // Force components (already include gravity, drag, thrust)
  double Fr   = force[0];   // radial force component [N]
  double Ft   = force[1];   // tangential force component [N]
  double m = total_mass_;   // rocket mass [kg]

  // ============================================================
  // RK2 METHOD - STEP 1: Compute half-step accelerations
  // ============================================================
  
  // Equations of motion in inertial polar coordinates:
  // d(vr)/dt = Fr/m - (vt^2)/r  (centripetal acceleration, negative!)
  // d(vt)/dt = Ft/m + (vr*vt)/r (Coriolis-like geometric term)
  
  double ar_initial = Fr / m - (vt * vt) / r;
  double at_initial = Ft / m + (vr * vt) / r;
  
  // Half-step velocities
  double vr_half = vr + 0.5 * ar_initial * dt;
  double vt_half = vt + 0.5 * at_initial * dt;
  
  // ============================================================
  // RK2 METHOD - STEP 2: Compute accelerations at half-step
  // ============================================================
  
  double ar_half = Fr / m - (vt_half * vt_half) / r;
  double at_half = Ft / m + (vr_half * vt_half) / r;
  
  // ============================================================
  // RK2 METHOD - STEP 3: Update velocities using half-step accelerations
  // ============================================================
  
  velocity_[0] += ar_half * dt;
  velocity_[1] += at_half * dt;
}


void Rocket::move(double dt, Vec const& force) {

 
// assumed reference frame centered inthe core of the earth not rotating
  // ============================================================
  // Integration using RK2 for both velocity and position
  // ============================================================
  
  // Current state
  double r  = pos_[0];
  double psi = pos_[1];
  double vr = velocity_[0];
  double vt = velocity_[1];
  double m = total_mass_;

  // sanity: ensure values are finite
  if (!std::isfinite(r) || !std::isfinite(vr) || !std::isfinite(vt) || !std::isfinite(m)) {
      throw std::runtime_error("Non-finite state detected in rocket.move");
  }
  
  // Force components
  double Fr = force[0];
  double Ft = force[1];
  
  if (r <= 1e-8) {
    throw std::runtime_error("Radius too small (singularity at r=0)");
  }
  
  // ============================================================
  // RK2 METHOD - STEP 1: Compute half-step state
  // ============================================================
  
  // Accelerations at current state
  double ar_0 = Fr / m - (vt * vt) / r;
  double at_0 = Ft / m + (vr * vt) / r;
  
  // Position and velocity at t + dt/2
  double vr_half = vr + 0.5 * ar_0 * dt;
  double vt_half = vt + 0.5 * at_0 * dt;
  double r_half = r + 0.5 * vr_half * dt;
  
  // Avoid singularity at r=0
  if (r_half <= 1e-8) {
    throw std::runtime_error("Radius too small at half-step");
  }
  
  // ============================================================
  // RK2 METHOD - STEP 2: Compute accelerations at half-step
  // ============================================================
  
  double ar_half = Fr / m - (vt_half * vt_half) / r_half;
  double at_half = Ft / m + (vr_half * vt_half) / r_half;
  
  // ============================================================
  // RK2 METHOD - STEP 3: Full-step integration
  // ============================================================
  
  // Update velocity
  velocity_[0] += ar_half * dt;
  velocity_[1] += at_half * dt;
  
  // Update position (radial)
  pos_[0] += velocity_[0] * dt;
  
  // Update position (angular)
  // dψ/dt = vt / r, use updated radius for stability
  double r_for_angle = std::max(pos_[0], 1e-8);
  pos_[1] += (velocity_[1] / r_for_angle) * dt;
  
  // ============================================================
  // Safety checks
  // ============================================================
  
  if (pos_[0] <= 0.0) {
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

void Rocket::set_state(std::ifstream& theta_file,
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

    // Improve flight angle based on guidance file using the altitude, +0.005 avoid singularity
    theta_ = improve_theta(theta_file, theta_, pos_[0]-sim::cost::earth_radius_+0.005,
                           orbital_h, file_pos);

    // ============================================================
    // 2. Re-orient velocity to be tangential to trajectory
    // ============================================================

    // Once above 20 km, align velocity with updated angle
    if (get_altitude() > 20000.0 && std::abs(old_theta) > 1e-8)
    {
        // Compute current speed magnitude
        const double speed = velocity_.norm();

        // Redistribute velocity components according to new angle
        velocity_[0] = speed * std::sin(theta_);
        velocity_[1] = speed * std::cos(theta_);
    }

    // ============================================================
    // 3. Compute propellant mass loss
    // ============================================================


    if (!engs_ || !engl_)
        throw std::runtime_error("Engine pointer is null");


    //compute mass loss
    double const solid_burn = engs_->delta_m(time, is_orbiting) * n_sol_eng_;
    double liquid_burn = 0.0;
    // FIX: Check if liquid engines exist before calculating burn to prevent crash.
    if (!n_liq_eng_.empty()) {
        liquid_burn = engl_->delta_m(time, is_orbiting) * n_liq_eng_.front();
    }

    // Apply mass loss
    mass_lost(solid_burn, liquid_burn);

    // ============================================================
    // 4. Check for stage separation
    // ============================================================

    if (current_stage_ != 0 && n_liq_eng_.size() >= 1){
        stage_release(solid_burn, liquid_burn);
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

    // If there are no more liquid stages, there's nothing to release.
    // This prevents a crash from accessing an empty m_liq_prop_ vector.
    if (m_liq_prop_.empty()) {
      return;
    }

    // Number of remaining liquid stages
    int const len{static_cast<int>(m_liq_prop_.size())};

    // Check that current_stage_ index is consistent
    if(!(current_stage_ < len + 1 && current_stage_ >= 0)){
      throw std::runtime_error("error in stage release");
    }

    if(n_liq_eng_.size() == 0){
      throw std::runtime_error("error in vectori liquid engine");
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
        engl_->release();
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
      engs_->release();

      std::cout << "stage released" << "\n";
    }
  }
}



double improve_theta(std::ifstream& file, double theta, double pos,
                     double orbital_h, std::streampos& file_pos) {
  // Ensure the file stream is valid and has been opened correctly
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
  pos = std::max(0., (pos * 170'000) / orbital_h);
  
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



Vec const Rocket::thrust(double time, double pa, bool is_orbiting) const {
 if (n_liq_eng_.empty()){
    return {0.0, 0.0}; //no more engine active
  }
  if (!engs_ || !engl_){
    throw std::runtime_error("Engine pointer is null");
  }
  if(engs_->is_released() && engl_->is_released()){
    return {0.0, 0.0}; //no more engine active
  }
  
  Vec engs = engs_->eng_force(pa,time,theta_, is_orbiting);
  Vec engl = engl_->eng_force(pa,time,theta_, is_orbiting);
  
  if (engs.norm() < 1e-8 && engl.norm() < 1e-8) {
    return {0.0, 0.0}; // No thrust if both engines are inactive
  }
 
  double const fr{engs[0] * n_sol_eng_ + engl[0] * n_liq_eng_[0]};
  double const fpsi{engs[1] * n_sol_eng_ + engl[1] * n_liq_eng_[0]};
  return {fr, fpsi};
}


bool is_orbiting(double r, Vec velocity)
{
    // robust check: r must be positive and finite. return false otherwise.
    if (!(r > 0.0) || !std::isfinite(r)) {
        return false;
    }

    double const vr = velocity[0];      // radial velocity [m/s]
    double const vt = velocity[1];      // tangential velocity [m/s] (inertial frame)

    // Earth constants
    const double mu = sim::cost::G_ * sim::cost::earth_mass_;

    // Circular orbital speed at radius r (in inertial frame)
    // For a stable circular orbit: vt = sqrt(mu/r), vr = 0
    const double v_circ = std::sqrt(mu / r);

    // Numerical tolerances for orbit detection
    const double eps_vr = 50.0;    // radial velocity tolerance (m/s)
    const double eps_vt = 50.0;    // tangential velocity tolerance (m/s)

    // Conditions for circular equatorial orbit
    bool radial_ok = std::abs(vr) < eps_vr;
    bool tangential_ok = std::abs(vt - v_circ) < eps_vt;

    return radial_ok && tangential_ok;
}

  

Vec g_force(double r, double mass, double vr) {
  // ============================================================
  // Gravitational force in INERTIAL reference frame
  // (centered at Earth's center, non-rotating)
  // ============================================================
  // In this frame, there are NO fictitious forces
  // (no centrifugal force, no Coriolis force)
  // Only the true gravitational force acts.
  // ============================================================
  
  const double mu = sim::cost::G_ * sim::cost::earth_mass_;

  // Gravitational force (radial direction, toward Earth center)
  // F_r = -mu * mass / r^2   (negative = toward center)
  double Fr = -mu * mass / (r * r);
  
  // Tangential component (always zero for gravity in spherical symmetry)
  double Ft = 0.0;

  return {Fr, Ft};
}



double Cd_from_Mach(double M) {
   // Realistic drag coefficient model for slender rockets as a function of Mach number.
   // Inspired by experimental rocket/missile aerodynamics and NASA trends.
   // Smooth and continuous (no discontinuities in Cd or its derivative).
    // --- Baseline drag coefficients for different regimes ---
    const double Cd_subsonic = 0.18;   // typical slec_star_ = std::sqrt(sim::cost::R_ * T_c_ / M_) * (1.0 / gamma_) * ...nder rocket Cd at low Mach
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

Vec drag(double rho, double altitude,
               double upper_area, Vec const& velocity, double a) {

    // If atmosphere is negligible, no drag
    if (altitude > 51000.0) {
        return {0.0, 0.0};
    }
    // Extract polar velocity components
    double vr    = velocity[0];      // radial velocity
    double vpsi = velocity[1]; // angular velocity
      // Total speed magnitude  
    double v = velocity.norm();

    // Avoid division by zero at very low speed
    if (v < 1e-7) {
        return {0.0, 0.0};
    }

    // Compute Mach number using your speed of sound model
    double M = v / a;

    // Compute drag coefficient depending on Mach number
    double Cd = Cd_from_Mach(M);

    // Drag magnitude: Fd = 0.5 * rho * v^2 * Cd * A
    double fd = 0.5 * rho * v* v * Cd * upper_area;

    double fr   = -fd * (vr / v);
    double fpsi = -fd * (vpsi / v);

    return {fr, fpsi};
}

 Vec const total_force(double rho, double total_mass, double altitude,
                      double upper_area, Vec const& velocity, Vec const& eng, double a) {
  Vec const gra{g_force(altitude + sim::cost::earth_radius_, total_mass, velocity[0])};
  Vec const drag_f{drag(rho, altitude, upper_area, velocity, a)};
  //pay attention is altitude
  double const r_force{eng[0]  + gra[0] + drag_f[0]};
  double const psi_force{eng[1] + gra[1] + drag_f[1]};
  if (psi_force <= 0 && altitude < 10000.0) {
    return {r_force, 0};
  } else {
    return {r_force, psi_force};
  }
}
}


    
