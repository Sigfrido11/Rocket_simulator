#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
#include <iostream>
#include <fstream>

int main(){
  std::string file_name = "theta_data.txt"; 
  std::ofstream output_rocket("output_rocket.txt");
  assert(output_rocket.is_open());
  std::cout << "rocket output is ok" << '\n';
  std::ofstream output_air("output_air.txt");
  assert(output_air.is_open());
  std::cout << "also air output is ok" << '\n';
  output_rocket << "posizione z-y   velocitàz-y    forza z-x" << '\n';
  output_rocket << "temp    pres    rho" << '\n'; 
  using Vec = std::array<double, 2>;
  rocket::Rocket rocket;
  rocket::Rocket::Engine eng;
  sim::Air_var air;
  double time;
  Vec force;
  double time{0};
  while (time< 400){
    rocket.set_state(file_name);
    eng.set_state(time);
    air.set_state(rocket.get_pos()[0], rocket.get_delta_altitude());
    force = rocket.total_force(air.rho_, air.p_);
    rocket.change_vel(force, time);
    rocket.move(time);
    output_rocket << rocket.get_pos()[0] << "  " <<rocket.get_pos()[1] <<rocket.get_velocity()[0] << "  " <<rocket.get_velocity()[1] << "  " <<force[0] << "  " << force[1] << '\n';
    output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    time += 1;
  }
  
   
}