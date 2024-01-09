#ifndef ROCKET_H
#define ROCKET_H
#include "assert.h"
#include "simulation.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <vector>


namespace rocket {
using Vec = std::array<double, 2>;

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
    double isp_{250};  // per i solidi
    double cm_{4.};    // coefficiente perdita massa
    double p_0_{5e6};
    double burn_a_{200e-6};
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
  };

  class Ad_engine final : public Engine {
    double p_0_{5e6};
    double burn_a_{200e-6};
    double nozzle_as_{221.0e-6};
    double t_0_{1710.0};
    double grain_rho_{1873};
    double grain_dim_{0.02};
    double burn_rate_a_{0.01};
    double burn_rate_n_{0.02};
    double prop_mm_{178};
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
  };

class Rocket {
  // tutte le informazioni contenute in rocket
  // cose da inizializzare
  std::string name_{"my_rocket"};
  double upper_area_{80.};
  double m_sol_cont_{15'000};
  double m_sol_prop_{40'000.};
  std::vector<double> m_liq_prop_{15'000};  // massa carburante liquida
  std::vector<double> m_liq_cont_{40'000};

  /* sarebbe difficile far cambiare tali valori in base all'angolo senza
  compiere esperimenti diretti quindi per semplicità assumiamo che rimangano
  costanti ma che cambi solo l'area esposta, dimmi se ti va bene anche se non
  saprei che altro fare */

  // cose di default
  double total_mass_{135'000};
  int total_stage_{1};  // numero degli stadi
  Vec velocity_{0., 0.};
  Vec pos_{0., 0.};  // ora altitude diventa inutile ho già tutto nel pos_
  int current_stage_{1};
  double theta_{1.57079632};  // angolo inclinazione

  Engine* eng_;
  int n_sol_eng_{1};
  std::vector<int> n_liq_eng_;
 public:
  // costruttore con tutto
  explicit Rocket(std::string const& name, double mass_structure, double Up_Ar,
               double s_p_m, double m_s_cont, std::vector<double> const& l_p_m,
               std::vector<double> const& l_c_m, Engine* eng,
               int n_solid_eng, std::vector<int> const& n_liq_eng);

  Rocket() = default;

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

  void set_state(std::string const& file_name, double orbital_h, double time,
                       bool is_orbiting, std::streampos& file_pos);

  void stage_release(double delta_ms, double delta_ml);  // solo il distacco dello stadio

  void change_vel(double time, Vec const& force);

  Vec const thrust(double time, bool is_orbiting) const;
};

Vec const total_force(double rho, double theta, double total_mass, double pos,
                      double upper_area, Vec const& velocity, Vec const& eng);

double improve_theta(std::string const& name_f, double theta, double pos,
                            double orbital_h, std::streampos& file_pos);

bool is_orbiting(double pos, double velocity);

double centripetal(double total_mass, double altitude, double y_vel);

double g_force(double altitude, double mass);

Vec const drag(double rho, double altitude, double theta,
                      double upper_area, Vec const& velocity);

double anti_g_turn(double gra, double centrip, double theta, bool is_orbiting,
                   int stage);

};  // namespace rocket
#endif