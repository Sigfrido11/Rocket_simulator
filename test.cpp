#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
#include <fstream>
#include <iostream>

int main() {
  using Vec = std::array<double, 2>;
  std::string file_name = "theta_data.txt";
  std::ofstream output_rocket("output_rocket.txt");
  assert(output_rocket.is_open());
  std::cout << "rocket output is ok" << '\n';
  std::ofstream output_air("output_air.txt");
  assert(output_air.is_open());
  std::cout << "also air output is ok" << '\n';
  output_rocket << "posizione z-y   velocitàz-y    forza z-x" << '\n';
  output_rocket << "temp    pres    rho" << '\n';
  std::vector<double> lpc = {10'000, 8'000};
  std::vector<double> lcm = {80'000, 35'000};
  rocket::Rocket rocket{10'000., 0.5, 0.8, 50'000, 25'000, 2, lpc, lcm};
  rocket::Rocket::Engine eng;
  sim::Air_var air;
  double time;
  Vec force;
  Vec eng_force;
  std::cout << "ora si entra nel ciclo" << "\n";
  while (time < 400) {
    rocket.set_state(file_name);
    std::cout << "il setting del razzo è andato bene" << 'n';
    bool is_orbiting =
        rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity()[1]);

    std::cout << "is orbiting andato bene" << is_orbiting <<'n' ;

    eng.set_state(time);
     std::cout << "il setting del motore è andato bene" << 'n';

    air.set_state(rocket.get_pos()[0], rocket.get_delta_altitude());
     std::cout << "il setting del aria è andato bene" << 'n';

    rocket.mass_lost(0., eng.delta_m(time, air.p_));
    std::cout << "la perdita di massa è andato bene ne rimane" <<rocket.get_mass() <<'n' ;
    if (!is_orbiting) {
      eng_force = eng.eng_force(air.p_, rocket.get_theta());
    } 
    else {eng_force = {0.,0,};}
    assert(eng_force[0]>=0 && eng_force[1]>=0);
    std::cout << "il calcolo della foza del motore è andato bene viene"<<eng_force[0] << " " << eng_force[1] << 'n';
    force = rocket.total_force(air.rho_, eng_force, rocket.get_pos()[0]);
    rocket.change_vel(force, time);
    assert(rocket.get_velocity()[0]>=0. && rocket.get_velocity()[0] >=0.);
    rocket.move(time, force);
    assert(rocket.get_pos()[0]>=0. &&rocket.get_pos()[0] >=0.);
    output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                  << rocket.get_velocity()[0] << "  "
                  << rocket.get_velocity()[1] << "  " << force[0] << "  "
                  << force[1] << '\n';
    output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    time += 1;
  }
}
