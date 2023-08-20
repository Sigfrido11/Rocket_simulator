#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
#include <iostream>
#include <fstream>

int main(){
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
  std::vector<double> lpc = {10'000,8'000};
  std::vector<double> lcm = {80'000,35'000};
  rocket::Rocket rocket{10'000., 0.5,0.8, 50'000, 25'000,2,lpc,lcm};
  rocket::Rocket::Engine eng;
  sim::Air_var air;
  double time;
  Vec force;
  while (time < 400){
    rocket.set_state(file_name);
    eng.set_state(time);
    air.set_state(rocket.get_pos()[0], rocket.get_delta_altitude());
    rocket.mass_lost(0., eng.delta_m(time, air.p_));
    force = rocket.total_force(air.rho_, air.p_, eng.eng_force(air.p_,rocket.get_theta()));
    rocket.change_vel(force, time);
    rocket.move(time, force);
    output_rocket << rocket.get_pos()[0] << "  " <<rocket.get_pos()[1] <<rocket.get_velocity()[0] << "  " <<rocket.get_velocity()[1] << "  " <<force[0] << "  " << force[1] << '\n';
    output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    time += 1;
  }
  
   
}