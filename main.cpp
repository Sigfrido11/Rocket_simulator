#include <fstream>
#include <iostream>
#include <filesystem>
#include <numbers>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <stdio.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "interface.h"
#include "rocket.h"
#include "simulation.h"
#include "engine.h"
#include "vector_math.h"

#ifndef ROCKET_ASSETS_DIR
#define ROCKET_ASSETS_DIR "assets"
#endif

//
// radial component [0]
// angular component [1]
//

namespace {
std::string asset_path(std::string_view relative_path) {
  return (std::filesystem::path(ROCKET_ASSETS_DIR) / relative_path).string();
}

constexpr double kPi = std::numbers::pi_v<double>;

std::string read_text_file(std::string const& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("cannot open config file: " + path);
  }
  return std::string((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
}

std::string extract_object(std::string const& text, std::string const& key) {
  std::string const marker = "\"" + key + "\"";
  std::size_t const key_pos = text.find(marker);
  if (key_pos == std::string::npos) {
    throw std::runtime_error("missing object key: " + key);
  }
  std::size_t const start = text.find('{', key_pos);
  if (start == std::string::npos) {
    throw std::runtime_error("invalid object for key: " + key);
  }

  int depth = 0;
  bool in_string = false;
  for (std::size_t i = start; i < text.size(); ++i) {
    char const c = text[i];
    if (c == '"' && (i == 0 || text[i - 1] != '\\')) {
      in_string = !in_string;
    }
    if (in_string) {
      continue;
    }
    if (c == '{') {
      ++depth;
    } else if (c == '}') {
      --depth;
      if (depth == 0) {
        return text.substr(start, i - start + 1);
      }
    }
  }
  throw std::runtime_error("unterminated object for key: " + key);
}

std::string extract_array(std::string const& text, std::string const& key) {
  std::string const marker = "\"" + key + "\"";
  std::size_t const key_pos = text.find(marker);
  if (key_pos == std::string::npos) {
    throw std::runtime_error("missing array key: " + key);
  }
  std::size_t const start = text.find('[', key_pos);
  if (start == std::string::npos) {
    throw std::runtime_error("invalid array for key: " + key);
  }

  int depth = 0;
  bool in_string = false;
  for (std::size_t i = start; i < text.size(); ++i) {
    char const c = text[i];
    if (c == '"' && (i == 0 || text[i - 1] != '\\')) {
      in_string = !in_string;
    }
    if (in_string) {
      continue;
    }
    if (c == '[') {
      ++depth;
    } else if (c == ']') {
      --depth;
      if (depth == 0) {
        return text.substr(start, i - start + 1);
      }
    }
  }
  throw std::runtime_error("unterminated array for key: " + key);
}

double extract_number(std::string const& text, std::string const& key) {
  std::regex const rx("\\\"" + key + "\\\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)");
  std::smatch match;
  if (!std::regex_search(text, match, rx)) {
    throw std::runtime_error("missing numeric key: " + key);
  }
  return std::stod(match[1].str());
}

double extract_number_or(std::string const& text, std::string const& key,
                         double fallback) {
  std::regex const rx("\\\"" + key + "\\\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)");
  std::smatch match;
  if (!std::regex_search(text, match, rx)) {
    return fallback;
  }
  return std::stod(match[1].str());
}

int extract_int(std::string const& text, std::string const& key) {
  return static_cast<int>(extract_number(text, key));
}

std::string extract_string(std::string const& text, std::string const& key) {
  std::regex const rx("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"");
  std::smatch match;
  if (!std::regex_search(text, match, rx)) {
    throw std::runtime_error("missing string key: " + key);
  }
  return match[1].str();
}

std::vector<double> extract_double_array(std::string const& text,
                                         std::string const& key) {
  std::vector<double> out;
  std::string const body = extract_array(text, key);
  std::regex const rx("-?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?");
  for (std::sregex_iterator it(body.begin(), body.end(), rx), end; it != end;
       ++it) {
    out.push_back(std::stod(it->str()));
  }
  return out;
}

std::vector<int> extract_int_array(std::string const& text,
                                   std::string const& key) {
  std::vector<int> out;
  std::string const body = extract_array(text, key);
  std::regex const rx("-?[0-9]+");
  for (std::sregex_iterator it(body.begin(), body.end(), rx), end; it != end;
       ++it) {
    out.push_back(std::stoi(it->str()));
  }
  return out;
}

template <typename T>
void normalize_size(std::vector<T>& values, std::size_t size, T fallback,
                    std::string const& field_name) {
  if (size == 0) {
    throw std::runtime_error("stage_num must be > 0");
  }
  if (values.empty()) {
    values.assign(size, fallback);
    return;
  }
  if (values.size() == 1 && size > 1) {
    values.assign(size, values.front());
    return;
  }
  if (values.size() != size) {
    throw std::runtime_error("invalid size for " + field_name);
  }
}

}  // namespace

int main() {
  interface::rocket_data rocket_data;
  std::vector<double> l_p_m;
  std::vector<double> l_c_m;
  std::vector<int> n_liq_eng;
  engine::Engine* solid_eng = nullptr;
  engine::Engine* liquid_eng = nullptr;

  try {
    char ans_eng{};
    std::cout << "do you prefer a base engine (b) or advanced engines (a)?"
              << "\n";
    std::cin >> ans_eng;
    if (ans_eng != 'a' && ans_eng != 'b') {
      throw std::runtime_error("invalid engine family, use 'a' or 'b'");
    }

    std::string file_name = asset_path("input_data/theta_data.txt");
    std::ifstream theta_file(file_name);
    if (!theta_file.is_open()) {
      throw std::runtime_error("Cannot open theta data file: " + file_name);
    }
    std::string const params_file = asset_path("input_data/simulation_params.json");
    std::string const params_text = read_text_file(params_file);

    std::string const rocket_obj = extract_object(params_text, "rocket");
    std::string const engine_obj = extract_object(params_text, "engine");
    std::string const base_obj = extract_object(engine_obj, "base");
    std::string const adv_sol_obj = extract_object(engine_obj, "advanced_solid");
    std::string const adv_liq_obj = extract_object(engine_obj, "advanced_liquid");

    rocket_data.name = extract_string(rocket_obj, "name");
    rocket_data.stage_num = extract_int(rocket_obj, "stage_num");
    rocket_data.mass_structure = extract_number(rocket_obj, "mass_structure_kg");
    rocket_data.up_ar = extract_number(rocket_obj, "upper_area_m2");
    rocket_data.s_p_m = extract_number(rocket_obj, "solid_propellant_mass_kg");
    rocket_data.m_s_cont = extract_number(rocket_obj, "solid_container_mass_kg");
    rocket_data.n_solid_eng = extract_int(rocket_obj, "solid_engine_count");

    l_p_m = extract_double_array(rocket_obj, "liquid_propellant_masses_kg");
    l_c_m = extract_double_array(rocket_obj, "liquid_container_masses_kg");
    n_liq_eng = extract_int_array(rocket_obj, "liquid_engines_per_stage");

    normalize_size(l_p_m, static_cast<std::size_t>(rocket_data.stage_num),
                   rocket_data.s_p_m, "liquid_propellant_masses_kg");
    normalize_size(l_c_m, static_cast<std::size_t>(rocket_data.stage_num),
                   rocket_data.m_s_cont, "liquid_container_masses_kg");
    normalize_size(n_liq_eng, static_cast<std::size_t>(rocket_data.stage_num),
                   1, "liquid_engines_per_stage");

    double const base_isp = extract_number(base_obj, "isp_s");
    double const base_cm = extract_number(base_obj, "cm");
    double const base_p0 = extract_number(base_obj, "chamber_pressure_pa");
    double const base_burn_a = extract_number(base_obj, "burn_area_m2");

    double const adv_p_c = extract_number_or(adv_sol_obj, "chamber_pressure_pa", 7.0e6);
    double const adv_t_c = extract_number(adv_sol_obj, "chamber_temperature_k");
    double const adv_a_b = extract_number(adv_sol_obj, "burn_area_m2");
    double const adv_a_t = extract_number(adv_sol_obj, "nozzle_throat_area_m2");
    double const adv_rho = extract_number(adv_sol_obj, "grain_density_kg_m3");
    double const adv_a = extract_number(adv_sol_obj, "burn_rate_a");
    double const adv_n = extract_number(adv_sol_obj, "burn_rate_n");
    double const adv_m = extract_number(adv_sol_obj, "propellant_molar_mass_g_mol") / 1000.0;

    double const liq_p_c = extract_number(adv_liq_obj, "chamber_pressure_pa");
    double const liq_t_c = extract_number(adv_liq_obj, "chamber_temperature_k");
    double const liq_a_t = extract_number(adv_liq_obj, "nozzle_throat_area_m2");
    double const liq_a_e = extract_number(adv_liq_obj, "burn_area_m2");

    engine::Base_engine base_solid{base_isp, base_cm, base_p0, base_burn_a};
    engine::Base_engine base_liquid{base_isp, base_cm, base_p0, base_burn_a};
    engine::Ad_sol_engine ad_solid{adv_p_c, adv_t_c, adv_a_b, adv_a_t, adv_rho,
                                   adv_a, adv_n, adv_m};
    engine::Ad_liquid_engine ad_liquid{liq_p_c, liq_t_c, liq_a_t, liq_a_e};

    if (ans_eng == 'a') {
      solid_eng = &ad_solid;
      liquid_eng = &ad_liquid;
    } else {
      solid_eng = &base_solid;
      liquid_eng = &base_liquid;
    }

    std::cout << "at which altitude do you want to orbit? (m) >60'000"
              << "\n";
    double orbital_h;
    std::cin >> orbital_h;

    std::ofstream output_rocket(asset_path("output_rocket.txt"));
    std::streampos start_pos;
    std::ofstream output_air(asset_path("output_air.txt"));
    output_rocket << "posizione y-x   velocità y-x    forza y-x" << '\n';
    output_air << "temp    pres    rho" << '\n';
    if (!output_rocket.is_open() or !output_air.is_open()) {
      throw std::runtime_error("impossible to open file");
    } else {
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
                          solid_eng,
                          liquid_eng,
                          rocket_data.n_solid_eng,
                          n_liq_eng};

    sim::Air_var air;
  float const width{1200.f};
  float const height{600.f};

  sf::RenderWindow window(sf::VideoMode(width, height), "Rocket simulator");
  window.setPosition(sf::Vector2i(0, 0));

  window.setFramerateLimit(5);

  sf::Texture texture1;
  if (!texture1.loadFromFile(asset_path("img/rocket.png"))) {
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
  if (!texture2.loadFromFile(asset_path("img/earth.jpeg"))) {
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
  if (!texture3.loadFromFile(asset_path("img/map.jpg"))) {
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
  if (!tnr.loadFromFile(asset_path("font/times_new_roman.ttf"))) {
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
  if (!buffer.loadFromFile(asset_path("music/launch.wav"))) {
    std::cout << "error in loading the countdown audio ";
    throw std::runtime_error("error in loading the countdown audio");
  }
  sf::Sound sound;
  sound.setBuffer(buffer);
  sound.play();

  sf::sleep(sf::seconds(0.5f));

  interface::run_countdown(countdown, drawables, vertices, window);

  sf::Music music;
  if (!music.openFromFile(asset_path("music/background_music.wav"))) {
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
          rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity())};

      rocket.set_state(theta_file, orbital_h, delta_time, orbiting, start_pos);

      air.set_state(rocket.get_pos()[0]-sim::cost::earth_radius_);
      double const pa = air.get_p();
      eng_force = rocket.thrust(delta_time, pa, orbiting);

      Vec const force{rocket::total_force(
          air.get_rho(), rocket.get_mass(), rocket.get_pos()[0]-sim::cost::earth_radius_,
          rocket.get_up_ar(), rocket.get_velocity(), eng_force, air.get_speed_sound())};

      double const angle_var{
          ((rocket.get_velocity()[1] + sim::cost::earth_speed_) * delta_time +
           0.5 * (force[1] / rocket.get_mass()) * std::pow(delta_time, 2)) /
          rocket.get_pos()[0]};
      angle_total += angle_var;

      rocket.move(delta_time, force);

      if (rocket.get_velocity()[0] < 0. && rocket.get_pos()[0]< sim::cost::earth_radius_) {
        std::cout << "error in velocity ";
        std::cout << rocket.get_velocity()[0];
        
        throw std::runtime_error("error in velocity");
      }

      if (eng_force == Vec(0.0, 0.0) && !orbiting && rocket.get_rem_stage() > 0) {
        std::cout << "error in thrust";
        throw std::runtime_error("error in thrust");
      }

      if (rocket.get_pos()[0] <= 0.) {
        std::cout << "error in position";
        throw std::runtime_error("error in position");
      };

      output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                    << rocket.get_velocity()[0] << "  "
                    << rocket.get_velocity()[1] << "  " << force[0] << "  "
                    << force[1] << '\n';
      output_air << air.get_t() << " " << air.get_p() << " " << air.get_rho() << '\n';

      double altitude_rocket =  rocket.get_pos()[0] * std::sin(rocket.get_pos()[1]);

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

      rocket1.setRotation(90 - rocket.get_theta() * 360 / (2 * kPi));
      outer_atm.setPosition(0.f,
                            altitude_rocket + (height * 3 / 4) - 100'000);
      ground.setPosition(0.f, altitude_rocket + (height * 3 / 4));
      inner_atm.setPosition(0.f,
                            altitude_rocket + (height * 3 / 4) - 51'000);

      if (altitude_rocket / sim::cost::earth_radius_ * 100.f < 50.f) {
        earth.setScale(200.f / 1195.f, 200.f / 1193.f);
        earth.setPosition((width - 500.f) / 4 + 500.f - 100.f,
                          height / 4.f - 100.f);
        rocket2.setPosition(
            (width - 500.f) / 4 + 500.f -
                100.f * altitude_rocket/ sim::cost::earth_radius_ *
                    std::sin(angle_total),
            height / 4.f - 100.f * altitude_rocket / sim::cost::earth_radius_ *
                               std::cos(angle_total));
      } else {
        earth.setScale(2.f / 1195.f, 2.f / 1193.f);
        earth.setPosition((width - 500.f) / 4 + 500.f - 1.f,
                          height / 4.f - 1.f);
        rocket2.setPosition((width - 500.f) / 4 + 500.f -
                                1.f * altitude_rocket / sim::cost::earth_radius_ *
                                    std::sin(rocket.get_theta()),
                            height / 4.f - 1.f * altitude_rocket /
                                               sim::cost::earth_radius_ *
                                               std::cos(angle_total));
      }

      rocket3.setPosition(angle_total / 2 / kPi * 700.f + 750.f,
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
    font.loadFromFile(asset_path("font/times_new_roman.ttf"));
    interface::handle_exception(e.what(), font);

  } catch (...) {
    sf::Font font;
    font.loadFromFile(asset_path("font/times_new_roman.ttf"));
    interface::handle_exception("unknown exception", font);
  }
}
