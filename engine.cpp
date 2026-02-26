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


Vec Base_engine::eng_force(double pa, double time, double theta,
                          bool is_orbiting) {

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
    const double R_univ = sim::cost::R_; // Universal gas constant [J/(kmol*K)]
    double R_spec = R_univ / M_;   // Specific gas constant [J/(kg*K)]
    double term = 1.0 - std::pow(p_e / p_c_, (gamma_ - 1.0) / gamma_);
    return std::sqrt(2.0 * gamma_ / (gamma_ - 1.0) * R_spec * T_c_ * term);
}

double Ad_sol_engine::dpc_dt() const {

    // Specific gas constant
    const double R_univ = sim::cost::R_;      // J/(mol*K)
    double R_spec = R_univ / M_;       // J/(kg*K)

    // Mass generation rate from combustion
    double m_gen = rho_p_ * A_b_ * a_ * std::pow(p_c_, n_);

    // Mass discharge rate through nozzle (choked assumption)
    double m_noz = (p_c_ * A_t_) / c_star_;

    // Chamber pressure time derivative
    return (R_spec * T_c_ / V_c_) * (m_gen - m_noz);
}

double Ad_sol_engine::compute_exit_pressure(double Pc) const {
    // Computes exit pressure Pe from chamber pressure Pc
// using isentropic nozzle relations.
// Assumptions:
// - Flow is choked at the throat (M = 1 at A_t)
// - Isentropic expansion
// - Constant gamma
// - Fixed expansion ratio epsilon = A_e / A_t
    const double epsilon = A_e_ / A_t_;

    // ---------- Step 1: Solve for exit Mach number M_e ----------
    // Use Newton-Raphson method
    double M = 3.0; // initial supersonic guess
    for (int i = 0; i < 30; ++i) {
        // Area-Mach function f(M)
        double term2 = (2.0 / (gamma_ + 1.0)) * (1.0 + 0.5 * (gamma_ - 1.0) * M * M);
        double exponent = (gamma_ + 1.0) / (2.0 * (gamma_ - 1.0));
        double f = (1.0 / M) * std::pow(term2, exponent) - epsilon;

        // Derivative f'(M)
        double gm1 = gamma_ - 1.0;
        double gp1 = gamma_ + 1.0;
        double A = (2.0 / gp1) * (1.0 + 0.5 * gm1 * M * M);
        double dA_dM = (2.0 / gp1) * gm1 * M;
        double df_dM = - (1.0 / M) * std::pow(A, exponent) / M 
                       + (1.0 / M) * exponent * std::pow(A, exponent - 1.0) * dA_dM;

        M -= f / df_dM;

        if (std::abs(f) < 1e-8)
            break;
    }

    if (M <= 1.0) {
        throw std::runtime_error("Invalid exit Mach number");
    }

    // ---------- Step 2: Compute exit pressure Pe ----------
    double pressure_ratio = std::pow(1.0 + 0.5 * (gamma_ - 1.0) * M * M,
                                     -gamma_ / (gamma_ - 1.0));

    return Pc * pressure_ratio;
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
           // Computes characteristic velocity c* [m/s]
// Assumes ideal gas and isentropic combustion products
// gamma  : specific heat ratio
// T_c    : chamber temperature [K]
// M      : molar mass [kg/kmol]
// Universal gas constant [J/(kmol*K)]
    const double R_univ = sim::cost::R_;

    // Specific gas constant [J/(kg*K)]
    double R_spec = R_univ / M;

    // First term: sqrt(R*T_c/gamma)
    double term1 = std::sqrt(R_spec * T_c / gamma_);

    // Second term: ((gamma+1)/2)^((gamma+1)/(2*(gamma-1)))
    double term2 = std::pow(
        (gamma_ + 1.0) / 2.0,
        (gamma_ + 1.0) / (2.0 * (gamma_ - 1.0))
    );

    c_star_=term1 * term2;
    
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
Vec Ad_sol_engine::eng_force(double pa, double time, double theta,
                          bool is_orbiting) {
    // No propellant consumption if orbiting or engine released
    if (is_orbiting || released_) {
        return {0.0,0.0};
    }
    double dp = dpc_dt();
    p_c_ += dp * time;

    if(p_c_ < 0.0)
    {
        p_c_ = 0.0;
    }
    double pe = compute_exit_pressure(p_c_);
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


// ============================================================================
// ADVANCED LIQUID ENGINE IMPLEMENTATION
// ============================================================================

Ad_liquid_engine::Ad_liquid_engine(double p_c, double T_c, double A_t, double A_e, double A_b, double M)
    : p_c_{p_c}, T_c_{T_c}, A_t_{A_t}, A_e_{A_e}, A_b_{A_b}, M_{M} {
    assert(p_c_ > 0 && T_c_ > 0 && A_t_ > 0 && A_e_ > 0 && A_b_ > 0 && M_ > 0);
    double term = 2.0 / (gamma_ + 1.0);
     c_star_ = std::sqrt(sim::cost::R_ * T_c_ / M_) * (1.0 / gamma_) * std::pow(term, (gamma_ + 1.0) / (2.0 * (gamma_ - 1.0)));
        
}

double Ad_liquid_engine::isentropic_area_ratio(double M, double gamma) {
    double term1 = 2.0 / (gamma + 1.0);
    double exponent = (gamma + 1.0) / (2.0 * (gamma - 1.0));
    return (1.0 / M) * std::pow(term1 * (1.0 + (gamma - 1.0) / 2.0 * M * M), exponent);
}


double Ad_liquid_engine::exit_mach(double areaRatio, double gamma) {

    /*
 * Solve for exit Mach number given the area ratio (Ae/At) and gamma
 * Uses a simple bisection method
 * areaRatio : Ae / At
 * gamma     : ratio of specific heats
 * Returns the supersonic exit Mach number
 */

    if (areaRatio <= 1.0) {
        throw std::runtime_error("Exit area must be greater than throat area for supersonic flow");
    }

    double lo = 1.0;  // Mach at throat = 1
    double hi = 50.0; // Arbitrary upper bound for supersonic Mach
    double mid;

    for (int i = 0; i < 80; ++i) { // ~80 iterations for convergence
        mid = 0.5 * (lo + hi);
        double f = isentropic_area_ratio(mid, gamma) - areaRatio;

        if (std::abs(f) < 1e-8) return mid; // convergence
        if (f > 0) hi = mid;
        else lo = mid;
    }

    return mid; // return last midpoint if not exactly converged
}


double Ad_liquid_engine::compute_exit_pressure() {
    /*
 * Compute the exit pressure of a liquid rocket engine
 * p_c : chamber pressure [Pa]
 * Ae  : exit area [m^2]
 * At  : throat area [m^2]
 * gamma : ratio of specific heats
 * Returns the isentropic exit pressure [Pa]
 */

    double areaRatio = A_e_ / A_t_;

    // Solve for supersonic exit Mach number
    double Me = exit_mach(areaRatio, gamma_);

    // Apply isentropic relation to get exit pressure
    double pe_pc = std::pow(1.0 + (gamma_ - 1.0) / 2.0 * Me * Me,
                             -gamma_ / (gamma_ - 1.0));

    return p_c_ * pe_pc;
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

// Mass depletion during burn
double Ad_liquid_engine::delta_m(double time, bool is_orbiting) const {
    if (is_orbiting || released_) {
        return 0.0;
    }
    return mass_flow_rate() * time;
}

// Force vector aligned with thrust direction
Vec Ad_liquid_engine::eng_force(double pa, double time, double theta,
                          bool is_orbiting)  {
    
     // No propellant consumption if orbiting or engine released
    if (is_orbiting || released_) {
        return {0.0,0.0};
    }
    //in this kind of engine we assume that the chamber pressure is constant and equal to the initial one
    // so we compute the exit pressure from the initial chamber pressure, otherwise much more complex models
    // would be needed to compute the time evolution

    if(p_c_ < 0.0)
    {
        p_c_ = 0.0;
    }

    double pe   = compute_exit_pressure();
    double mdot = mass_flow_rate();
    double v_e  = exhaust_velocity(pe);

    double thrust =
        mdot * v_e +
        (pe - pa) * A_e_;

    double F_r   = thrust * std::sin(theta);
    double F_psi = thrust * std::cos(theta);

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

} // namespace engine
