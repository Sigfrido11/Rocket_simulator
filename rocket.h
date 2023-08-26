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
  double upper_area_{15.};
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
    virtual double const delta_m(double, bool) const = 0;

    virtual Vec const eng_force(std::vector<double>, bool) const = 0;

    virtual void release() = 0;

    virtual bool const is_ad_eng() const = 0;

    virtual bool const is_released() const = 0;

    virtual ~Engine() =default;
  };

  class Base_engine final : public Engine {
    double isp_{250};  // per i solidi
    double cm_{4.};
    double p_0_{5e6};
    double burn_a_{200e-6};
    bool released_{false};

   public:
    explicit Base_engine(double isp, double cm, double p0, double burn_a)
        : isp_{isp}, cm_{cm}, p_0_{p0}, burn_a_{burn_a} {
      assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0 && burn_a_ >= 0);
    }

    explicit Base_engine(double isp, double cm, double p0)
        : isp_{isp}, cm_{cm}, p_0_{p0} {
      assert(isp_ >= 0 && cm_ >= 0 && p_0_ >= 0);
    }

    explicit Base_engine(double isp, double cm) : isp_{isp}, cm_{cm} {
      assert(isp_ >= 0 && cm_ >= 0);
    }

    Base_engine() = default;

    double const delta_m(double, bool) const override;

    void release() override;

    Vec const eng_force(std::vector<double>, bool) const override;

    virtual bool const is_ad_eng() const override;

    virtual bool const is_released() const override;
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
    bool released_{false};

   public:
    explicit Ad_engine(double burn_a, double nozzle_as, double t_0,
                       double grain_dim, double grain_rho, double a_coef,
                       double burn_rate_n)
        : burn_a_{burn_a},
          t_0_{t_0},
          grain_dim_{grain_dim},
          grain_rho_{grain_rho},
          burn_rate_a_{a_coef},
          burn_rate_n_{burn_rate_n} {
          assert(p_0_ >= 0 && burn_a_ >= 0 && nozzle_as_ >= 0 && t_0_ >= 0 &&
             grain_dim_ >= 0 && grain_rho_ >= 0 && burn_rate_a_ >= 0 &&
               burn_rate_n_ >= 0);
      double fac1 = burn_rate_a_ * grain_rho_ * grain_dim_ / nozzle_as_;
      double exponent = sim::cost::gamma_ + 1 / (sim::cost::gamma_ - 1);
      double fac2 = std::pow(2 / sim::cost::gamma_ + 1, exponent);
      double fac3 =
          std::sqrt((sim::cost::gamma_ * fac2) / sim::cost::R_ * t_0_);
      p_0_ = std::pow(fac1 / fac3, 1 / (1 - burn_rate_n_));
    }

    explicit Ad_engine(double p_0, double burn_a, double nozzle_as, double t_0)
        : burn_a_{burn_a}, p_0_{p_0}, t_0_{t_0}, nozzle_as_{nozzle_as} {
          assert(p_0_ >= 0 && burn_a_ >= 0 && nozzle_as_ >= 0 && t_0_ >= 0);
    }

    explicit Ad_engine(double p_0, double t_0) : p_0_{p_0}, t_0_{t_0} {
          assert(p_0_ >= 0 && t_0_ >= 0);
    }

    explicit Ad_engine() = default;

    double const delta_m(double, bool) const override;

    void release() override;

    Vec const eng_force(std::vector<double>, bool) const override;

    virtual bool const is_ad_eng() const override;

    virtual bool const is_released() const override;
  };

 private:
  std::unique_ptr<Engine> eng_s_ =nullptr;
  int n_sol_eng_{1};Vec const Rocket::thrust(double theta, double pos, double p_ext, double time,
                         bool is_orbiting) const {
  Vec engs;
  Vec engl;
  if (eng_s_->is_ad_eng()) {
    std::vector<double> par{p_ext, theta};
    Vec engs = eng_s_->eng_force(par, is_orbiting);
  } else {
    std::vector<double> par{time, theta, pos};
    Vec engs = eng_s_->eng_force(par, is_orbiting);
  }
  if (liq_eng_[0].is_ad_eng()) {
    std::vector<double> par{p_ext, theta};
    Vec engl = liq_eng_[0].eng_force(par, is_orbiting);
  } else {
    std::vector<double> par{time, theta, pos};
    Vec engl = liq_eng_[0].eng_force(par, is_orbiting);
  }
  double z = engs[0] * n_sol_eng_ + engl[0] * n_liq_eng_[0];
  double y = engs[0] * n_sol_eng_+ engl[0] * n_liq_eng_[0];
  return {z, y};
}

  std::vector<Engine> liq_eng_;
  std::vector<int> n_liq_eng_;

 public:
  // costruttore con tutto

  explicit Rocket(std::string name, double mass_structure, double Up_Ar,
                  double Lat_Ar, double s_p_m, double m_s_cont,
                  std::vector<double> l_p_m, std::vector<double> l_c_m,
                  Engine& eng_s, std::vector<Engine> eng_l,
                  int n_solid_eng, std::vector<int> n_liq_eng)
      : total_mass_{mass_structure +
                    std::accumulate(l_p_m.begin(), l_p_m.end(), 0.) + s_p_m +
                    m_s_cont + std::accumulate(l_c_m.begin(), l_c_m.end(), 0.)},
        name_{name},
        lateral_area_{Lat_Ar},
        upper_area_{Up_Ar},
        m_sol_prop_{s_p_m},
        m_sol_cont_{m_s_cont},
        total_stage_{static_cast<int>(l_p_m.size())},
        m_liq_prop_{l_p_m},
        m_liq_cont_{l_c_m},
        liq_eng_{eng_l},
        n_sol_eng_{n_solid_eng},
        n_liq_eng_{n_liq_eng} {
    assert(m_liq_cont_.size() == m_liq_prop_.size() &&
           n_liq_eng_.size() == m_liq_prop_.size());
    assert(lateral_area_ > 0 && upper_area_ > 0 && m_sol_cont_ > 0 &&
          m_sol_prop_ > 0);
          
    std::for_each(n_liq_eng.begin(), n_liq_eng.end(),
                  [](int value) { assert(value >= 0); });
    std::for_each(m_liq_cont_.begin(), m_liq_cont_.end(),
                      [&](double value) mutable { assert(value > 0.); });
    std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                  [&](double value) mutable { assert(value > 0.); });
    
    *eng_s_ = eng_s;
 }

  // costruttore senza container aree e massa struttura

  explicit Rocket(std::string name, double s_p_m, std::vector<double> l_p_m,
                  Engine& eng_s, std::vector<Engine> eng_l,
                  int n_solid_eng, std::vector<int> n_liq_eng)
      : total_mass_{15'000 + std::accumulate(l_p_m.begin(), l_p_m.end(), 0.) +
                    s_p_m + 15'000 * (l_p_m.size() + 1)},
        name_{name},
        m_sol_prop_{s_p_m},
        total_stage_{static_cast<int>(l_p_m.size())},
        m_liq_prop_{l_p_m},
        liq_eng_{eng_l},
        n_sol_eng_{n_solid_eng},
        n_liq_eng_{n_liq_eng} {
    assert(m_sol_prop_ >= 0.);
    assert(m_liq_cont_.size() == n_liq_eng_.size() &&
           n_liq_eng_.size() == liq_eng_.size());
    std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                  [](double value) { assert(value > 0.); });

    std::for_each(n_liq_eng.begin(), n_liq_eng.end(),
                  [](int value) { assert(value >= 0); });

    std::for_each(m_liq_cont_.begin(), m_liq_cont_.end(),
                      [&](double value) mutable { value = 15'000; });
    *eng_s_ = eng_s;
  }

  // costruttore senza container aree e carburanti

  explicit Rocket(std::string name, Engine& eng_s,
                  std::vector<Engine> eng_l, int n_solid_eng,
                  std::vector<int> n_liq_eng)
      : name_{name},
        liq_eng_{eng_l},
        n_sol_eng_{n_solid_eng},
        n_liq_eng_{n_liq_eng} {
    assert(n_liq_eng_.size() == liq_eng_.size());
    std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                  [](double value) { assert(value > 0.); });
    std::for_each(m_liq_cont_.begin(), m_liq_cont_.end(),
                  [&](double value) mutable { value = 15'000; });
    std::for_each(m_liq_prop_.begin(), m_liq_prop_.end(),
                  [&](double value) mutable { value = 40'000; });
    std::for_each(n_liq_eng.begin(), n_liq_eng.end(), [](int value) {
      assert(value >= 0);});
   *eng_s_ = eng_s;
    total_stage_ = static_cast<int>(eng_l.size());
    total_mass_ =
        15'000 + 20'000 * (total_stage_ + 1) + 40'000 * (total_stage_ + 1);
  }

  Rocket() = default;

  Vec const get_velocity() const;

  Vec const get_pos() const;

  void mass_lost(double, double);

  double const get_theta() const;

  void move(double, Vec);

  double const get_up_ar() const;

  double const get_lat_ar() const;

  double const get_mass() const;

  void set_state(std::string, double, double, bool, std::streampos stream_pos);

  void stage_release(double, double);  // solo il distacco dello stadio

  void change_vel(double,Vec);

  Vec const thrust(double,double,bool) const;
};


Vec const total_force(double, double, double,double, double, double, bool, Vec, Vec);

double improve_theta(std::string,double, double, std::streampos);

bool const is_orbiting(double, double);

Vec const centripetal(double, double, double);

Vec const g_force(double, double, double);

Vec const drag(double, double, double, double, double, Vec);

};  // namespace rocket
#endif
