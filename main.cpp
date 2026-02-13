#include <fstream>
#include <iostream>


#include <stdio.h>
#include "interface.h"
#include "rocket.h"
#include "simulation.h"
#include "vector_math.h"

//
// asse x componente [1]
// asse y componente [0]
//

int main() {
  
  interface::rocket_data rocket_data;
  rocket::Ad_engine ad_eng;
  rocket::Base_engine base_eng;
  rocket::Engine *eng;
  std::vector<double> l_p_m;
  std::vector<double> l_c_m;
  std::vector<int> n_liq_eng;
  char ans_eng;
  try {
  {  // aggiungo uno scope in modo da cancellare poi tutte le
    // variabili che non servono
    interface::ad_eng_data ad_eng_data;
    interface::base_eng_data base_eng_data;
    std::cout << "let's build the engine for your rocket"
              << "\n"
              << "do you prefer an advance engine (a) or a base engine (b)"
              << "\n";
    std::cin >> ans_eng;
    switch (ans_eng) {
      case 'a':
        interface::select_ad_eng(ad_eng_data);
        if (ad_eng_data.type == 'm') {
          ad_eng =
              rocket::Ad_engine{ad_eng_data.burn_a,      ad_eng_data.nozzle_as,
                                ad_eng_data.t_0,         ad_eng_data.grain_dim,
                                ad_eng_data.grain_rho,   ad_eng_data.a_coef,
                                ad_eng_data.burn_rate_n, ad_eng_data.prop_mm};
        } else {
          ad_eng = rocket::Ad_engine{ad_eng_data.p_0, ad_eng_data.burn_a,
                                     ad_eng_data.nozzle_as, ad_eng_data.t_0};
        }
        break;
      case 'b':
        interface::select_base_eng(base_eng_data);

        base_eng = rocket::Base_engine{base_eng_data.isp, base_eng_data.cm,
                                       base_eng_data.p0, base_eng_data.burn_a};
        break;

      default:
        throw std::runtime_error(
            "invalid value inserted, please restart ");
        break;
    }
    if (ans_eng == 'a') {
      eng = &ad_eng;
    } else {
      eng = &base_eng;
    }
    char ans_rocket;
    std::cout << "how many details do you want to insert to create your"
              << "\n"
              << "rocket (many (m), some (s), few (f))?"
              << "\n";
    std::cin >> ans_rocket;
    switch (ans_rocket) {
      case 'm':
        rocket_data = interface::create_complete_roc();
        l_p_m.resize(rocket_data.stage_num);
        l_c_m.resize(rocket_data.stage_num);
        n_liq_eng.resize(rocket_data.stage_num);
        int ans;
        std::cout
            << "how many engines does each liquid stage have: < 3"
            << "\n";
        std::cin >> ans;
        std::fill_n(l_p_m.begin(), rocket_data.stage_num, rocket_data.s_p_m);
        std::fill_n(l_c_m.begin(), rocket_data.stage_num, rocket_data.m_s_cont);
        std::fill_n(n_liq_eng.begin(), rocket_data.stage_num, ans);

        break;
      case 's':
        rocket_data = interface::create_med_roc();
        l_p_m.resize(rocket_data.stage_num);
        l_c_m.resize(rocket_data.stage_num);
        n_liq_eng.resize(rocket_data.stage_num);
        std::cout
            << "how many engines does each liquid stage have: < 3"
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
            << "how many engines does each liquid stage have: < 3"
            << "\n";
        std::cin >> ans;
        std::fill_n(l_p_m.begin(), rocket_data.stage_num, 400'000);
        std::fill_n(l_c_m.begin(), rocket_data.stage_num, 15'000);
        std::fill_n(n_liq_eng.begin(), rocket_data.stage_num, ans);
        break;

      default:
        throw std::runtime_error(
            "invalid value inserted for rocket, please restart ");
        break;
    }
  }

  std::string file_name = "assets/theta_data.txt";
  std::ofstream output_rocket("assets/output_rocket.txt");
  std::streampos start_pos;
  std::ofstream output_air("assets/output_air.txt");
  output_rocket << "posizione y-x   velocità y-x    forza y-x" << '\n';
  output_air << "temp    pres    rho" << '\n';
  if (!output_rocket.is_open() or !output_air.is_open()) {
    throw std::runtime_error("impossible to open file");
  }
  else{
  std::cout << "rocket output is ok" << '\n';
  std::cout << "also air output is ok" << '\n';
  }
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
  std::cout << "at which altitude do you want to orbit? (m) >60'000"
            << "\n";
  double orbital_h;
  std::cin >> orbital_h;

  float const width{1200.f};
  float const height{600.f};

  sf::RenderWindow window(sf::VideoMode(width, height), "Rocket simulator");
  window.setPosition(sf::Vector2i(0, 0));

  window.setFramerateLimit(5);

  sf::Texture texture1;
  if (!texture1.loadFromFile("rocket.png")) {
    std::cout << "error in loading rocket.png";
    throw std::runtime_error("error in loading rocket.png");
  }
  sf::Sprite rocket1;
  rocket1.setTexture(texture1);
  rocket1.setOrigin(sf::Vector2f(1024.f, 1024.f));
  rocket1.setScale(sf::Vector2f(0.2f, 0.2f));
  rocket1.setPosition(sf::Vector2f(250.f, height / 4.f * 3.f - 1024.f / 5.f));

  sf::RectangleShape inner_atm({500.f, 51000.f});
  inner_atm.setFillColor(sf::Color::Cyan);
  inner_atm.setPosition(0.f, height / 4.f * 3.f - 51000.f);

  sf::RectangleShape outer_atm({500.f, 49000.f});
  outer_atm.setFillColor(sf::Color::Blue);
  outer_atm.setPosition(0.f, height / 4.f * 3.f - 100000.f);

  sf::RectangleShape ground({500.f, 1000.f});
  ground.setFillColor(sf::Color::Green);
  ground.setPosition(0.f, height / 4.f * 3.f);

  sf::Texture texture2;
  if (!texture2.loadFromFile("earth.jpeg")) {
    std::cout << "error in loading earth.jpeg";
    throw std::runtime_error("error in loading earth.jpeg");
  }
  sf::Sprite earth;
  earth.setTexture(texture2);
  earth.setScale(200.f / 1195.f, 200.f / 1193.f);
  earth.setPosition((width - 500.f) / 4 + 500.f - 100.f, height / 4.f - 100.f);

  sf::CircleShape rocket2(2.f);
  rocket2.setFillColor(sf::Color::Red);
  rocket2.setOrigin(2.f, 2.f);
  rocket2.setPosition((width - 500.f) / 4 + 500.f, height / 4.f - 100.f);

  float angle_total{};

  sf::Texture texture3;
  if (!texture3.loadFromFile("map.jpg")) {
    std::cout << "error in loading map.jpg";
    throw std::runtime_error("error in loading map.jpg");
  }
  sf::Sprite map;
  map.setTexture(texture3);
  map.setScale(sf::Vector2f(700.f / 2058.f, 300.f / 1262.f));
  map.setPosition(500.f, height / 2);

  sf::CircleShape rocket3(2.f);
  rocket3.setFillColor(sf::Color::Red);
  rocket3.setOrigin(2.f, 2.f);
  rocket3.setPosition(750.f, height / 4 * 3);

  sf::Font tnr;
  if (!tnr.loadFromFile("times_new_roman.ttf")) {
    std::cout << "error in loading the font";
    throw std::runtime_error("error in loading the font");
  }

  sf::Text altitude;
  interface::set_text_style(altitude, tnr);
  altitude.setString("Altitude: ");
  altitude.setPosition((width - 500.f) / 2 + 500.f + 50.f, 30.f);

  sf::Text angle;
  interface::set_text_style(angle, tnr);
  angle.setString("Angle: ");
  angle.setPosition((width - 500.f) / 2 + 500.f + 50.f, 70.f);

  sf::Text speed;
  interface::set_text_style(speed, tnr);
  speed.setString("Speed: ");
  speed.setPosition((width - 500.f) / 2 + 500.f + 50.f, 110.f);

  sf::Text stage;
  interface::set_text_style(stage, tnr);
  stage.setString("Stage: ");
  stage.setPosition((width - 500.f) / 2 + 500.f + 50.f, 150.f);

  sf::Text fuel_left;
  interface::set_text_style(fuel_left, tnr);
  fuel_left.setString("Fuel left: ");
  fuel_left.setPosition((width - 500.f) / 2 + 500.f + 50.f, 190.f);

  int out_time{};

  sf::Text time;
  interface::set_text_style(time, tnr);
  time.setString("Time: ");
  time.setPosition((width - 500.f) / 2 + 500.f + 50.f, 230.f);

  sf::Text countdown;
  countdown.setFont(tnr);
  countdown.setCharacterSize(400);
  countdown.setFillColor(sf::Color::Yellow);
  countdown.setPosition((width - 500.f) / 2, 20.f);

  sf::Vertex line[]{sf::Vertex(sf::Vector2f(500.f, 0.f)),
                    sf::Vertex(sf::Vector2f(500.f, height))};

  sf::Vertex line1[]{sf::Vertex(sf::Vector2f(500.f, height / 2)),
                     sf::Vertex(sf::Vector2f(width, height / 2))};

  sf::Vertex line2[]{
      sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, 0.f)),
      sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, height / 2))};

  std::vector<sf::Drawable *> drawables{
      &map,      &ground, &inner_atm, &rocket1, &earth,     &rocket2, &rocket3,
      &altitude, &angle,  &speed,     &stage,   &fuel_left, &time};

  std::vector<sf::Vertex *> vertices{line, line1, line2};

  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile("launch.wav")) {
    std::cout << "error in loading the countdown audio";
    throw std::runtime_error("error in loading the countdown audio");
  }
  sf::Sound sound;
  sound.setBuffer(buffer);
  sound.play();

  sf::sleep(sf::seconds(0.5f));

  interface::run_countdown(countdown, drawables, vertices, window);

  sf::Music music;
  if (!music.openFromFile("background_music.wav")) {
    std::cout << "error in loading the background music";
    throw std::runtime_error("error in loading the background music");
  }

  music.play();
  music.setLoop(true);

  Vec eng_force;

  double delta_time{1};
  // game loop inizia
    while (window.isOpen()) {
      sf::Event event;

      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
          window.close();
        }
      }

      bool const orbiting{
          rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity());

      rocket.set_state(file_name, orbital_h, delta_time, orbiting, start_pos);

      air.set_state(rocket.get_pos()[0]-sim::cost::earth_radius_);

      eng_force = rocket.thrust(delta_time, orbiting);

      Vec const force{rocket::total_force(
          air.get_rho(), rocket.get_mass(), rocket.get_pos()[0]-sim::cost::earth_radius_,
          rocket.get_up_ar(), rocket.get_velocity(), eng_force, air.get_speed_sound()};

      double const angle_var{
          ((rocket.get_velocity()[1] + sim::cost::earth_speed_) * delta_time +
           0.5 * (force[1] / rocket.get_mass()) * std::pow(delta_time, 2)) /
          rocket_radius};
      angle_total += angle_var;

      rocket.move(delta_time, force);

      if ((rocket.get_velocity()[0] < 0. && eng_force != Vec({0, 0})) ||
          rocket.get_velocity()[1] < 0.) {
        std::cout << "error in velocity";
        throw std::runtime_error("error in velocity");
      }

      if (rocket.get_pos()[0] <= 0. || rocket.get_pos()[1] < 0.) {
        std::cout << "error in position";
        throw std::runtime_error("error in position");
      };

      output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                    << rocket.get_velocity()[0] << "  "
                    << rocket.get_velocity()[1] << "  " << force[0] << "  "
                    << force[1] << '\n';
      output_air << air.get_t() << " " << air.get_p() << " " << air.get_rho() << '\n';

      // grafica

      int const out_time_min{out_time / 60};
      int const out_time_sec{out_time - out_time_min * 60};
      time.setString("Time: " + std::to_string(out_time_min) + " min " +
                     std::to_string(out_time_sec) + " sec ");

      altitude.setString("Altitude: " + std::to_string(rocket.get_pos()[0]) +
                         " m");

      angle.setString("Angle: " + std::to_string(rocket.get_theta()) + " rad");

      stage.setString("Stage: " + std::to_string(rocket.get_rem_stage()) + "/" +
                      std::to_string(rocket_data.stage_num + 1));

      speed.setString(
          "Speed: " +
          std::to_string(
              sqrt(std::pow(rocket.get_velocity()[0], 2) +
                   std::pow(rocket.get_velocity()[1] + sim::cost::earth_speed_,
                            2))) +
          " m/s");

      fuel_left.setString("Fuel left: " +
                          std::to_string(rocket.get_fuel_left()));

      rocket1.setRotation(90 - rocket.get_theta() * 360 / (2 * M_PI));
      outer_atm.setPosition(0.f,
                            rocket.get_pos()[0] + (height * 3 / 4) - 100'000);
      ground.setPosition(0.f, rocket.get_pos()[0] + (height * 3 / 4));
      inner_atm.setPosition(0.f,
                            rocket.get_pos()[0] + (height * 3 / 4) - 51'000);

      if (rocket.get_pos()[0] / sim::cost::earth_radius_ * 100.f < 50.f) {
        earth.setScale(200.f / 1195.f, 200.f / 1193.f);
        earth.setPosition((width - 500.f) / 4 + 500.f - 100.f,
                          height / 4.f - 100.f);
        rocket2.setPosition(
            (width - 500.f) / 4 + 500.f -
                100.f * rocket_radius / sim::cost::earth_radius_ *
                    std::sin(angle_total),
            height / 4.f - 100.f * rocket_radius / sim::cost::earth_radius_ *
                               std::cos(angle_total));
      } else {
        earth.setScale(2.f / 1195.f, 2.f / 1193.f);
        earth.setPosition((width - 500.f) / 4 + 500.f - 1.f,
                          height / 4.f - 1.f);
        rocket2.setPosition((width - 500.f) / 4 + 500.f -
                                1.f * rocket_radius / sim::cost::earth_radius_ *
                                    std::sin(rocket.get_theta()),
                            height / 4.f - 1.f * rocket_radius /
                                               sim::cost::earth_radius_ *
                                               std::cos(angle_total));
      }

      rocket3.setPosition(angle_total / 2 / M_PI * 700.f + 750.f,
                          height / 4 * 3);
      sf::Vector2f const pos3{rocket3.getPosition()};
      if (pos3.x > width) {
        rocket3.setPosition(pos3.x - floor((pos3.x - 500.f) / (width - 500.f)) *
                                         (width - 500.f),
                            pos3.y);
      }

      window.clear();

      std::for_each(drawables.begin(), drawables.end(),
                    [&](sf::Drawable *obj) { window.draw(*obj); });

      std::for_each(vertices.begin(), vertices.end(),
                    [&](sf::Vertex *obj) { window.draw(obj, 2, sf::Lines); });

      window.display();

      out_time += delta_time;
    }
  } catch (const std::exception &e) {
    sf::Font font;
    font.loadFromFile("times_new_roman.ttf");
    interface::handle_exception(e.what(), font);

  } catch (...) {
    sf::Font font;
    font.loadFromFile("times_new_roman.ttf");
    interface::handle_exception("unknown exception", font);
  }
}