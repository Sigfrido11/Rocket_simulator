#ifndef SIMULATION_H
#define SIMULATION_H

namespace simulation {
// convenzione le classi hanno l'iniziale maiuscola

class Atmosphere {
  double air_density_{1.225};
  inline static double temperature_{293.0};
  inline static constexpr int karman_line{100000}; // linea oltre la quale la resistenza
                                 // aereodinamica si può considerare nulla
  inline static constexpr double pressure_at_sea_level{101325};
  inline static constexpr double temperature_at_sea_level{288.15};
  double altitude;

public:
  // l'inizializzazione di default deve essere fatta con i valori al livello del
  // mare potremmo creare dei valori di default per le varie possibili
  // condizioni atmosferiche

  // dovranno essere create funzioni che restituiscono i vari valori, magari si
  // potrebbe valutare di definirne alcune statiche in modo da poter essere
  // usate sempre

  double const Air_temperature(double altitude) {
    // calcola la temperatura dell'aria, per ogni 100 metri la temperatura
    // dell'aria diminuisce di circa 0,36
  }

  double Air_density(double altitude) {
    // Calcola la densità dell'aria a una determinata altitudine
    // tencincamente dovremmo saperla calcolare da soli ma guardate le formule
    // nel caso
  }
  // verranno assunte trascurabili le differenze al livello atmosferico alla
  // base e alla sommità del razzo

  double Drag_force(double altitude, double velocity) {
    // Calcola la forza di resistenza dell'aria a una determinata altitudine e
    // velocità
    /*
    double air_velocity = velocity;
    double air_density_at_altitude = air_density(altitude);
    double drag_force = 0.5 * DRAG_COEFFICIENT * ROCKET_AREA *
                        air_density_at_altitude * pow(air_velocity, 2);
    return drag_force;*/
  }

};

class Gravity {
  double altitude{0};
  inline static constexpr double gravity_constant{6.674e-11}; // costante gravitazionale
  inline static constexpr double earh_mass{5.97e24};          // massa della Terra
  inline static constexpr double earh_radius{6371e6};

public:
  // creare funzioni che restituiscono i vari valori visto che sono costanti si
  // potrebbero definire alcuni di esso in modo statico
  double Gravitational_force(double altitude) {
    // Calcola la forza gravitazionale a una determinata altitudine
  }
  bool Is_orbiting () const {
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
};
} // namespace simulation


#endif