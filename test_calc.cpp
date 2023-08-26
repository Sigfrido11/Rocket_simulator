#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "simulation.h"
#include "rocket.h"
#include "doctest.h"
#include <memory>

TEST_CASE("costruttore rocket") {
std::string name{"my rocket"};
double mass_structure{12'000};
double Up_Ar{0.4};
double Lat_Ar{0.6};
double s_p_m{14'500}; 
double m_s_cont{20'000};
std::vector<double> l_p_m{32'000, 23'000};
std::vector<double> l_c_m{12'000, 13'000};
rocket::Rocket::Base_engine eng_b;
rocket::Rocket::Ad_engine eng_a1;
rocket::Rocket::Ad_engine eng_a2;
std::vector<rocket::Rocket::Engine> eng_l;
eng_l.push_back(eng_a1);
eng_l.push_back(eng_a2);
int n_solid_eng{2};
std::vector<int> n_liq_eng{2,3};
    
rocket::Rocket rocket_all {name, mass_structure, Up_Ar, Lat_Ar,
s_p_m, m_s_cont, l_p_m , l_c_m, eng_b, eng_l, n_solid_eng, n_liq_eng};

CHECK(rocket_all.get_lat_ar()== doctest::Approx(0.6));
CHECK(rocket_all.get_up_ar()== doctest::Approx(0.4));
CHECK(rocket_all.get_mass()== doctest::Approx(126500));  
rocket_all.mass_lost(300,1000);
CHECK(rocket_all.get_mass()== doctest::Approx(125200));

std::array<double, 2> thrust = rocket_all.thrust(100'000, 12.,false);
rocket_all.change_vel(12.,thrust );
rocket_all.move(12, thrust);
CHECK(rocket_all.get_velocity()== {3,5});
CHECK(rocket_all.get_pos()== {6,7});

CHECK(rocket_all.get_theta()== doctest::Approx(0.4));
rocket_all.mass_lost();
rocket_all.stage_release();




  void mass_lost(double, double);

  double const get_up_ar() const;

  double const get_lat_ar() const;

  double get_mass() const;

  void set_state(std::string, double, double, bool, std::streampos stream_pos);

  void stage_release(double, double);  // solo il distacco dello stadio



rocket::Rocket rocket_med {name, s_p_m, l_p_m,
                   eng_b,  eng_l, n_solid_eng, n_liq_eng};

  // costruttore senza container aree e carburanti
rocket::Rocket rocket_minim {name, eng_b, eng_l,n_solid_eng, n_liq_eng};

 
};

TEST_CASE("simulation") {
  sim::Air_var air;
  air.set_state(3'000);
  CHECK(air.p_== doctest::Approx(97'719) && air.rho_== doctest::Approx(1.26) && air.t_== doctest::Approx(268));
  air.set_state(18'000);
  CHECK(air.p_== doctest::Approx(7'564) && air.rho_== doctest::Approx(0.12) && air.t_== doctest::Approx(216.));
  air.set_state(25'000);
  CHECK(air.p_== doctest::Approx(2'505) && air.rho_== doctest::Approx(0.039) && air.t_== doctest::Approx(221));
  air.set_state(35'000);
  CHECK(air.p_== doctest::Approx(110.9) && air.rho_== doctest::Approx(1.6e-3) && air.t_== doctest::Approx(236));
  air.set_state(48'000);
  CHECK(air.p_== doctest::Approx(102) && air.rho_== doctest::Approx(1.31e-3) && air.t_== doctest::Approx(270));
  
}

TEST_CASE("main") {
  
};