#include "engine.h"
#include "simulation.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace engine {


// ============================================================================
// BASE ENGINE IMPLEMENTATION
// ============================================================================

Base_engine::Base_engine(double isp, double cm, double p0, double burn_a)
    : isp_{isp}, cm_{cm}, p_0_{p0}, burn_a_{burn_a}
{
    // Basic physical validation
    assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0 && burn_a_ >= 0);
}

// Computes mass depletion assuming constant mass flow coefficient.
double Base_engine::delta_m(double dt, bool is_orbiting) const {

    // No propellant consumption if orbiting or engine released
    if (is_orbiting || released_) {
        return 0.0;
    }

    // Mass flow rate (kg/s)
    double m_dot = p_0_ * burn_a_ * cm_;

    // Propellant consumed during this timestep
    return m_dot * dt;
}


Vec Base_engine::eng_force(double pa, double pe, double theta,
                                 bool is_orbiting) const {

    if (is_orbiting || released_) {
        return {0.0, 0.0};
    }

    constexpr double g0 = 9.80665;

    double m_dot = p_0_ * burn_a_ * cm_;
    double thrust = m_dot * isp_ * g0;

    // theta = pitch angle from local horizontal
    // r̂ = radial direction
    // ψ̂ = tangential direction
    double F_r   = thrust * std::sin(theta);
    double F_psi = thrust * std::cos(theta);

    return {F_r, F_psi};
}


void Base_engine::release() {
    released_ = true;
}

bool Base_engine::is_ad_eng() const {
    return false;
}

bool Base_engine::is_liquid() const {
    return false;
}

bool Base_engine::is_released() const {
    return released_;
}

double Base_engine::get_pression() const {
    return p_0_;
}



// ============================================================================
// ADVANCED SOLID ENGINE IMPLEMENTATION
// ============================================================================

// ------------------------
// Private helper functions
// ------------------------

// Computes the regression rate of the propellant [m/s]
// Formula: r_dot = a * p_c^n
double Ad_sol_engine::regression_rate() const {
    return a_ * std::pow(p_c_, n_);
}

// Computes the propellant mass flow rate [kg/s]
// Formula: m_dot = rho_p * A_b * r_dot
double Ad_sol_engine::mass_flow_rate() const {
    double r_dot = regression_rate();
    return rho_p_ * A_b_ * r_dot;
}

// Computes the exhaust velocity using isentropic expansion [m/s]
// Formula: v_e = sqrt( 2 * gamma / (gamma-1) * R_spec * T_c * (1 - (p_e/p_c)^((gamma-1)/gamma)) )
double Ad_sol_engine::exhaust_velocity(double p_e) const {
    if(p_e > p_c_){
        throw std::runtime_error("pe > pc");
    }
    const double R_univ = 8314.5; // Universal gas constant [J/(kmol*K)]
    double R_spec = R_univ / M_;   // Specific gas constant [J/(kg*K)]
    double term = 1.0 - std::pow(p_e / p_c_, (gamma_ - 1.0) / gamma_);
    return std::sqrt(2.0 * gamma_ / (gamma_ - 1.0) * R_spec * T_c_ * term);
}

// ------------------------
// Public interface methods
// ------------------------

Ad_sol_engine::Ad_sol_engine(double p_c,
                             double T_c,
                             double A_b,
                             double A_t,
                             double rho_p,
                             double a,
                             double n,
                             double M)
    : p_c_{p_c},
      T_c_{T_c},
      A_b_{A_b},
      A_t_{A_t},
      rho_p_{rho_p},
      a_{a},
      n_{n},
      M_{M} {
    assert(p_c_ > 0 && T_c_ > 0 && A_b_ > 0 && A_t_ > 0 && rho_p_ > 0 &&
           a_ > 0 && n_ > 0 && M_ > 0);
}

// Returns the change in mass of propellant over a timestep [kg]
// If the engine is orbiting, combustion may differ (optional extension)
double Ad_sol_engine::delta_m(double time, bool is_orbiting) const {
    // No propellant consumption if orbiting or engine released
    if (is_orbiting || released_) {
        return 0.0;
    }
    // Mass consumed = mass flow rate * delta time
    return mass_flow_rate() * time;
}

// Computes engine thrust [N]
// eng_par contains any required parameters like external pressure
Vec Ad_sol_engine::eng_force(double pa, double pe, double theta, bool is_orbiting) const {
    // No propellant consumption if orbiting or engine released
    if (is_orbiting || released_) {
        return {0.0,0.0};
    }
    // Compute exhaust velocity
    double v_e = exhaust_velocity(pe); 

    // Mass flow rate
    double m_dot = mass_flow_rate();

    // Ideal thrust formula: F = m_dot * v_e + (p_e - p_a) * A_e
    double thrust = m_dot * v_e + (pe - pa) * A_e_;
    double F_r   = thrust * std::sin(theta);
    double F_psi = thrust * std::cos(theta);

    return {F_r, F_psi};

}

// Release the engine (sets the flag to true)
void Ad_sol_engine::release() {
    released_ = true;
}

// Returns true if this is an advanced solid engine
bool Ad_sol_engine::is_ad_eng() const {
    return true;
}

bool Ad_sol_engine::is_liquid() const {
    return liquid_;
}

// Returns true if the engine has been released
bool Ad_sol_engine::is_released() const {
    return released_;
}

// Returns the chamber pressure [Pa]
double Ad_sol_engine::get_pression() const {
    return p_c_;
}



// ============================================================================
// ADVANCED LIQUID ENGINE IMPLEMENTATION
// ============================================================================

Ad_liquid_engine::Ad_liquid_engine(double p_c, double T_c, double A_t, double A_e)
    : p_c_{p_c}, T_c_{T_c}, A_t_{A_t}, A_e_{A_e} {
    assert(p_c_ > 0 && T_c_ > 0 && A_t_ > 0 && A_e_ > 0);
}

// Mass flow from characteristic velocity formulation
double Ad_liquid_engine::mass_flow_rate() const {
    return (p_c_ * A_t_) / c_star_;
}

// Isentropic exhaust velocity
double Ad_liquid_engine::exhaust_velocity(double p_e) const {

    if(p_e > p_c_){
        throw std::runtime_error("pe > pc");
    }
    double term = 1.0 - std::pow(p_e / p_c_,
                    (gamma_ - 1.0) / gamma_);

    return std::sqrt((2.0 * gamma_ / (gamma_ - 1.0)) *
                     (sim::cost::R_ * T_c_ / M_) * term);
}

// Thrust including pressure term
double Ad_liquid_engine::thrust(double pa, double pe) const {

    double mdot = mass_flow_rate();
    double v_e  = exhaust_velocity(pe);

    return mdot * v_e + (pe - pa) * A_e_;
}

// Mass depletion during burn
double Ad_liquid_engine::delta_m(double time, bool is_orbiting) const {
    if (is_orbiting || released_) {
        return 0.0;
    }
    return mass_flow_rate() * time;
}

// Force vector aligned with thrust direction
Vec Ad_liquid_engine::eng_force(double pa, double pe, double theta,
                                bool is_orbiting) const {
    
    if (is_orbiting || released_) {
        return {0.0,0.0};
    }

    double F_r   = thrust(pa,pe) * std::sin(theta);
    double F_psi = thrust(pa,pe) * std::cos(theta);

    return {F_r, F_psi};

}

void Ad_liquid_engine::release() {
    released_ = true;
}

bool Ad_liquid_engine::is_ad_eng() const {
    return true;
}

bool Ad_liquid_engine::is_liquid() const {
    return liquid_;
}

bool Ad_liquid_engine::is_released() const {
    return released_;
}

double Ad_liquid_engine::get_pression() const {
    return p_c_;
}

} // namespace engine
