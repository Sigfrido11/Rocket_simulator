#include "rocket.h"

namespace rocket {

 //   class Rocket 

  
  
  /*
  creare funzioni che restituiscano e che modifichino i valori con parametri di
  input, creare funzione rilascio stadio che cambia il valore della massa totale
  togliendo la massa dello stadio e che cambi anche la spinta.
  */

  // convenzione le funzioni membro avranno la lettera maisucola per
  // distinguerle dalle altre se vi sembra una cavolata ditelo
  double const Rocket::drag_coefficient() const { return drag_coefficient_; }

  double const Rocket::rocket_area() const { return rocket_area_; }

  double const Rocket::mass() const { return total_mass_; }

  simulation::Vec const& Rocket::velocity() const { return velocity_; }

  simulation::Vec& Rocket::velocity() { return velocity_; }

  void Rocket::improve_theta() const {
    // algoritmo per migliorare l'angolo di ascesa
  }

  double const Rocket::altitude() const { return altitude_; }

  void Rocket::move(double time) const {
    position_[0] = velocity_[0] * time;
    position_[1] = velocity_[1] * time;
  }

  double const Rocket::thrust(double time) {
    ;
    // somma spinta solida e liquida se presente deve infine verificare che vi
    // sia ancora carburante nello stadio nel caso non ci fosse chiama la
    // funzione Stage_release

    /*
     ATTENZIONE la spinta del carburante solido sarà presente solo nella prima
     parte del lancio e scomparirà al distacco del primo stadio. ATTENZIONE il
     propellente solido brucia costantemente alla stessa velocità quindi si
     esaurirà dopo un certo tempo che si può calcolare facilemnte io lo
     sconsiglio perchè poi il carburante bruciato va sottratto comunque alla
     massa per sapere la spinta. in quello liquido bisgonerà ricordarsi che in
     ongi istante la sua massa deve essere modificata
     */
    if ((mass_solid_propellant- velocità espulsione * time )<0){
    stage_release();
    }
  }
  void Rocket::stage_release() {
    total_mass -= it_stage_mass;
    stage_number = -1;
    mass_liquid_contaier.erase(mass_liquid_container.end())
    // la funzione cambia la massa del razzo sottraendo quella dello stadio e
    // anche la spinta, per aggevolare la cosa è stato inizializzato un
    // iteratore basterà spostare la sua posizione controllando prima se esce o
    // meno dall range del vector

    //    cambia anche l'area probabilmente
  }

  
};

}  // namespace rocket

//spinta diff, massa diff, stage release: to do
