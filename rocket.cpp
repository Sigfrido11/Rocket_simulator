#include "rocket.h"

#include <cmath>

namespace rocket {

// convenzione le funzioni membro avranno la lettera maisucola per
// distinguerle dalle altre se vi sembra una cavolata ditelo
Vec const Rocket::drag(double rho) const {
  if (pos_[0] < 75'000) {
    double z =
        0.5 * rho * upper_area_ * std::cos(theta_) * std::pow(velocity_[0], 2);
    double y = 0.5 * rho * lateral_area_ * std::sin(theta_) *
               std::pow(velocity_[1], 2);
    return {z, y};
  } else {
    return {0., 0.};
  }
}

double const Rocket::get_mass() const { return total_mass_; }

void Rocket::improve_theta() {
  //  quelli riportati sono una stima di quello che si è osservato fare
  //  sperimentalmente
  // sono stati infatti osservate le telemetrie di 10 voli per ottenere valori
  // medi delle grandezze cercate la funzione ottimale per l'angolo in radianti
  // è ang= -0,352ln(z)+1.8533
  theta_ = -0, 352 * std::log(pos_[0]) + 1.8533;
}

Vec Rocket::get_velocity() const { return velocity_; }

Vec Rocket::get_pos() const { return pos_; }

void Rocket::set_pos(Vec position) { pos_ = position; }

void Rocket::set_velocity(Vec vel) { velocity_ = vel; }

double const Rocket::get_altitude() const { return pos_[0]; }

double const Rocket::get_delta_altitude() const { return delta_altitude_; }

void Rocket::move(double time) {
  double z = pos_[0] + velocity_[0] * time;
  delta_altitude_ = z - pos_[0];
  pos_[0] = z;
  pos_[1] = pos_[1] + velocity_[1] * time;
}

void rocket::change_vel(Vec force, double time) {
  velocity_[0] = velocity_[0] + (force[0] * (1 / total_mass_) * time);
  velocity_[1] = velocity_[1] + (force[1] * (1 / total_mass_) * time);
}

Vec const Rocket::thrust(double time) {
  if (mass_solid_container_ != 0) {
    

    double thrust = 
  }
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
  // controlla sia il rilascio del solido che quello del liquido
  // if ((mass_solid_propellant - velocità espulsione * time )<0){
  stage_release();
}

void Rocket::stage_release() {
  if (mass_solid_container_ == 0) {
    assert(current_stage_ != 0);
    current_stage_ -= 1;
    int rem_stage{total_stage_ - current_stage_};
    total_mass_ -=
        mass_liquid_container_[rem_stage] - mass_liquid_propellant_[rem_stage];
  } else {
    assert(current_stage_ != 0);
    current_stage_ -= 1;
    int rem_stage{total_stage_ - current_stage_};
    total_mass_ -= mass_solid_container_ - mass_solid_propellant_;
    mass_solid_container_ = 0;
  }
}

// la funzione cambia la massa del razzo sottraendo quella dello stadio e
// anche la spinta, per aggevolare la cosa è stato inizializzato un
// iteratore basterà spostare la sua posizione controllando prima se esce o
// meno dall range del vector

//    cambia anche l'area probabilmente
};  // namespace rocket
// namespace rocket
// spinta diff, massa diff, stage release: to do
