#ifndef ROCKET_H
#define ROCKET_H

#include <assert.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

namespace rocket {
using Vec = std::array<double, 2>;
class Rocket {
  // tutte le informazioni contenute in rocket
  // cose da inizializzare
  std::string name_{"my_rocket"};
  double lateral_area_{400.};
  double upper_area_{15.};
  double mass_solid_cont_{15'000};
  double mass_solid_prop_{40'000.};
  std::vector<double> mass_liq_prop_{15'000};  // massa carburante liquida
  std::vector<double> mass_liq_cont_{40'000};

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
  std::streampos start_pos_;
  double theta_{1.57079632};  // angolo inclinazione
  double upper_drag_{0.48};
  double lateral_drag_{0.82};

 public:
  class Engine {
    double isp_{250};  // per i solidi
    double cm_{4.};
    double p_0_{5e6};
    double burn_a_{200e-6};
    /*
    // per ogni stadio del razzo vi sarà una diversa spinta,
    // questo vettore contiene le spinte relative ad ogni stadio
    // ho tolto gli iteratori per snellire le informazioni del razzo visto che
    // dobbiamo copiarlo spesso
    double nozzle_as_{221.0e-6};
    double t_0_{1710.0};
    double v_0_{};
    double molecular_weight_{0.4242};
    double grain_dim_{0.02};
    double grain_rho_{1879};
    double a_coef_{0.01};
    double r_coef_{0.1};
    double n_coef_{0.02};
    double delta_pres_{0.};

    bool is_steady_{false};
    */

   public:
    explicit Engine(double isp, double cm, double p0, double burn_a)
        : isp_{isp}, cm_{cm}, p_0_{p0}, burn_a_{burn_a} {
      assert(isp_ > 0 && cm_ > 0 && p_0_ > 0 && burn_a_ > 0);
    }

    explicit Engine(double isp, double cm, double p0)
        : isp_{isp}, cm_{cm}, p_0_{p0} {
      assert(isp_ > 0 && cm_ > 0 && p_0_ > 0);
    }

    explicit Engine(double isp, double cm) : isp_{isp}, cm_{cm} {
      assert(isp_ > 0 && cm_ > 0);
    }

    Engine() = default;

    Rocket::Engine& operator=(Rocket::Engine const& other);

    Vec const eng_force(double, double, double) const;

    double const delta_m(double) const;

    Engine const solid_eng() const;

    Engine const liquid_eng() const;
    /*

        Vec const Vel_eq() const;

        void int_pression(double, double);

        void r_coef();

        Vec const v_eq() const;

        double const v_exit(double) const;

        void set_state(double,double);

        double const get_pression() const;
    */
  };

 private:
  Engine eng_s_ {Engine::solid_eng()};
  std::vector<Engine> liq_eng_{Engine::liquid_eng()};

 public:
  // costruttore con tutto
  explicit Rocket(std::string name, double mass_structure, double Up_Ar,
                  double Lat_Ar, double s_p_m, double m_s_cont,
                  std::vector<double> l_p_m, std::vector<double> l_c_m,
                  Engine eng_s, std::vector<Engine> eng_l)
      : total_mass_{mass_structure +
                    std::accumulate(l_p_m.begin(), l_p_m.end(), 0.) + s_p_m +
                    m_s_cont + std::accumulate(l_c_m.begin(), l_c_m.end(), 0.)},
        name_{name},
        lateral_area_{Lat_Ar},
        upper_area_{Up_Ar},
        mass_solid_prop_{s_p_m},
        mass_solid_cont_{m_s_cont},
        total_stage_{l_p_m.size()},
        mass_liq_prop_{l_p_m},
        mass_liq_cont_{l_c_m},
        eng_s{eng_s} {
    assert(mass_liq_cont_.size() == mass_liq_prop_.size());
    assert(lateral_area_ > 0 && upper_area_ > 0 && mass_solid_cont_ > 0 &&
           mass_solid_prop_ > 0);
    std::for_each(mass_liq_cont_.begin(), mass_liq_cont_.end(),
                  [=](double value) { assert(value > 0.); });
    std::for_each(mass_liq_prop_.begin(), mass_liq_prop_.end(),
                  [=](double value) { assert(value > 0.); });
    std::for_each(liq_eng_.begin(), liq_eng_.end(),
                  [&](std::vector<Engine> eng) { 
                    
                    eng =  });
  }

  // costruttore senza container aree e massa struttura

  explicit Rocket(std::string name, double s_p_m, std::vector<double> l_p_m,
                  Engine eng_s, std::vector<Engine> eng_l)
      : total_mass_{15'000 + std::accumulate(l_p_m.begin(), l_p_m.end(), 0.) +
                    s_p_m + 15'000 * (l_p_m.size() + 1)},
        name_{name},
        mass_solid_prop_{s_p_m},
        total_stage_{l_p_m.size()},
        mass_liq_prop_{l_p_m},
        eng_s = eng_s,
        eng_l = eng_l {
    assert(mass_solid_prop_ > 0 && mass_liq_prop_ > 0);

    std::for_each(mass_liq_prop_.begin(), mass_liq_prop_.end(),
                  [=](double value) { assert(value > 0.); });

    std::for_each(mass_liq_cont_.begin(), mass_liq_cont_.end(),
                  [=](double value) { value = 15'000; });
    mass_solid_cont_{15'000};
  }

  // costruttore senza container aree e carburanti

  explicit Rocket(std::string name, int stage_number, Engine eng_s,
                  std::vector<Engine> eng_l)
      : total_mass_{15'000 + 20'000 * (stage_number + 1) +
                    40'000 * (stage_number + 1)},
        name_{name},
        total_stage_{stage_number},
        eng_s = eng_s,
        eng_l = eng_l {
    assert(mass_liq_cont_.size() == mass_liq_prop_.size());
    assert(lateral_area_ > 0 && upper_area_ > 0 && mass_solid_cont_ > 0 &&
           mass_solid_prop_ > 0);
    std::for_each(mass_liq_prop_.begin(), mass_liq_prop_.end(),
                  [=](double value) { assert(value > 0.); });

    std::for_each(mass_liq_cont_.begin(), mass_liq_cont_.end(),
                  [=](double value) { value = 15'000; });
    std::for_each(mass_liq_prop_.begin(), mass_liq_prop_.end(),
                  [=](double value) { value = 40'000; });
              
  }

  Rocket() = default;

  Vec const drag(double) const;

  Vec get_velocity() const;

  Vec get_pos() const;

  void mass_lost(double, double);

  void improve_theta(std::string, double);

  double get_theta() const;

  void move(double time, Vec);

  double get_mass() const;

  void stage_release(double, double);  // solo il distacco dello stadio

  void change_vel(Vec force, double time);

  void set_state(std::string, double, double, double);

  void saturnV(){};

  void sojuz(){};

  Vec const total_force(double, Vec, double) const;
};

bool const is_orbiting(double, double);

Vec const centripetal(double, double, double);

// double const opt_velocity(double);

// double const opt_aceleration(double);

Vec const g_force(double, double, double);

};  // namespace rocket
#endif
