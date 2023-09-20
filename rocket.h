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

 public:
  class Engine {
   public:
    virtual double delta_m(double, bool) const = 0;

    virtual Vec const eng_force(std::vector<double>, bool) const = 0;

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
    double spin_coef_{1};
    bool released_{false};

   public:
    explicit Base_engine(double, double, double, double);

    explicit Base_engine(double isp, double cm, double p0)
        : isp_{isp}, cm_{cm}, p_0_{p0} {
      assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0);
    }

    Base_engine() = default;

    double delta_m(double, bool) const override;

    void release() override;

    Vec const eng_force(std::vector<double>, bool) const override;

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
    double spin_coef_{1};
    bool released_{false};

   public:
    explicit Ad_engine(double, double, double, double, double, double, double,
                       double);

    explicit Ad_engine(double p_0, double burn_a, double nozzle_as, double t_0);

    explicit Ad_engine() = default;

    double delta_m(double, bool) const override;

    void release() override;

    Vec const eng_force(std::vector<double>, bool) const override;

    virtual bool is_ad_eng() const override;

    virtual bool is_released() const override;

    virtual double get_pression() const override;
  };

 private:
  Engine* eng_;
  int n_sol_eng_{1};
  std::vector<int> n_liq_eng_;

 public:
  // costruttore con tutto

  explicit Rocket(std::string, double, double, double, double, double,
                  std::vector<double>, std::vector<double>,
                  Engine*, int, std::vector<int>);

  Rocket() = default;

  Vec const get_velocity() const;

  Vec const get_pos() const;

  void mass_lost(double const, double const);

  double get_theta() const;

  void move(double, Vec);

  double get_up_ar() const;

  int get_rem_stage() const;

  double get_fuel_left() const;

  double get_lat_ar() const;

  double get_mass() const;

  void set_state(std::string, double, double, bool);

  void stage_release(double, double);  // solo il distacco dello stadio

  void change_vel(double, Vec);

  Vec const thrust(double, bool) const;
};

Vec const total_force(double, double, double, double, double, double, Vec, Vec);

double improve_theta(std::string, double, double, double);

bool is_orbiting(double, double);

double centripetal(double, double, double);

double g_force(double, double);

Vec const drag(double, double, double, double, double, Vec);

double anti_g_turn(double gra, double centrip, double theta, bool is_orbiting,
                   int stage);

};  // namespace rocket
#endif
