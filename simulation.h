#ifndef SIMULATION_H
#define SIMULATION_H
#include <array>
#include <cmath>

#include "rocket.h"

namespace simulation {
// convenzione le classi hanno l'iniziale maiuscola
class Vec {
  std::array<double, 2> vec_;

 public:
  Vec(double x, double y)
      : vec_[1] = x,
        vec_[2] = y{
            // nulla da fare
        };

  double get_x();
  double get_y();

  Vec operator+(Vec);
  Vec operator*(double);
};

class Atmosphere {
  double air_density_{1.225};
  static double constexpr temperature_{293.0};
  static constexpr int karman_line{100000};
  // linea oltre la quale la resistenza
  // aereodinamica si può considerare nulla
  static constexpr double pressure_at_sea_level{101325};
  static constexpr double temperature_at_sea_level{288.15};

 public:
  // l'inizializzazione di default deve essere fatta con i valori al livello del
  // mare potremmo creare dei valori di default per le varie possibili
  // condizioni atmosferiche

  // dovranno essere create funzioni che restituiscono i vari valori, magari si
  // potrebbe valutare di definirne alcune statiche in modo da poter essere
  // usate sempre

  const double Air_temperature(rocket::Rocket const&);

  double Air_density(rocket::Rocket const&);
  // verranno assunte trascurabili le differenze al livello atmosferico alla
  // base e alla sommità del razzo

  const Vec Drag_force(rocket::Rocket const&);
};

//
//????
class Gravity {
  static constexpr double gravity_constant{
      6.674e-11};                               // costante gravitazionale
  static constexpr double earth_mass{5.97e24};  // massa della Terra
  static constexpr double earth_radius{6371e6};

 public:
  // creare funzioni che restituiscono i vari valori visto che sono costanti si
  // potrebbero definire alcuni di esso in modo statico
  double G_force(rocket::Rocket const&);
  // è solo funzione. Utile se voglio trasportare stato??
  //???
  //
  bool Is_orbiting() const;

  double g_energy(rocket::Rocket const& rocket);
};

Vec force(rocket::Rocket, Atmosphere, Gravity);

void accelerate(Rocket& rocket, vec force, double time) {
  rocket.velocity() = rocket.velocity() + ((force*(1/rocket.mass())*time);
}

void move(Rocket& rocket, double time) {
  rocket.pos() = rocket.pos() + (rocket.velocity() * time);
}

// INCLINAZIONE

}  // namespace simulation

#endif

