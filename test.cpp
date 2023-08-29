#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
#include <stdio.h>

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

  std::string name{"my rocket"};
  double mass_structure{12'000};
  double Up_Ar{0.4};
  double Lat_Ar{0.6};
  double s_p_m{44'500};
  double m_s_cont{20'000};
  std::vector<double> l_p_m{32'000, 50'000};
  std::vector<double> l_c_m{12'000, 13'000};

  int n_solid_eng{2};
  std::vector<int> n_liq_eng{2, 2};

  rocket::Rocket::Base_engine base{200., 1.5, 8e6, 220e-6};
  std::unique_ptr<rocket::Rocket::Engine> eng_b =
      std::make_unique<rocket::Rocket::Base_engine>(base);

  std::unique_ptr<rocket::Rocket::Engine> ad_1 =
      std::make_unique<rocket::Rocket::Ad_engine>(8.2e6, 420.e-5, 250.e-5,
                                                  2'800.);
  std::unique_ptr<rocket::Rocket::Engine> ad_2 =
      std::make_unique<rocket::Rocket::Ad_engine>(8.2e6, 420e-5, 250e-5, 2'800);
  std::vector<std::unique_ptr<rocket::Rocket::Engine>> vec_liq;
  vec_liq.push_back(std::move(ad_1));
  vec_liq.push_back(std::move(ad_2));
  rocket::Rocket rocket{name,  mass_structure, Up_Ar,       Lat_Ar,
                        s_p_m, m_s_cont,       l_p_m,       l_c_m,
                        eng_b, vec_liq,        n_solid_eng, n_liq_eng};
  sim::Air_var air;
  int i{0};
  double time{1};
  std::streampos start_pos;
  Vec eng_force;
  std::cout << "a che altezza vuoi orbitare? ";
  double orbital_h;
  std::cin >> orbital_h;
  while (i < 400) {
    bool orbiting =
        rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity()[1]);

    double const imp_thrust{
        rocket::improve_thrust(rocket.get_pos()[0], time, rocket.get_mass(),
                               orbital_h, eng_force, rocket.get_velocity())};

    rocket.set_state(file_name, orbital_h, imp_thrust, time, orbiting,
                     start_pos);

    air.set_state(rocket.get_pos()[0]);

    eng_force = rocket.thrust(air.p_, time, imp_thrust, orbiting);

    Vec force = rocket::total_force(air.rho_, rocket.get_theta(),
                                    rocket.get_mass(), rocket.get_pos()[0],
                                    rocket.get_up_ar(), rocket.get_lat_ar(),
                                    rocket.get_velocity(), eng_force);

    rocket.move(time, force);

    rocket.change_vel(time, force);

    assert(rocket.get_velocity()[0] >= 0. && rocket.get_velocity()[1] >= 0.);

    assert(rocket.get_pos()[0] >= 0. && rocket.get_pos()[1] >= 0.);
    output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                  << rocket.get_velocity()[0] << "  "
                  << rocket.get_velocity()[1] << "  " << force[0] << "  "
                  << force[1] << '\n';
    output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    i += 1;
  }
}
