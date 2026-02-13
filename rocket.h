#ifndef ROCKET_H
#define ROCKET_H

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "assert.h"
#include "simulation.h"
#include "vector_math.h"

namespace rocket {

 struct eng_par{
  double theta;
  double time;
  double pos; 
};

class Engine {
   public:
    virtual double delta_m(double time, bool is_orbiting) const = 0;

    virtual Vec const eng_force(eng_par const& par,
                                       bool is_orbiting) const = 0;

    virtual void release() = 0;

    virtual bool is_ad_eng() const = 0;

    virtual bool is_released() const = 0;

    virtual double get_pression() const = 0;

    virtual ~Engine() = default;
  };

  class Base_engine final : public Engine {
    double isp_{250};  // impulso specifico per tempo
    double cm_{4.};    // coefficiente perdita massa
    double p_0_{5e6}; //pressione
    double burn_a_{200e-6}; //area di combustione
    bool released_{false};
    
   public:
    explicit Base_engine(double isp, double cm, double p0,
                                 double burn_a);

    explicit Base_engine(double isp, double cm, double p0)
        : isp_{isp}, cm_{cm}, p_0_{p0} {
      assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0);
    }

    Base_engine() = default;

    double delta_m(double time, bool is_orbiting) const override;

    void release() override;

    Vec const eng_force(eng_par const& par,
                                       bool is_orbiting) const override;

    virtual bool is_ad_eng() const override;

    virtual bool is_released() const override;

    virtual double get_pression() const override;

    ~Base_engine() override = default;
  };

  class Ad_engine final : public Engine {
    double p_0_{5e6};  //pressione
    double burn_a_{200e-6}; //area di combustione
    double nozzle_as_{221.0e-6}; //area beccuccio
    double t_0_{1710.0}; //temperatura
    double grain_rho_{1873}; //densità combustibile
    double grain_dim_{0.02}; //dimensione dei grani
    double burn_rate_a_{0.01};
    double burn_rate_n_{0.02};
    double prop_mm_{178}; //massa molare combustibile
    bool released_{false};
   public:
    explicit Ad_engine(double burn_a, double nozzle_as, double t_0,
                             double grain_dim, double grain_rho, double a_coef,
                             double burn_rate_n, double prop_mm);

    explicit Ad_engine(double p_0, double burn_a, double nozzle_as, double t_0);

    explicit Ad_engine() = default;

    double delta_m(double time, bool is_orbiting) const override;

    void release() override;

    Vec const eng_force(eng_par const& par,
                                       bool is_orbiting) const override;

    virtual bool is_ad_eng() const override;

    virtual bool is_released() const override;

    virtual double get_pression() const override;

    ~Ad_engine() override = default;
  };

class Rocket {
  // Rocket basic configuration and state parameters

// Rocket name identifier
std::string name_{"my_rocket"};

// Cross-sectional upper area of the rocket (m^2)
double upper_area_{80.0};

// Solid propulsion system parameters

// Mass of the solid fuel container (kg)
double m_sol_cont_{15000.0};

// Mass of the solid propellant (kg)
double m_sol_prop_{40000.0};

// Liquid propulsion system parameters (per stage or per tank)

// Mass of liquid propellant (kg)
// Stored as a vector to support multi-stage rockets
std::vector<double> m_liq_prop_{15000.0};

// Mass of liquid fuel container/tank (kg)
// Stored as a vector to support multi-stage rockets
std::vector<double> m_liq_cont_{40000.0};

// Total initial mass of the rocket (kg)
double total_mass_{135000.0};

// Total number of rocket stages
int total_stage_{1};

// Current velocity vector (m/s) (r/psi)
Vec velocity_{0.0, 0.0};

// Current position vector (m) (r,psi)
Vec pos_{sim::cost::earth_radius_, 0.0};

// Index of the currently active stage
int current_stage_{1};

// Rocket inclination angle (radians)
// Initialized to pi/2 ≈ 1.57079632 (vertical launch)
double theta_{1.57079632};

// Pointer to the engine currently associated with the rocket
Engine* eng_;

// Number of solid engines
int n_sol_eng_{1};

// Number of liquid engines per stage
// Stored as a vector to support multi-stage configurations
std::vector<int> n_liq_eng_;

 public:
  // costruttore con tutto
  explicit Rocket(std::string const& name, double mass_structure, double Up_Ar,
               double s_p_m, double m_s_cont, std::vector<double> const& l_p_m,
               std::vector<double> const& l_c_m, Engine* eng,
               int n_solid_eng, std::vector<int> const& n_liq_eng);

  Rocket() = default;

  /* -------------------------------------------------------------------------- */
/*                                Rocket getters methods                              */
/* -------------------------------------------------------------------------- */

  Vec const get_velocity() const;

  Vec const get_pos() const;

  void mass_lost(double solid_lost, double liq_lost);

  double get_theta() const;

  void move(double time, Vec const& force);

  double get_up_ar() const;

  int get_rem_stage() const;

  double get_fuel_left() const;

  double get_lat_ar() const;

  double get_mass() const;

  double get_altitude() const;

/* -------------------------------------------------------------------------- */
/*                               kinematics rocket                              */
/* -------------------------------------------------------------------------- */


  void set_state(std::string const& file_name, double orbital_h, double time,
                       bool is_orbiting, std::streampos& file_pos);

  void stage_release(double delta_ms, double delta_ml);  

  void change_vel(double time, Vec const& force);

  Vec const thrust(double time, bool is_orbiting) const; //engine thrust
};


/* -------------------------------------------------------------------------- */
/*                               Force computation                              */
/* -------------------------------------------------------------------------- */

Vec const total_force(double rho, double theta, double total_mass, double pos,
                      double upper_area, Vec const& velocity, Vec const& eng);

double improve_theta(std::string const& name_f, double theta, double pos,
                            double orbital_h, std::streampos& file_pos);

bool is_orbiting(double pos, double velocity);

//forse dovremmo togliere la forza centripeta non è davvero fisica
double centripetal(double total_mass, double altitude, double x_vel);

Vec const g_force(double altitude, double mass);

double Cd_from_Mach(double M);

Vec const drag(double rho, double altitude, double theta,
                      double upper_area, Vec const& velocity);


                      
};  // namespace rocket
#endif