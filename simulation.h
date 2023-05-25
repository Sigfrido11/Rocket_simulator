#include "simulation.h"

namespace simulation {

double Vec::get_x() {
  return vec_[0];
}
double Vec::get_y() {
  return vec_[1];
}

Vec Vec::operator+(Vec vec2) {
  vec_[0] = vec_[0] + vec2[0];
  vec_[1] = vec_ [1] + vec2[1];
  return *this;
}

Vec Vec::operator*(double a){
  vec_[0] = vec_[0] * a;
  vec_[1] = vec_ [1] * a;
}

const double Atmosphere::Air_temperature(rocket::Rocket const& rocket) {
    const double Ro{6.3568e6};
    double H = Ro * rocket.altitude() / (Ro + rocket.altitude());
    if (H <= 11000) {
      return {288.15 + 0.0065 * H};
    } else if ((H > 11000) && (H <= 20000)) {
      return 216.65;
    } else if ((H > 20000) && (H <= 32000)) {
      return {216.65 + 0.001 * (H - 20000)};
    } else if ((H > 32000) && (H <= 47000)) {
      return {228.65 + 0.0028 * (H - 32000)};
    } else if ((H > 47000) && (H <= 51000)) {
      return 270.65;
    }
    // trova le formule per H > 51000 m, cioè per z > 51412 m
    // calcola la temperatura dell'aria, per ogni 100 metri la temperatura
    // dell'aria diminuisce di circa 0,36
  }

double Atmosphere::Air_density(rocket::Rocket const& rocket) {
    // Calcola la densità dell'aria a una determinata altitudine
    // tencincamente dovremmo saperla calcolare da soli ma guardate le formule
    // nel caso
}

const Vec Atmosphere::Drag_force(rocket::Rocket const& rocket) {
  if (rocket.altitude() > karman_line) {return {0.,0.};}
   
  double air_velocity_x = rocket.velocity().get_x();
  double air_velocity_y = rocket.velocity().get_y();
  double air_density_at_altitude = Air_density(rocket);
  double rocket_area = rocket.rocket_area();
    
  double drag_force_x = 0.5 * rocket.drag_coefficient() * rocket_area * 
    air_density_at_altitude * pow(air_velocity_x, 2);
  double drag_force_y = 0.5 * rocket.drag_coefficient() * rocket_area * 
    air_density_at_altitude * pow(air_velocity_y, 2);
  return {drag_force_x, drag_force_y};
}

double Gravity::G_force(rocket::Rocket const& rocket) {
  return (gravity_constant * earth_mass * rocket.mass()) / std::pow((rocket.altitude() + earth_radius), 2);
}

bool Gravity::Is_orbiting() const {
  // controlla la velocità del razzo sull'asse delle y e controlla se la forza
  // centripeta
  //  permette di avere l'orbita
  /*
  ATTENZIONE le orbite non sono mai perfettamente circolari in teoria ci
  dovrebbe essere un "afelio" = apoapice " ed un perielio" = periapice la
  situazione si potrebbe complicare notevolmente, vediamo con che accuratezza
  riusciamo a lavorare al progetto
  */
}

double Gravity::g_energy(rocket::Rocket const& rocket) {
    return - rocket.mass() * earth_mass * gravity_constant / (earth_radius + rocket.altitude()); 
    // qui era stato inserito
    // un quadrato, cosa non presente nella formula
    // l'ho pure spostato nella classe gravity per dare più senso
}

Vec Gravity::force(rocket::Rocket rocket, Atmosphere atm, Gravity gravity) {
  Vec drag = atm.Drag_force(rocket);
  Vec G = gravity.G_force(rocket);
  Vec thrust = rocket.thrust(time);
  return {drag + G + thrust};  
}

}
