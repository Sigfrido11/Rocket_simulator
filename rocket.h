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

namespace rocket {
using Vec = std::array<double, 2>;
class Rocket {
  // tutte le informazioni contenute in rocket
  // cose da inizializzare
  std::string name_{"my_rocket"};
  double lateral_area_{400.};
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
  double upper_drag_{0.48};
  double lateral_drag_{0.82};

 public:
  class Engine {
   public:
    virtual double delta_m(double, bool) const = 0;

    virtual Vec const eng_force(std::vector<double>, bool) const = 0;

    virtual void release() = 0;

    virtual void set_spin(double) = 0;

    virtual bool is_ad_eng() const = 0;

    virtual bool is_released() const = 0;

    virtual ~Engine() = default;
  };

  class Base_engine final : public Engine {
    double isp_{250};  // per i solidi
    double cm_{4.};    // coefficiente perdita massa
    double p_0_{5e6};
    double burn_a_{200e-6};
    double spin_coef_;
    bool released_{false};

   public:
    explicit Base_engine(double isp, double cm, double p0, double burn_a);

    explicit Base_engine(double isp, double cm, double p0)
        : isp_{isp}, cm_{cm}, p_0_{p0} {
      assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0);
    }

    explicit Base_engine(double isp, double cm) : isp_{isp}, cm_{cm} {
      assert(isp_ >= 0 && cm_ >= 0);
    }

    Base_engine() = default;

    double delta_m(double, bool) const override;

    void release() override;

    Vec const eng_force(std::vector<double>, bool) const override;

    virtual bool is_ad_eng() const override;

    virtual bool is_released() const override;

    virtual void set_spin(double) override;
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
    double prop_mm_ {178};
    double spin_coef_;
    bool released_{false};

   public:
    explicit Ad_engine(double burn_a, double nozzle_as, double t_0,
                       double grain_dim, double grain_rho, double a_coef,
                       double burn_rate_n, double prop_mm);

    explicit Ad_engine(double p_0, double burn_a, double nozzle_as, double t_0);

    explicit Ad_engine() = default;

    double delta_m(double, bool) const override;

    void release() override;

    virtual void set_spin(double) override;

    Vec const eng_force(std::vector<double>, bool) const override;

    virtual bool is_ad_eng() const override;

    virtual bool is_released() const override;

    double get_pression() const;
  };

 private:
  std::unique_ptr<Engine> eng_s_ = std::make_unique<Base_engine>(250, 3.5);
  int n_sol_eng_{1};
  std::vector<std::unique_ptr<Engine>> liq_eng_;
  std::vector<int> n_liq_eng_;

 public:
  // costruttore con tutto

  explicit Rocket(std::string name, double mass_structure, double Up_Ar,
                  double Lat_Ar, double s_p_m, double m_s_cont,
                  std::vector<double> l_p_m, std::vector<double> l_c_m,
                  std::unique_ptr<Engine>& eng_s,
                  std::vector<std::unique_ptr<Engine>>& eng_l, int n_solid_eng,
                  std::vector<int> n_liq_eng);

  // costruttore senza container aree e massa struttura

  Rocket() = default;

  Vec const get_velocity() const;

  Vec const get_pos() const;

  void mass_lost(double const, double const);

  double get_theta() const;

  void move(double, Vec);

  double get_up_ar() const;

  int get_rem_stage() const;

  double get_rem_fuel() const;

  double get_lat_ar() const;

  double get_mass() const;

  void set_state(std::string, double, double, double, bool, std::streampos stream_pos);

  void stage_release(double, double);  // solo il distacco dello stadio

  void change_vel(double, Vec);

  Vec const thrust(double, double, double, bool) const;
};

Vec const total_force(double, double, double, double, double, double, Vec, Vec);

double improve_theta(std::string, double, double, std::streampos);

bool is_orbiting(double, double);

Vec const centripetal(double, double, double);

double g_force(double, double);

Vec const drag(double, double, double, double, double, Vec);

double improve_thrust(double, double, double, double, Vec,  Vec);

};  // namespace rocket
#endif
