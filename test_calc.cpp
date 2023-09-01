#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "doctest.h"
#include "rocket.h"
#include "simulation.h"

TEST_CASE("TESTING THE CALCS") {
  SUBCASE("Testing rocket") {
    std::string name{"my rocket"};
    double mass_structure{12'000};
    double Up_Ar{0.4};
    double Lat_Ar{0.6};
    double s_p_m{14'500};
    double m_s_cont{20'000};
    std::vector<double> l_p_m{320'000, 23'000};
    std::vector<double> l_c_m{12'000, 13'000};
    std::unique_ptr<rocket::Rocket::Engine> eng_b =
        std::make_unique<rocket::Rocket::Base_engine>(170., 1.5, 8e6, 220e-6);
    std::unique_ptr<rocket::Rocket::Engine> ad_1 =
        std::make_unique<rocket::Rocket::Ad_engine>(6e6, 420.e-2, 250.e-2,
                                                    2'800.);
    std::unique_ptr<rocket::Rocket::Engine> ad_2 =
        std::make_unique<rocket::Rocket::Ad_engine>(3e6, 400.e-4, 250.e-4,
                                                    2'800.);
    std::vector<std::unique_ptr<rocket::Rocket::Engine>> vec_liq;
    vec_liq.push_back(std::move(ad_1));
    vec_liq.push_back(std::move(ad_2));
    int n_solid_eng{1};
    std::vector<int> n_liq_eng{1, 1};

    rocket::Rocket rocket{name,  mass_structure, Up_Ar,       Lat_Ar,
                          s_p_m, m_s_cont,       l_p_m,       l_c_m,
                          eng_b, vec_liq,        n_solid_eng, n_liq_eng};

    rocket::Rocket::Ad_engine eng_1{6e6, 420.e-2, 250.e-5, 2'800.};
    rocket::Rocket::Ad_engine eng_2{3e6, 400.e-2, 250.e-4, 2'800.};
    double const pres1{eng_1.get_pression()};
    double const pres2{eng_2.get_pression()};
    CHECK(pres1 == doctest::Approx(6e6));
    CHECK(pres2 == doctest::Approx(3e6));
    CHECK(rocket.get_up_ar() == doctest::Approx(0.4));
    CHECK(rocket.get_mass() == doctest::Approx(414500));
    rocket.mass_lost(300, 1000);
    CHECK(rocket.get_mass() == doctest::Approx(413200));
    CHECK(rocket.get_fuel_left() == doctest::Approx(333200));
    CHECK(rocket.get_rem_stage() == 3);
    std::array<double, 2> thrust = rocket.thrust(1, false);
    CHECK(thrust[0] / 1e7 == doctest::Approx(3.11824));
    CHECK(thrust[1] == doctest::Approx(0.211881));
    rocket.move(2, thrust);
    rocket.change_vel(12., thrust);
    CHECK(rocket.get_velocity()[0] == doctest::Approx(905.588));
    CHECK(rocket.get_velocity()[1] * 1e6 == doctest::Approx(6.15338));
    CHECK(rocket.get_pos()[0] == doctest::Approx(150.931));
    CHECK(rocket.get_pos()[1] * 1e5 == doctest::Approx(0.102556));
    rocket.mass_lost(120., 320.);
    CHECK(rocket.get_mass() == 412760);
    CHECK(rocket.get_fuel_left() == 332760);
    rocket.mass_lost(1200., 30200.);
    CHECK(rocket.get_rem_stage() == 3);
    CHECK(rocket.get_mass() == 381360);
    CHECK(rocket.get_fuel_left() == 301360);
    rocket.set_state("theta_data.txt", 170'000, 0.01, false);
    CHECK(rocket.get_theta() == doctest::Approx(1.5708));
    CHECK(rocket.get_velocity()[0] == doctest::Approx(905.588));
    rocket.set_state("theta_data.txt", 170'000, 1, true);
    rocket.thrust(1, false);
    CHECK(thrust[0] / 1e7 == doctest::Approx(3.11824));
    CHECK(thrust[1] == doctest::Approx(0.211881));
    rocket.move(5, thrust);
    rocket.change_vel(5., thrust);
    CHECK(rocket.get_velocity()[0] == doctest::Approx(1316.59));
    CHECK(rocket.get_velocity()[1] * 1e6 == doctest::Approx(8.94612));
    CHECK(rocket.get_pos()[0] == doctest::Approx(5706.39));
    CHECK(rocket.get_pos()[1] * 1e5 == doctest::Approx(3.87743 ));
    CHECK(rocket.get_fuel_left() == doctest::Approx(299342));
    rocket.stage_release(400,200);
    CHECK(rocket.get_rem_stage() == 3);
    
  };

  SUBCASE("simulation") {
    sim::Air_var air;
    air.set_state(3'000);

    CHECK(air.p_ == doctest::Approx(99525.3));
    CHECK(air.rho_ == doctest::Approx(1.29051));
    CHECK(air.t_ == doctest::Approx(268.659));
    air.set_state(18'000);
    CHECK(air.p_ == doctest::Approx(7'564.4));
    CHECK(air.rho_ == doctest::Approx(0.121632));
    CHECK(air.t_ == doctest::Approx(216.65));
    air.set_state(25'000);
    CHECK(air.p_ == doctest::Approx(2549.1));
    CHECK(air.rho_ == doctest::Approx(0.0400812));
    CHECK(air.t_ == doctest::Approx(221.552));
    air.set_state(35'000);
    CHECK(air.p_ == doctest::Approx(574.53));
    CHECK(air.rho_ == doctest::Approx(0.00846222));
    CHECK(air.t_ == doctest::Approx(236.515));
    air.set_state(48'000);
    CHECK(air.p_ == doctest::Approx(102.276));
    CHECK(air.rho_ == doctest::Approx(1.31e-3));
    CHECK(air.t_ == doctest::Approx(270.65));
  }
  SUBCASE("testing is_orbiting") {
    bool is_orbiting = rocket::is_orbiting(50, 1'000);
    CHECK(is_orbiting == false);
    is_orbiting = rocket::is_orbiting(60'000, 8'000);
    CHECK(is_orbiting == true);
  };
  SUBCASE("centripetal") {
    double centripetal = rocket::centripetal(150'000, 40'000, 5'000);
    CHECK(centripetal == doctest::Approx(584294));
    centripetal = rocket::centripetal(150'000, 40'000, 3'000);
    CHECK(centripetal == doctest::Approx(210346));
  };
  SUBCASE("g_force") {
    double g_force = rocket::g_force(500, 14'000);
    CHECK(g_force == doctest::Approx(137170));
    g_force = rocket::g_force(3'400, 24'000.);
    CHECK(g_force == doctest::Approx(234935));
  };
  SUBCASE("drag") {
    std::array<double, 2> drag_f =
        rocket::drag(1, 1'000, 1.5, 10, 500, {800., 240.});
    CHECK((drag_f[0] / 1e6) == doctest::Approx(1.59599));
    CHECK((drag_f[1] / 1e6) == doctest::Approx(0.763962));
    drag_f = rocket::drag(0.8, 2'000, 1.5, 10, 500, {2800., 2040.});
    CHECK((drag_f[0] / 1e7) == doctest::Approx(1.56407));
    CHECK((drag_f[1] / 1e7) == doctest::Approx(4.4157));
    drag_f = rocket::drag(0.8, 53'000, 1.5, 10, 500, {2800., 2040.});
    CHECK(drag_f[0] == doctest::Approx(0.));
    CHECK(drag_f[1] == doctest::Approx(0.));
  }
  SUBCASE("improve_theta") {
    std::string file_name = "theta_data.txt";
    std::ifstream file(file_name);
    double thetha = rocket::improve_theta(file_name, 1.56, 57'000, 80'000);
    CHECK(thetha == doctest::Approx(0.182038));
    std::chrono::high_resolution_clock clock;
    auto start = clock.now();
    thetha = rocket::improve_theta(file_name, 1.56, 57'500, 80'000);
    auto dur1 = clock.now() - start;
    CHECK(thetha == doctest::Approx(0.178687));
    thetha = rocket::improve_theta(file_name, 1.56, 5'500, 80'000);
    CHECK(thetha == doctest::Approx(1.04009));
  }
  SUBCASE("Total force") {
    double rho{1.};
    double theta{1.};
    double tot_m{150'000};
    double altitude{5'00};
    double lat_a{100.};
    double up_a{800.};
    std::array<double, 2> vel{846, 150};
    std::array<double, 2> eng_f{100'000, 10'000};
    std::array<double, 2> tot = rocket::total_force(rho, theta, tot_m, altitude,
                                                    up_a, lat_a, vel, eng_f);
    double centrip{rocket::centripetal(tot_m, altitude, vel[1])};
    double gra{rocket::g_force(altitude, tot_m)};
    std::array<double, 2> drag_f =
        rocket::drag(rho, altitude, theta, up_a, lat_a, vel);
    double z = eng_f[0] + centrip - gra - drag_f[0];
    CHECK(tot[0] == z);
    CHECK(tot[1] == 0);
    rho = 0.;
    theta = 0.1;
    tot_m = 10'000;
    altitude = 75'00;
    lat_a = 100.;
    up_a = 800.;
    vel = {7846, 9150};
    eng_f = {0., 0.};
    tot = rocket::total_force(rho, theta, tot_m, altitude, up_a, lat_a, vel,
                              eng_f);
    centrip = rocket::centripetal(tot_m, altitude, vel[1]);
    gra = rocket::g_force(altitude, tot_m);
    drag_f = rocket::drag(rho, altitude, theta, up_a, lat_a, vel);
    z = eng_f[0] + centrip - gra - drag_f[0];
    double y{eng_f[1] - drag_f[1]};
    CHECK(tot[0] == z);
    CHECK(tot[1] == y);
    CHECK(tot[0] == doctest::Approx(33349.4));
    CHECK(tot[1] == doctest::Approx(0.));
  };

  SUBCASE("Base_engine function") {
    rocket::Rocket::Base_engine base_all{300., 3., 14e6, 200e-6};
    rocket::Rocket::Base_engine base_med{300., 3.2, 14e6};

    CHECK(base_all.is_ad_eng() == false);
    CHECK(base_med.is_ad_eng() == false);

    CHECK(base_all.is_released() == false);
    CHECK(base_med.is_released() == false);

    CHECK(base_all.delta_m(0.1, false) == doctest::Approx(840));
    CHECK(base_med.delta_m(0.1, false) == doctest::Approx(896.));

    CHECK(base_all.delta_m(0.1, true) == doctest::Approx(0.));
    CHECK(base_med.delta_m(0.1, true) == doctest::Approx(0.));

    double time{1};
    double theta{1.4};
    double pos{1000};
    std::vector<double> par{time, theta, pos};
    std::array<double, 2> force = base_all.eng_force(par, true);
    CHECK(force[0] == doctest::Approx(0.));
    CHECK(force[1] == doctest::Approx(0.));

    force = base_med.eng_force(par, true);
    CHECK(force[0] == doctest::Approx(0.));
    CHECK(force[1] == doctest::Approx(0.));

    force = base_all.eng_force(par, false);
    CHECK(force[0] / 1e7 == doctest::Approx(2.43276));
    CHECK(force[1] / 1e6 == doctest::Approx(4.19594));

    force = base_med.eng_force(par, false);
    CHECK(force[0] / 1e7 == doctest::Approx(2.59494));
    CHECK(force[1] / 1e6 == doctest::Approx(4.47567));

    base_all.release();
    base_med.release();

    CHECK(base_all.is_released() == true);
    CHECK(base_med.is_released() == true);
  };

  SUBCASE("Ad_engine function") {
    double burn_a{200e-2};
    double nozzle_as{220e-2};
    double t_0{3'000};
    double grain_dim{0.020};
    double grain_rho{2'700};
    double a_coef{50};
    double burn_rate_n{0.021};
    double p_0{2.1e6};
    double prop_mm{180};
    rocket::Rocket::Ad_engine ad_all{burn_a,      nozzle_as, t_0,
                                     grain_dim,   grain_rho, a_coef,
                                     burn_rate_n, prop_mm};
    rocket::Rocket::Ad_engine ad_med{p_0, burn_a, nozzle_as, t_0};

    CHECK(ad_all.get_pression() / 1e6 == doctest::Approx(2.93386));

    CHECK(ad_all.is_ad_eng() == true);
    CHECK(ad_med.is_ad_eng() == true);

    CHECK(ad_all.is_released() == false);
    CHECK(ad_med.is_released() == false);

    CHECK(ad_all.delta_m(0.1, false) == doctest::Approx(8324.65));
    CHECK(ad_med.delta_m(0.1, false) == doctest::Approx(5925.42));

    CHECK(ad_all.delta_m(0.1, true) == doctest::Approx(0.));
    CHECK(ad_med.delta_m(0.1, true) == doctest::Approx(0.));

    double theta{1.4};
    std::vector<double> par{theta};
    std::array<double, 2> force = ad_all.eng_force(par, true);
    CHECK(force[0] == doctest::Approx(0.));
    CHECK(force[1] == doctest::Approx(0.));

    force = ad_med.eng_force(par, true);
    CHECK(force[0] == doctest::Approx(0.));
    CHECK(force[1] == doctest::Approx(0.));

    force = ad_all.eng_force(par, false);
    CHECK(force[0] / 1e7 == doctest::Approx(1.1122));
    CHECK(force[1] / 1e6 == doctest::Approx(1.91828));

    force = ad_med.eng_force(par, false);
    CHECK(force[0] / 1e7 == doctest::Approx(0.784677));
    CHECK(force[1] / 1e6 == doctest::Approx(1.35339));

    theta = 1.;
    par[0] = theta;
    force = ad_all.eng_force(par, true);
    CHECK(force[0] == doctest::Approx(0.));
    CHECK(force[1] == doctest::Approx(0.));

    force = ad_med.eng_force(par, true);
    CHECK(force[0] == doctest::Approx(0.));
    CHECK(force[1] == doctest::Approx(0.));

    force = ad_all.eng_force(par, false);
    CHECK(force[0] / 1e7 == doctest::Approx(0.949702));
    CHECK(force[1] / 1e6 == doctest::Approx(6.09796));

    force = ad_med.eng_force(par, false);
    CHECK(force[0] / 1e7 == doctest::Approx(0.670032));
    CHECK(force[1] / 1e6 == doctest::Approx(4.30223));

    ad_all.release();
    ad_med.release();

    CHECK(ad_all.is_released() == true);
    CHECK(ad_med.is_released() == true);
  }
};