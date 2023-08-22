#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

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
  std::vector<double> lcm = {20'000, 5'000};
  rocket::Rocket rocket{15'000., 0.5, 0.8, 50'000, 25'000, 2, lpc, lcm};
  sim::Air_var air;
  double i;
  double time{1};
  Vec force;
  Vec eng_force;
  double delta_ms{0.};
  double delta_ml{0.};
  rocket::Rocket::Engine a;
  std::cout << "a che altezza vuoi orbitare?";
  double orbital_h;
  std::cin >> orbital_h;
  while (i < 400) {
    rocket.set_state(file_name,orbital_h,delta_ms,delta_ml);
    bool is_orbiting =
           rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity()[1]);


    air.set_state(rocket.get_pos()[0]);
    
    rocket.mass_lost(0., delta_ms); // occhio qui devi mettere una durata, se usi il tempo così da problemi

    if (!is_orbiting) {
      eng_force = eng.eng_force(delta_ms, rocket.get_theta(), rocket.get_pos()[0]);
      
    }
    else {eng_force = {0.,0,};
    std::cout << "messo in orbita signori";}
    // assert(eng_force[0]>=0 && eng_force[1]>=0);
    force = rocket.total_force(air.rho_, eng_force, rocket.get_pos()[0]);
    rocket.change_vel(force, time);
//    assert(rocket.get_velocity()[0]>=0. && rocket.get_velocity()[0] >=0.);
    rocket.move(time, force);
   // assert(rocket.get_pos()[0]>=0. &&rocket.get_pos()[0] >=0.);
    output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                  << rocket.get_velocity()[0] << "  "
                  << rocket.get_velocity()[1] << "  " << force[0] << "  "
                  << force[1] << '\n';
    output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    i += 1;
  
}
}