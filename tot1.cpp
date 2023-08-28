#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

void run_countdown(sf::Text& countdown, std::vector<sf::Drawable*>& drawables,
                   std::vector<sf::Vertex*>& vertices,
                   sf::RenderWindow& window) {
  sf::Clock clock;
  for (int i = 10; i >= 1; --i) {
    countdown.setString(std::to_string(i));
    window.clear(sf::Color::White);
    window.display();
    sf::Time dur=sf::Time::Zero;
    clock.restart();
    while (dur.asSeconds() < 1.0f) {
      std::for_each(drawables.begin(), drawables.end(),
                    [&](sf::Drawable* obj) { window.draw(*obj); });
      std::for_each(vertices.begin(), vertices.end(),
                    [&](sf::Vertex* obj) { window.draw(obj, 2, sf::Lines); });
      window.draw(countdown);
      dur = clock.getElapsedTime();
    }
  }

  countdown.setString("LIFT OFF!");
  window.clear(sf::Color::White);
  std::for_each(drawables.begin(), drawables.end(),
                [&](sf::Drawable* obj) { window.draw(*obj); });

  std::for_each(vertices.begin(), vertices.end(),
                [&](sf::Vertex* obj) { window.draw(obj, 2, sf::Lines); });
  window.draw(countdown);
  window.display();
}

int main() {
  float width{1200.f};
  float height{600.f};
  sf::RenderWindow window(sf::VideoMode(width, height), "Rocket simulator");
  window.setPosition(sf::Vector2i(0, 0));

  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  sf::Music music;
  if (!music.openFromFile("01_A Better Beginning.wav")) return -1;  // error
  music.play();
  music.setLoop(true);

  sf::Texture texture1;
  if (!texture1.loadFromFile("rocket.png")) {
    // error
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
    // error
  }
  sf::Sprite earth;
  earth.setTexture(texture2);
  earth.setScale(sf::Vector2f(200.f / 1195.f, 200.f / 1193.f));
  earth.setPosition((width - 500.f) / 4 + 500.f - 100.f, height / 4.f - 100.f);

  sf::CircleShape rocket2(1.f);
  rocket2.setFillColor(sf::Color::Red);
  rocket2.setOrigin(1.f, 1.f);

  sf::Texture texture3;
  if (!texture3.loadFromFile("map.jpg")) {
    // error
  }
  sf::Sprite map;
  map.setTexture(texture3);
  map.setScale(sf::Vector2f(700.f / 2058.f, 300.f / 1262.f));
  map.setPosition(500.f, height / 2);

  sf::CircleShape rocket3(2.f);
  rocket3.setFillColor(sf::Color::Red);
  rocket3.setOrigin(2.f, 2.f);
  rocket3.setPosition(750.f, height / 4 * 3);

  sf::Font font;
  if (!font.loadFromFile("times_new_roman.ttf")) {
    // error
  }

  sf::Text altitude;
  altitude.setFont(font);
  altitude.setString("Altitude: ");
  altitude.setCharacterSize(24);
  altitude.setFillColor(sf::Color::White);
  altitude.setPosition((width - 500.f) / 2 + 500.f + 50.f, 30.f);

  sf::Text angle;
  angle.setFont(font);
  angle.setString("Angle: ");
  angle.setCharacterSize(24);
  angle.setFillColor(sf::Color::White);
  angle.setPosition((width - 500.f) / 2 + 500.f + 50.f, 70.f);

  sf::Text speed;
  speed.setFont(font);
  speed.setString("Speed: ");
  speed.setCharacterSize(24);
  speed.setFillColor(sf::Color::White);
  speed.setPosition((width - 500.f) / 2 + 500.f + 50.f, 110.f);

  sf::Text stage;
  stage.setFont(font);
  stage.setString("Stage: ");
  stage.setCharacterSize(24);
  stage.setFillColor(sf::Color::White);
  stage.setPosition((width - 500.f) / 2 + 500.f + 50.f, 150.f);

  sf::Text fuel_left;
  fuel_left.setFont(font);
  fuel_left.setString("Fuel left: ");
  fuel_left.setCharacterSize(24);
  fuel_left.setFillColor(sf::Color::White);
  fuel_left.setPosition((width - 500.f) / 2 + 500.f + 50.f, 190.f);

  int out_time{};

  sf::Text time;
  time.setFont(font);
  time.setCharacterSize(24);
  time.setFillColor(sf::Color::White);
  time.setPosition((width - 500.f) / 2 + 500.f + 50.f, 230.f);

  sf::Text countdown;
  countdown.setFont(font);
  countdown.setCharacterSize(400);
  countdown.setFillColor(sf::Color::Yellow);
  countdown.setPosition((width - 500.f) / 2, 20.f);

  sf::Vertex line[] = {sf::Vertex(sf::Vector2f(500.f, 0.f)),
                       sf::Vertex(sf::Vector2f(500.f, height))};

  sf::Vertex line1[] = {sf::Vertex(sf::Vector2f(500.f, height / 2)),
                        sf::Vertex(sf::Vector2f(width, height / 2))};

  sf::Vertex line2[] = {
      sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, 0.f)),
      sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, height / 2))};

  std::vector<sf::Drawable*> drawables{
      &map,      &ground, &inner_atm, &rocket1, &earth,     &rocket2, &rocket3,
      &altitude, &angle,  &speed,     &stage,   &fuel_left, &time};

  std::vector<sf::Vertex*> vertices{line, line1, line2};

  // inizializzazione parametri

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

  rocket::Rocket rocket{name,  mass_structure, Up_Ar,       Lat_Ar,
                        s_p_m, m_s_cont,       l_p_m,       l_c_m,
                        eng_b, vec_liq,        n_solid_eng, n_liq_eng};

  sim::Air_var air;
  int i{0};
  double delta_time{1.667e-2};
  std::streampos start_pos;
  Vec force;
  double delta_ms{0.};
  double delta_ml{0.};
  std::cout << "a che altezza vuoi orbitare?";
  double orbital_h;
  std::cin >> orbital_h;

  run_countdown(countdown, drawables, vertices, window);

  // game loop starts

  while (window.isOpen()) {
    sf::Event event;

    bool orbiting =
        rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity()[1]);

    rocket.set_state(file_name, orbital_h, delta_time, orbiting, start_pos);

    air.set_state(rocket.get_pos()[0]);

    Vec eng_force = rocket.thrust(air.p_, delta_time, orbiting);

    force = rocket::total_force(air.rho_, rocket.get_theta(), rocket.get_mass(),
                                rocket.get_pos()[0], delta_time,
                                rocket.get_up_ar(), rocket.get_lat_ar(),
                                orbiting, rocket.get_velocity(), eng_force);

    rocket.change_vel(delta_time, force);
    assert(rocket.get_velocity()[0] >= 0. && rocket.get_velocity()[0] >= 0.);
    rocket.move(delta_time, force);
    assert(rocket.get_pos()[0] >= 0. && rocket.get_pos()[0] >= 0.);
    output_rocket << rocket.get_pos()[0] << "  " << rocket.get_pos()[1]
                  << rocket.get_velocity()[0] << "  "
                  << rocket.get_velocity()[1] << "  " << force[0] << "  "
                  << force[1] << '\n';
    output_air << air.t_ << " " << air.p_ << " " << air.rho_ << '\n';
    i += 1;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
      }
    }

    int out_time_min{out_time / 60};
    int out_time_sec{out_time - out_time_min * 60};
    time.setString("Time: " + std::to_string(out_time_min) + " min " +
                   std::to_string(out_time_sec) + " sec ");

    rocket1.rotate(rocket.get_theta() * 360 / (2 * M_PI));
    outer_atm.setPosition(0, rocket.get_pos()[0] + (height * 3 / 4) - 100'000);
    ground.setPosition(0.f, rocket.get_pos()[0] + (height * 3 / 4));
    inner_atm.setPosition(0.f, rocket.get_pos()[0] + (height * 3 / 4) - 51'000);

    float angle_total{};
    rocket2.setPosition(
        (width - 500.f) / 4 + 500.f -
            sim::cost::earth_radius_ * std::sin(rocket.get_theta()),
        height / 4.f - 200.f + 100.f * std::cos(angle_total));
    // altitudine .get_pos()[0];
    // velocità tangenziale get_velocity()[1] ;

    rocket3.move(0.1f, 0.f);
    const sf::Vector2f a{rocket3.getPosition()};
    if (a.x > width) {
      rocket3.setPosition(a.x - (width - 500.f), a.y);
    }

    window.clear();

    std::for_each(drawables.begin(), drawables.end(),
                  [&](sf::Drawable* obj) { window.draw(*obj); });

    std::for_each(vertices.begin(), vertices.end(),
                  [&](sf::Vertex* obj) { window.draw(obj, 2, sf::Lines); });

    window.display();

    out_time += 1;
  }

  return 0;
}