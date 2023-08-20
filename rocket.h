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
using vectorD = std::vector<double>;
class Rocket {
  // tutte le informazioni
  std::string name_;
  double theta_{0.};  // angolo inclinazione
  double upper_drag_{0.48};
  double lateral_drag_{0.82};
  double orbital_h_{};

  /* sarebbe difficile far cambiare tali valori in base all'angolo senza
  compiere esperimenti diretti quindi per semplicità assumiamo che rimangano
  costanti ma che cambi solo l'area esposta, dimmi se ti va bene anche se non
  saprei che altro fare */

  double total_mass_;
  Vec velocity_{0., 0.};
  double mass_solid_cont_{0.};
  double mass_solid_prop_{0.};
  vectorD mass_liq_prop_;  // massa carburante liquida
  vectorD mass_liq_cont_;
  Vec pos_{0., 0.};  // ora altitude diventa inutile ho già tutto nel pos_
  double delta_altitude_{0.};
  double solid_thrust_{0};  // solo una volta se presente il carbutante solido
  vectorD stage_thrust_;
  double lateral_area_{0.};
  double upper_area_{0.};
  int total_stage_{1};  // numero degli stadi
  int current_stage_{
      1};  // numero degli stadi nel mentre per eliminare gli iteratori
 public:
  // costruttore con stadi
  // costruttore non con container
  explicit Rocket(double x, double Up_Ar, double Lat_Ar, double spm, double st,
                  int num_stage, vectorD lpm, vectorD lt)
      : total_mass_{x + ((std::accumulate(lpm.begin(), lpm.end(), 0.) + spm) *
                         19. / 18.)},  // x in realtà solo navicella
        lateral_area_{Lat_Ar},
        upper_area_{Up_Ar},
        mass_solid_prop_{spm},
        mass_solid_cont_{st},
        total_stage_{num_stage},
        current_stage_{num_stage},
        mass_liq_prop_{lpm},
        stage_thrust_{lt} {}

  // costruttore con container
  explicit Rocket(double x, double Up_Ar, double Lat_Ar, double spm, double scm,
                  double st, int num_stage, vectorD lpm,
                  std::vector<double> lcm, vectorD lt)
      : total_mass_{x + std::accumulate(lpm.begin(), lpm.end(), 0.) +
                    std::accumulate(lcm.begin(), lcm.end(), 0.) + scm +
                    spm},  // x in realtà solo navicella
        lateral_area_{Lat_Ar},
        upper_area_{Up_Ar},
        mass_solid_prop_{spm},
        mass_solid_cont_{scm},
        solid_thrust_{st},
        total_stage_{num_stage},
        current_stage_{num_stage},
        mass_liq_prop_{lpm},
        mass_liq_cont_{lcm},
        stage_thrust_{lt} {}

  Vec const drag(double) const;

  void delta_altitude(double);

  double get_delta_altitude() const;

  Vec get_velocity() const;

  Vec get_pos() const;

  void mass_lost(double, double);

  void improve_theta(std::string);

  double get_theta() const;

  double get_altitude() const;

  void move(double time, Vec);

  double get_mass() const;

  void stage_release();  // solo il distacco dello stadio

  void change_vel(Vec force, double time);

  Vec const move(Vec position, double time);

  void set_state(std::string);

  void saturnV(){};

  void sojuz(){};

  Vec const total_force(double, double) const;

class Engine {
  // per ogni stadio del razzo vi sarà una diversa spinta,
  // questo vettore contiene le spinte relative ad ogni stadio
  // ho tolto gli iteratori per snellire le informazioni del razzo visto che
  // dobbiamo copiarlo spesso
  int isp_{275};
  double nozzle_as_{0.};
  double t_0_{0.};
  double v_0_{0.};
  double p_0_{0.};
  double burn_a_{0.};
  double molecular_weight_{0.};
  double grain_dim_{0.};
  double grain_rho_{0.};
  double a_coef_{0.};
  double r_coef_{0.};
  double n_coef_{0.};
  double delta_pres_{0.};

 public:
  Vec const Vel_eq() const;

  void int_pression(double);

  void r_coef();

  Vec const v_eq() const;

  Vec const v_exit(double) const;

  Vec const eng_force(double) const;

  double const delta_m(double, double) const;

  void set_state(double,double);
};
};

Vec const centripetal(double, double, double);

double const opt_velocity(double);

double const opt_aceleration(double);

bool const Is_orbiting();

Vec const g_force(double,double,double);

};  // namespace rocket
#endif
