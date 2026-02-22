#include "simulation.h"

#include <cmath>
#include <cassert>
#include <algorithm>

namespace sim {

/* -------------------------------------------------------------------------- */
/*                                Getter methods                              */
/* -------------------------------------------------------------------------- */

double Air_var::get_p() const
{
    return p_; // Pressure [Pa]
}

double Air_var::get_rho() const
{
    return rho_; // Density [kg/m^3]
}

double Air_var::get_t() const
{
    return t_; // Temperature [K]
}

/* -------------------------------------------------------------------------- */
/*                      Atmospheric state (ISA standard)                       */
/* -------------------------------------------------------------------------- */
/*
   INPUT:
     altitude : geometric altitude above sea level [m]

   MODEL:
     International Standard Atmosphere (ISA)
     Valid up to ~51 km

   NOTES:
     - altitude is clamped to >= 0
     - temperature is never allowed to reach 0 K
     - pressure and density smoothly go to zero above the model ceiling
*/

void Air_var::set_state(double altitude)
{
    // -----------------------------------------------------------------------
    // Safety: altitude must be non-negative
    // -----------------------------------------------------------------------
    altitude = std::max(0.0, altitude);

    // Convert altitude to kilometers (ISA uses km for layer definitions)
    const double h = altitude * 1e-3; // [km]

    // Physical constants 
    constexpr double g0 = 9.80665; // [m/s^2]
    constexpr double Rspec = cost::R_ / cost::molar_mass_; // [J/(kg·K)]

    // Sea level reference conditions
    constexpr double T0 = 288.15; // [K]
    constexpr double P0 = cost::sea_pression_; // [Pa]

    // -----------------------------------------------------------------------
    // ISA layers
    // -----------------------------------------------------------------------

    if (h <= 11.0) {
        // Troposphere
        constexpr double L = -6.5e-3; // Temperature lapse rate [K/m]
        t_ = T0 + L * altitude;
        p_ = P0 * std::pow(t_ / T0, -g0 / (L * Rspec));
    }
    else if (h <= 20.0) {
        // Tropopause (isothermal)
        constexpr double T11 = 216.65; // [K]
        constexpr double h11 = 11'000.0; // [m]

        const double p11 =
            P0 * std::pow(T11 / T0, -g0 / (-6.5e-3 * Rspec));

        t_ = T11;
        p_ = p11 * std::exp(-g0 * (altitude - h11) / (Rspec * T11));
    }
    else if (h <= 32.0) {
        // Lower stratosphere
        constexpr double L = 1.0e-3; // [K/m]
        constexpr double T20 = 216.65; // [K]
        constexpr double h20 = 20'000.0; // [m]

        const double p20 =
            P0 *
            std::pow(T20 / T0, -g0 / (-6.5e-3 * Rspec)) *
            std::exp(-g0 * (h20 - 11'000.0) / (Rspec * T20));

        t_ = T20 + L * (altitude - h20);
        p_ = p20 * std::pow(t_ / T20, -g0 / (L * Rspec));
    }
    else if (h <= 47.0) {
        // Middle stratosphere
        constexpr double L = 2.8e-3; // [K/m]
        constexpr double T32 = 228.65; // [K]
        constexpr double h32 = 32'000.0; // [m]

        const double p32 =
            P0 *
            std::pow(216.65 / T0, -g0 / (-6.5e-3 * Rspec)) *
            std::exp(-g0 * (20'000.0 - 11'000.0) / (Rspec * 216.65)) *
            std::pow(T32 / 216.65, -g0 / (1.0e-3 * Rspec));

        t_ = T32 + L * (altitude - h32);
        p_ = p32 * std::pow(t_ / T32, -g0 / (L * Rspec));
    }
    else if (h <= 51.0) {
        // Stratopause (isothermal)
        constexpr double T47 = 270.65; // [K]
        constexpr double h47 = 47'000.0; // [m]

        const double p47 =
            P0 *
            std::pow(216.65 / T0, -g0 / (-6.5e-3 * Rspec)) *
            std::exp(-g0 * (20'000.0 - 11'000.0) / (Rspec * 216.65)) *
            std::pow(228.65 / 216.65, -g0 / (1.0e-3 * Rspec)) *
            std::pow(T47 / 228.65, -g0 / (2.8e-3 * Rspec));

        t_ = T47;
        p_ = p47 * std::exp(-g0 * (altitude - h47) / (Rspec * T47));
    }
    else {
        // Above ISA validity: atmosphere negligible
        t_ = 270.65; // keep temperature finite
        p_ = 0.0;
        rho_ = 0.0;
        return;
    }

    // -----------------------------------------------------------------------
    // Density from ideal gas law
    // -----------------------------------------------------------------------
    rho_ = p_ / (Rspec * t_);
}

/* -------------------------------------------------------------------------- */
/*                         Speed of sound (ideal gas)                         */
/* -------------------------------------------------------------------------- */

double Air_var::get_speed_sound() const
{
    constexpr double Rspec = cost::R_ / cost::molar_mass_;
    return std::sqrt(cost::gamma_ * Rspec * t_);
}

} // namespace sim



