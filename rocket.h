#ifndef ROCKET_H
#define ROCKET_H

#include <string>
#include <vector>
#include <algorithm>
#include "simulation.h"
#include "assert.h"
#include <iterator>

namespace rocket {
using vectorD = std::vector<double>;  // MODIFICARE TUTTO COSI PER ALLEGGERIRE
    class Rocket {
  // convenzione le variabili private terminano con _
  std::string name_;
  simulation::Vec pos { 0., 0. };
  double theta_{0.};  // indica l'inclinazione del razzo
                      // double
  //   y_cent_of_mass;  // la posizione del baricentro, mi sembra più complicata
  // ma se rimane tempo possiamo valutare le rotazioni
  // diversamente rispetto a quanto si crede è meglio avere la testa del razzo
  // pesante
  double drag_coefficient_{0.82};
  double total_mass_;
  double rocket_area_;
  int stage_number{1};  // numero degli stadi
  double mass_solid_propellant_;
  double mass_solid_container_;  // ogni stadio ha il contenitore dello stesso
                                 // peso??
  double solid_thrust_{0};
  // spinta carburante liquido il suo valore può cambiare durante il decollo
  /*  spinta carburante liquido il suo valore può cambiare durante il
  decollo il suo cambiamento servirà per ottenere il giusto compromesso fra
  spinta e resistenza aereodinamica e il suo valore deve essere sommato alla
  spinta di ogni stadio, nel caso non fosse presente verrà inizializzato a 0
  e nulla cambierà*/
  std::vector<double> mass_liquid_propellant_;  // massa carburante liquida
  std::vector<double>::iterator it_mass_propellant =
      mass_liquid_propellant_.begin();  // iteratore massa liquida
  std::vector<double> mass_liquid_container_;
  std::vector<double>::iterator it_mass_container_ =
      mass_solid_container_.begin();
  std::vector<double>
      stage_thrust_;  // per ogni stadio del razzo vi sarà una diversa spinta,
  // questo vettore contiene le spinte relative ad ogni stadio

  std::vector<double>::iterator it_stage_thrust = stage_thrust_.begin();
  std::vector<double>
      stage_mass_;  // questo vettore contiene le masse dei vari stadi
  std::vector<double>::iterator it_stage_mass = stage_mass_.begin();

  simulation::Vec velocity_ { 0., 0.};
  simulation::Vec position_{0., 0.};

double altitude_{};

 public:
  // costruttore con stadi
  // costruttore non con container
  Rocket(double x, double y, double spm, double st, int n,
         std::vector<double> lpm, std::vector<double> lt)
      : total_mass_{x + ((std::accumulate(lpm.begin(), lpm.end(), 0.) + spm) *
                         19. / 18.)},  // x in realtà solo navicella
        rocket_area_{y},
        mass_solid_propellant_{spm},
        mass_solid_container_{st},
        stage_number_{n},
        mass_liquid_propellant_{lpm},
        stage_thrust_{lt} {}
  // costruttore con container
  Rocket(double x, double y, double spm, double scm, double st, int n,
         std::vector<double> lpm, std::vector<double> lcm,
         std::vector<double> lt)
      : total_mass_{x + std::accumulate(lpm.begin(), lpm.end(), 0.) +
                    std::accumulate(lcm.begin(), lcm.end(), 0.) + scm +
                    spm},  // x in realtà solo navicella
        rocket_area_{y},
        mass_solid_propellant_{spm},
        mass_solid_container_{scm},
        solid_thrust_{st},
        stage_number_{n},
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
  const double drag_coefficient() const;

  const double rocket_area() const;

  const double mass() const;

  simulation::Vec const& velocity() const;

  simulation::Vec& velocity();

  void improve_theta() const;

  double const altitude() const;

  void move(double time) const;

  double const thrust(double time);
  void stage_release();

  // costruttori di preset

  void saturnV(){

  };

  void sojuz(){

  };
};

}  // namespace rocket

// spinta diff, massa diff, stage release: to do

#endif
