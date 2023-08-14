#ifndef ROCKET_H
#define ROCKET_H

#include <algorithm>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include "assert.h"

namespace rocket {
using Vec = std::array<double, 2>;
using vectorD = std::vector<double>;  
class Rocket {
  // tutte le informazioni
  std::string name_;
  double theta_{0.};  // angolo inclinazione
  double upper_drag_{0.48}; 
  double lateral_drag_{0.82};

  /* sarebbe difficile far cambiare tali valori in base all'angolo senza compiere esperimenti diretti
  quindi per semplicità assumiamo che rimangano costanti ma che cambi solo l'area esposta, dimmi se ti va bene
  anche se non saprei che altro fare */

  double total_mass_;
  double lateral_area_;
  double upper_area_;
  int total_stage_{1};  // numero degli stadi
  int current_stage_{1};  // numero degli stadi nel mentre per eliminare gli iteratori
  double solid_thrust_{0};  // solo una volta se presente il carbutante solido
  double mass_solid_container_;
  double mass_solid_propellant_;
  vectorD mass_liquid_propellant_;  // massa carburante liquida
  vectorD mass_liquid_container_;
  vectorD
      stage_thrust_;  // per ogni stadio del razzo vi sarà una diversa spinta,
  // questo vettore contiene le spinte relative ad ogni stadio
  // ho tolto gli iteratori per snellire le informazioni del razzo visto che
  // dobbiamo copiarlo spesso
  int isp_solid{275};
  int isp_liquid;
  Vec velocity_{0., 0.};
  Vec pos_{0., 0.}; // ora altitude diventa inutile ho già tutto nel pos_
  double delta_altitude_{0.};

 public:
  // costruttore con stadi
  // costruttore non con container
  Rocket(double x, double Up_Ar, double Lat_Ar, double spm, double st, int num_stage, vectorD lpm,
         vectorD lt)
      : total_mass_{x + ((std::accumulate(lpm.begin(), lpm.end(), 0.) + spm) *
                         19. / 18.)},  // x in realtà solo navicella
        lateral_area_{Lat_Ar}, upper_area_{Up_Ar},
        mass_solid_propellant_{spm},
        mass_solid_container_{st},
        total_stage_{num_stage},
        current_stage_{num_stage},
        mass_liquid_propellant_{lpm},
        stage_thrust_{lt} {}

  // costruttore con container
  Rocket(double x, double Up_Ar, double Lat_Ar, double spm, double scm, double st, int num_stage,
         vectorD lpm, std::vector<double> lcm,
         vectorD lt)
      : total_mass_{x + std::accumulate(lpm.begin(), lpm.end(), 0.) +
                    std::accumulate(lcm.begin(), lcm.end(), 0.) + scm +
                    spm},  // x in realtà solo navicella
        lateral_area_{Lat_Ar}, upper_area_{Up_Ar},
        mass_solid_propellant_{spm},
        mass_solid_container_{scm},
        solid_thrust_{st},
        total_stage_{num_stage},
        current_stage_{num_stage},
        mass_liquid_propellant_{lpm},
        mass_liquid_container_{lcm},
        stage_thrust_{lt} {}

  /*
  create vari costruttori in base ai dettagli che inseriscono
  le posizioni non dovranno mai essere inizializzate e saranno prese di default
  = 0 mentre la y del centro di massa verrà calcolata, quindi in sostanza
  secondo me bisognerebbe inizializzare solo la massa, il numero di stadi, la
  spinta dei razzi e la rispettiva massa, se viene specificato un solo valore
  per le informazioni relative allo stadio si assumeranno uguali per tutti gli
  stadi
  */

  /*
  creare funzioni che restituiscano e che modifichino i valori con parametri di
  input, creare funzione rilascio stadio che cambia il valore della massa totale
  togliendo la massa dello stadio e che cambi anche la spinta.
  */

  // convenzione le funzioni membro avranno la lettera maisucola per
  // distinguerle dalle altre se vi sembra una cavolata ditelo
  Vec const drag(double) const;

  const double rocket_area() const;

  const double get_mass() const;

  void delta_altitude(double);

  double const get_delta_altitude() const;

  Vec get_velocity() const;

  Vec get_pos() const;

  void mass_lose();

  void set_velocity(Vec);

  void set_pos(Vec);

  void improve_theta();

  double const get_altitude() const;

  void move(double time);

  Vec const thrust(double time);

  void stage_release();  // solo il distacco dello stadio

  void change_vel(Vec force, double time);

  Vec const move(Vec position, double time);

  Vec const Drag_force() const;

  Vec const G_force() const;
  // è solo funzione. Utile se voglio trasportare stato??

  bool Is_orbiting() const;

  double g_energy(rocket::Rocket &);
};

  // costruttori di preset

  void saturnV(){

  };

  void sojuz(){

  };
};
}  // namespace rocket
#endif
