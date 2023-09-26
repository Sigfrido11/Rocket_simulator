#include "interface.h"
#include "rocket.h"
#include "simulation.h"

#include <stdio.h>
#include <fstream>
#include <iostream>

int main() {
  rocket::Rocket::Ad_engine ad_eng;
  rocket::Rocket::Base_engine base_eng;
  interface::rocket_data rocket_data;
  std::unique_ptr<rocket::Rocket::Engine> eng;
  std::vector<double> l_p_m;
  std::vector<double> l_c_m;
  std::vector<int> n_liq_eng;

  char ans_eng;

  {  // aggiungo uno scope in modo da cancellare poi tutte le
    // variabili che non servono

    interface::ad_eng_data ad_eng_data;
    interface::base_eng_data base_eng_data;
    std::cout
        << "let's build the engine for the solid propellant of your rocket"
        << "\n"
        << "do you prefer an advance (a) rocket or base rocket (b)"
        << "\n";
    std::cin >> ans_eng;
    switch (ans_eng) {
      case 'a':
        rocket_data.eng_type = 'a';
        std::cout << rocket_data.eng_type << '\n';
        interface::select_ad_eng(ad_eng_data);
        if (ad_eng_data.type == 'm') {
          ad_eng = rocket::Rocket::Ad_engine{
              ad_eng_data.burn_a,      ad_eng_data.nozzle_as,
              ad_eng_data.t_0,         ad_eng_data.grain_dim,
              ad_eng_data.grain_rho,   ad_eng_data.a_coef,
              ad_eng_data.burn_rate_n, ad_eng_data.prop_mm};
        } else {
          ad_eng =
              rocket::Rocket::Ad_engine{ad_eng_data.p_0, ad_eng_data.burn_a,
                                        ad_eng_data.nozzle_as, ad_eng_data.t_0};
        }
        break;
      case 'b':
        rocket_data.eng_type = 'a';
        interface::select_base_eng(base_eng_data);

        base_eng =
            rocket::Rocket::Base_engine{base_eng_data.isp, base_eng_data.cm,
                                        base_eng_data.p0, base_eng_data.burn_a};
        break;

      default:
        std::cout << "invalid value for rocket please restart ";
        assert(false);
        break;
    }
    if (ans_eng == 'a') {
      eng = std::make_unique<rocket::Rocket::Ad_engine>(ad_eng);
    } else {
      eng = std::make_unique<rocket::Rocket::Base_engine>(base_eng);
    }
    char ans_roc;
    std::cout << "how many details do you want to insert to create your"
              << "\n"
              << "rocket (many (m), some (s), few (f))?"
              << "\n";
    std::cin >> ans_roc;
    switch (ans_roc) {
      int ans;
      case 'm':
        rocket_data = interface::create_complete_roc();
        l_p_m.resize(rocket_data.stage_num);
        l_c_m.resize(rocket_data.stage_num);
        n_liq_eng.resize(rocket_data.stage_num);
        std::fill_n(l_p_m.begin(), rocket_data.stage_num, rocket_data.s_p_m);
        std::fill_n(l_c_m.begin(), rocket_data.stage_num, rocket_data.m_s_cont);
        for (int i{0}; i < rocket_data.stage_num; i++) {
          std::cout << "how many engines does the liquid propellant have for "
                       "stage "
                    << i << "\n";
          std::cin >> ans;
          n_liq_eng.push_back(ans);
        }
        break;
      case 's':
        rocket_data = interface::create_med_roc();
        l_p_m.resize(rocket_data.stage_num);
        l_c_m.resize(rocket_data.stage_num);
        n_liq_eng.resize(rocket_data.stage_num);
        std::cout
            << "how many engine does the liquid propellant have for each stage "
               "(strcly less than solid): < 3"
            << "\n";
        std::cin >> ans;
        std::fill_n(l_p_m.begin(), rocket_data.stage_num, rocket_data.s_p_m);
        std::fill_n(l_c_m.begin(), rocket_data.stage_num, 15'000);
        std::fill_n(n_liq_eng.begin(), rocket_data.stage_num, ans);
        break;

      case 'f':
        rocket_data = interface::create_minim_roc();
        l_p_m.resize(rocket_data.stage_num);
        l_c_m.resize(rocket_data.stage_num);
        n_liq_eng.resize(rocket_data.stage_num);
        std::cout
            << "how many engine does the liquid propellant have for each stage "
               "(strcly less than solid): < 3"
            << "\n";
        std::cin >> ans;
        std::fill_n(l_p_m.begin(), rocket_data.stage_num,
                    rocket_data.s_p_m);  // mettilo da inizializzare
        std::fill_n(l_c_m.begin(), rocket_data.stage_num, 15'000);
        std::fill_n(n_liq_eng.begin(), rocket_data.stage_num, ans);
        break;

      default:
        std::cout << "invalid value for rocket please restart";
        assert(false);
        break;
    }
  }

  using Vec = std::array<double, 2>;
  std::string file_name = "theta_data.txt";
  std::ofstream output_rocket("output_rocket.txt");
  assert(output_rocket.is_open());
  std::cout << "rocket output is ok" << '\n';
  std::ofstream output_air("output_air.txt");
  assert(output_air.is_open());
  std::cout << "also air output is ok" << '\n';
  output_rocket << "posizione z-y   velocità z-y    forza z-x" << '\n';
  output_air << "temp    pres    rho" << '\n';

  rocket::Rocket rocket{rocket_data.name,
                        rocket_data.mass_structure,
                        rocket_data.up_ar,
                        rocket_data.s_p_m,
                        rocket_data.m_s_cont,
                        l_p_m,
                        l_c_m,
                        eng,
                        rocket_data.n_solid_eng,
                        n_liq_eng};
  sim::Air_var air;
  std::cout << "at which altitude do you want to orbit? (m)"
            << "\n";
  double orbital_h;
  std::cin >> orbital_h;

  Vec eng_force;
  std::streampos file_pos;
  double delta_time{1};
  // game loop iniziass
  try {
    while (true) {
      bool const orbiting{
          rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity()[1])};

      rocket.set_state(file_name, orbital_h, delta_time, orbiting, file_pos);

      air.set_state(rocket.get_pos()[0]);

      eng_force = rocket.thrust(delta_time, orbiting);

      Vec const force{rocket::total_force(
          air.rho_, rocket.get_theta(), rocket.get_mass(), rocket.get_pos()[0],
          rocket.get_up_ar(), rocket.get_velocity(), eng_force)};

      rocket.move(delta_time, force);

      rocket.change_vel(delta_time, force);

      // assert(rocket.get_velocity()[0] >= 0. && rocket.get_velocity()[1] >=
      // 0.);

      assert(rocket.get_pos()[0] >= 0. && rocket.get_pos()[1] >= 0.);
      output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                    << rocket.get_velocity()[0] << "  "
                    << rocket.get_velocity()[1] << "  " << force[0] << "  "
                    << force[1] << '\n';
      output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    }
  }

catch (std::runtime_error const& e) {
  std::cerr << e.what() << '\n';
};
}

