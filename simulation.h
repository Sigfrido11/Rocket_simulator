#ifndef SIMULATION_H
#define SIMULATION_H
#include <array>
#include <cmath>
#include "rocket.h"

namespace simulation {
// convenzione le classi hanno l'iniziale maiuscola
 class vec {
  std::array<double, 2> vec_;
 public:
  vec{double x, double y} : vec_[1] = x, vec_[2] = y {
// nulla da fare
  }

  double get_x() {return vec_[0];}
  double get_y() {return vec_[1];}

  vec operator+(vec vec2) {
    vec_[0] = vec_[0] + vec2[0];
    vec_[1] = vec_ [1] + vec2[1];
    return *this;
  }
  vec operator*(double a) {
    vec_[0] = vec_[0] * a;
    vec_[1] = vec_ [1] * a;
  }
}

class Atmosphere {
  double air_density_{1.225};
  inline static double constexpr temperature_{293.0};
  inline static constexpr int karman_line{
      100000};  // linea oltre la quale la resistenza
                // aereodinamica si può considerare nulla
  inline static constexpr double pressure_at_sea_level{101325};
  inline static constexpr double temperature_at_sea_level{288.15};

 public:
  // l'inizializzazione di default deve essere fatta con i valori al livello del
  // mare potremmo creare dei valori di default per le varie possibili
  // condizioni atmosferiche

  // dovranno essere create funzioni che restituiscono i vari valori, magari si
  // potrebbe valutare di definirne alcune statiche in modo da poter essere
  // usate sempre

  double const Air_temperature(Rocket const& rocket) {
    double const Ro{6.3568e6};
    double H = Ro * rocket.(rocket::altitude()) / (Ro + rocket.altitude());
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

  double Air_density(Rocket const& rocket) {
    // Calcola la densità dell'aria a una determinata altitudine
    // tencincamente dovremmo saperla calcolare da soli ma guardate le formule
    // nel caso
  }
  // verranno assunte trascurabili le differenze al livello atmosferico alla
  // base e alla sommità del razzo

  vec Drag_force(Rocket const const& rocket) {
    if (altitude > karman_line) {return 0.;}
    }

    double air_velocity_x = rocket.velocity().get_x();
    double air_velocity_y = rocket.velocity().get_y();
    double air_density_at_altitude = Air_density(rocket.altitude());
    double rocket_area = rocket.rocket_area();
    
    double drag_force_x = 0.5 * rocket.drag_coefficient_() * rocket_area * 
      air_density_at_altitude * pow(air_velocity_x, 2);
    double drag_force_y = 0.5 * rocket.drag_coefficient_() * rocket_area * 
      air_density_at_altitude * pow(air_velocity_y, 2);
    
    return {drag_force_x, drag_force_y};
  }
};
//
//????
class Gravity {
  inline static constexpr double gravity_constant{6.674e-11}; // costante gravitazionale
  inline static constexpr double earth_mass{5.97e24};  // massa della Terra
  inline static constexpr double earth_radius{6371e6};

 public:
  // creare funzioni che restituiscono i vari valori visto che sono costanti si
  // potrebbero definire alcuni di esso in modo statico
  double G_force(simulation::Rocket rocket) {
    return (gravity_constant * earh_mass * rocket.mass())/
           std::pow((rocket.altitude() + earth_radius), 2);
  } /// è solo funzione. Utile se voglio trasportare stato??
//???
//
  bool Is_orbiting() const {
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
  double g_energy(Rocket const& rocket) {
  return - rocket.mass() * earth_mass * gravity_constant / (earth_radius + rocket.altitude()); // qui era stato inserito
  // un quadrato, cosa non presente nella formula
  // l'ho pure spostato nella classe gravity per dare più senso
}
};

vec force(Rocket const& rocket, Atmosphere const& atm, Gravity const& gravity) {
  vec drag = atm.Drag_force(rocket);
  vec G = gravity.G_force(rocket);
  vec thrust = rocket.thrust(r);
  return drag + G + thrust;  
}

void accelerate(Rocket& rocket, vec force, double time) {
  rocket.velocity() = rocket.velocity() + ((force*(1/rocket.mass())*time);
  
}

void move(Rocket& rocket, double time) {
  rocket.pos()=rocket.pos()+(rocket.velocity()*time);
 }

//INCLINAZIONE

}  // namespace simulation

#endif