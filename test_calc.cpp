#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <memory>
#include<array>
#include "doctest.h"
#include "rocket.h"
#include "simulation.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include<chrono>

TEST_CASE("TESTING THE CALCS") {

  SUBCASE("Testing rocket"){
  std::string name{"my rocket"};
  double mass_structure{12'000};
  double Up_Ar{0.4};
  double Lat_Ar{0.6};
  double s_p_m{14'500};
  double m_s_cont{20'000};
  std::vector<double> l_p_m{32'000, 23'000};
  std::vector<double> l_c_m{12'000, 13'000};
  std::unique_ptr<rocket::Rocket::Engine> eng_b =
      std::make_unique<rocket::Rocket::Base_engine>(0.1, 0.4);
  std::unique_ptr<rocket::Rocket::Engine> ad_1 =
      std::make_unique<rocket::Rocket::Ad_engine>(0.1, 0.4);
  std::unique_ptr<rocket::Rocket::Engine> ad_2 =
      std::make_unique<rocket::Rocket::Ad_engine>(0.1, 0.4);
  std::vector<std::unique_ptr<rocket::Rocket::Engine>> vec_liq;
  vec_liq.push_back(std::move(ad_1));
  vec_liq.push_back(std::move(ad_2));
  int n_solid_eng{1};
  std::vector<int> n_liq_eng{1};

  rocket::Rocket rocket_all{name,  mass_structure, Up_Ar,       Lat_Ar,
                            s_p_m, m_s_cont,       l_p_m,       l_c_m,
                            eng_b, vec_liq,        n_solid_eng, n_liq_eng};

  CHECK(rocket_all.get_lat_ar() == doctest::Approx(0.6));
  CHECK(rocket_all.get_up_ar() == doctest::Approx(0.4));
  CHECK(rocket_all.get_mass() == doctest::Approx(126500));
  rocket_all.mass_lost(300, 1000);

  CHECK(rocket_all.get_mass() == doctest::Approx(125200));
  CHECK(rocket_all.get_rem_fuel() == doctest::Approx(1));

  std::array<double, 2> thrust = rocket_all.thrust(100'000, 12., false);
  rocket_all.change_vel(12., thrust);
  rocket_all.move(12, thrust);
  CHECK(rocket_all.get_velocity()[0] == 3);
  CHECK(rocket_all.get_velocity()[1] == 5);

  CHECK(rocket_all.get_pos()[0] == 3);
  CHECK(rocket_all.get_pos()[1] == 5);
  rocket_all.mass_lost(1200., 3200.);
  rocket_all.stage_release(100, 200);
  CHECK(rocket_all.get_rem_stage() == 1);
  CHECK(rocket_all.get_mass() == doctest::Approx(1));
  CHECK(rocket_all.get_rem_fuel() == 1);

  rocket::Rocket rocket_med{name,    s_p_m,       l_p_m,    eng_b,
                            vec_liq, n_solid_eng, n_liq_eng};
  // costruttore senza container aree e carburanti
  rocket::Rocket rocket_minim{name, eng_b, vec_liq, n_solid_eng, n_liq_eng};
};

SUBCASE("simulation") {
  sim::Air_var air;
  air.set_state(3'000);

  CHECK(air.p_ == doctest::Approx(97'719));
  CHECK(air.rho_ == doctest::Approx(1.26));
  CHECK(air.t_ == doctest::Approx(268));
  air.set_state(18'000);
  CHECK(air.p_ == doctest::Approx(7'564));
  CHECK(air.rho_ == doctest::Approx(0.12));
  CHECK(air.t_ == doctest::Approx(216.));
  air.set_state(25'000);
  CHECK(air.p_ == doctest::Approx(2'505));
  CHECK(air.rho_ == doctest::Approx(0.039));
  CHECK(air.t_ == doctest::Approx(221));
  air.set_state(35'000);
  CHECK(air.p_ == doctest::Approx(110.9));
  CHECK(air.rho_ == doctest::Approx(1.6e-3));
  CHECK (air.t_ == doctest::Approx(236));
  air.set_state(48'000);
  CHECK(air.p_ == doctest::Approx(102));
  CHECK( air.rho_ == doctest::Approx(1.31e-3));
  CHECK(air.t_ == doctest::Approx(270));
}
SUBCASE("testing is_orbiting"){
bool is_orbiting = rocket::is_orbiting(50, 1'000);
CHECK(is_orbiting == false);
is_orbiting = rocket::is_orbiting(60'000, 8'000);
CHECK(is_orbiting == true);
};
SUBCASE("centripetal"){
std::array<double,2> centripetal = rocket::centripetal(150'000,40'000, 5'000);
CHECK(centripetal[0] == doctest::Approx(5.95e5));
CHECK(centripetal[1] == doctest::Approx(0.));
centripetal = rocket::centripetal(150'000,40'000, 3'000);
CHECK(centripetal[0] == doctest::Approx(2.14e5));
CHECK(centripetal[1] == doctest::Approx(0.));
};
SUBCASE("g_force"){

std::array<double,2> g_force = rocket::g_force(500,14'000,1.4);
CHECK(g_force[0] == doctest::Approx(10.));
CHECK(g_force[1] == doctest::Approx(10.));
g_force = rocket::g_force(3'400,24'000,1.);
CHECK(g_force[0] == doctest::Approx(10.));
CHECK(g_force[1] == doctest::Approx(10.));

};
SUBCASE("drag"){
std::array<double,2> drag_f = rocket::drag(1,1'000,1.5,10,500,{800.,240.});
CHECK(drag_f[0] == doctest::Approx(10.));
CHECK(drag_f[1] == doctest::Approx(10.));
drag_f = rocket::drag(0.8,2'000,1.5,10,500,{2800.,2040.});
CHECK(drag_f[0] == doctest::Approx(10.));
CHECK(drag_f[1] == doctest::Approx(10.));
drag_f = rocket::drag(0.8,53'000,1.5,10,500,{2800.,2040.});
CHECK(drag_f[0] == doctest::Approx(0.));
CHECK(drag_f[1] == doctest::Approx(0.));
}
SUBCASE ("improve_theta"){
std::string file_name = "theta_data.txt";
std::ifstream file(file_name);
std::streampos pos1 {file.tellg()};
std::streampos pos2 {file.tellg()};
double thetha = rocket::improve_theta(file_name, 7'000, 80'000, pos1);
CHECK(thetha == doctest::Approx(0.));
std::chrono::high_resolution_clock clock;
auto start = clock.now();
thetha = rocket::improve_theta(file_name, 7'500, 80'000, pos1);
auto dur1 = clock.now() - start;
CHECK(thetha == doctest::Approx(0.));
start = clock.now();
thetha = rocket::improve_theta(file_name, 7'500, 80'000, pos2);
auto dur2 = clock.now() - start;
CHECK(thetha == doctest::Approx(0.));
CHECK(dur1 < dur2);
}
SUBCASE("Total force"){
double rho{1.};
double theta{1.};
double tot_m{150'000};
double altitude{5'00};
double time{0.1};
double lat_a{100.};
double up_a{800.};
bool is_orbiting{false};
std::array<double,2> vel {846,150};
std::array<double,2> eng_f {100'000,10'000};
std::array<double,2> tot = rocket::total_force(rho,theta,tot_m,altitude,time,up_a,lat_a,is_orbiting,vel,eng_f);
std::array<double,2> centrip = rocket::centripetal(tot_m, altitude, vel[1]);
std::array<double,2> gra = rocket::g_force(altitude, tot_m, theta);
std::array<double,2> drag_f = rocket::drag(rho, altitude, theta, up_a, lat_a, vel);
double z = eng_f[0] - gra[0] - drag_f[0];
double y = eng_f[1] + centrip[1] - gra[1] - drag_f[1];
CHECK(tot[0] == z);
CHECK(tot[1] == y);

rho=0.;
theta=0.1;
tot_m=10'000;
altitude=75'00;
time=0.1;
lat_a=100.;
up_a=800.;
is_orbiting=true;
vel = {7846,9150};
eng_f = {0.,0.};
tot = rocket::total_force(rho,theta,tot_m,altitude,time,up_a,lat_a,is_orbiting,vel,eng_f);
centrip = rocket::centripetal(tot_m, altitude, vel[1]);
gra = rocket::g_force(altitude, tot_m, theta);
drag_f = rocket::drag(rho, altitude, theta, up_a, lat_a, vel);
z = eng_f[0] - gra[0] - drag_f[0];
y = eng_f[1] + centrip[1] - gra[1] - drag_f[1];
CHECK(tot[0] == z);
CHECK(tot[1] == y);
CHECK(tot[0] == doctest::Approx(0.));
CHECK(tot[1] == doctest::Approx(0.));

};

SUBCASE("Base_engine function"){

rocket::Rocket::Base_engine base_all {300.,3.,14e6,200e-6};
rocket::Rocket::Base_engine base_med {300.,3.,14e6};
rocket::Rocket::Base_engine base_minim {300.,3.};

CHECK(base_all.is_ad_eng()==false);
CHECK(base_med.is_ad_eng()==false);
CHECK(base_minim.is_ad_eng()==false);

CHECK(base_all.is_released()==false);
CHECK(base_med.is_released()==false);
CHECK(base_minim.is_released()==false);

CHECK(base_all.delta_m(0.1, false)==doctest::Approx(0.));
CHECK(base_med.delta_m(0.1, false)==doctest::Approx(0.));
CHECK(base_minim.delta_m(0.1, false)==doctest::Approx(0.));

CHECK(base_all.delta_m(0.1, true)==doctest::Approx(0.));
CHECK(base_med.delta_m(0.1, true)==doctest::Approx(0.));
CHECK(base_minim.delta_m(0.1, true)==doctest::Approx(0.));

double time{0.1};
double theta{1.4};
double pos{1000};
std::vector<double> par {time,theta,pos};
std::array<double,2> force= base_all.eng_force( par ,true);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= base_med.eng_force( par ,true);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= base_minim.eng_force( par ,true);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= base_all.eng_force( par ,false);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= base_med.eng_force( par ,false);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= base_minim.eng_force( par ,false);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

base_all.release();
base_med.release();
base_minim.release();

CHECK(base_all.is_released()==true);
CHECK(base_med.is_released()==true);
CHECK(base_minim.is_released()==true);
};

SUBCASE("Ad_engine function"){
double burn_a{200e-6};
double nozzle_as{220e-6};
double t_0{3'000};
double grain_dim{0.020};
double grain_rho{1'700};
double a_coef{2.1};
double burn_rate_n{2.1};
double p_0{2.1};
rocket::Rocket::Ad_engine ad_all{burn_a,nozzle_as,t_0,grain_dim,grain_rho,a_coef,burn_rate_n};
rocket::Rocket::Ad_engine ad_med{p_0, burn_a, nozzle_as,t_0};
rocket::Rocket::Ad_engine ad_minim {p_0, t_0};

CHECK(ad_all.is_ad_eng()==true);
CHECK(ad_med.is_ad_eng()==true);
CHECK(ad_minim.is_ad_eng()==true);

CHECK(ad_all.is_released()==false);
CHECK(ad_med.is_released()==false);
CHECK(ad_minim.is_released()==false);

CHECK(ad_all.delta_m(0.1, false)==doctest::Approx(0.));
CHECK(ad_med.delta_m(0.1, false)==doctest::Approx(0.));
CHECK(ad_minim.delta_m(0.1, false)==doctest::Approx(0.));

CHECK(ad_all.delta_m(0.1, true)==doctest::Approx(0.));
CHECK(ad_med.delta_m(0.1, true)==doctest::Approx(0.));
CHECK(ad_minim.delta_m(0.1, true)==doctest::Approx(0.));

CHECK(ad_all.get_pression()==doctest::Approx(0.));

double p_ext{80'000};
double theta{1.4};
std::vector<double> par {p_ext,theta};
std::array<double,2> force= ad_all.eng_force(par ,true);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= ad_med.eng_force( par ,true);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= ad_minim.eng_force( par ,true);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= ad_all.eng_force( par ,false);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= ad_med.eng_force( par ,false);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

force= ad_minim.eng_force( par ,false);
CHECK(force[0]==doctest::Approx(0.));
CHECK(force[1]==doctest::Approx(0.));

ad_all.release();
ad_med.release();
ad_minim.release();

CHECK(ad_all.is_released()==true);
CHECK(ad_med.is_released()==true);
CHECK(ad_minim.is_released()==true);
}
};