#include <stdio.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "interface.h"
#include "rocket.h"
#include "simulation.h"

int main()
{
  float width{1200.f};
  float height{600.f};
  sf::RenderWindow window(sf::VideoMode(width, height), "Rocket simulator");
  window.setPosition(sf::Vector2i(0, 0));

  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  sf::Music music;
  if (!music.openFromFile("01_A Better Beginning.wav"))
    return -1; // error
  music.play();
  music.setLoop(true);

  sf::Texture texture1;
  if (!texture1.loadFromFile("rocket.png"))
  {
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
  if (!texture2.loadFromFile("earth.jpeg"))
  {
    // error
  }
  sf::Sprite earth;
  earth.setTexture(texture2);
  earth.setScale(sf::Vector2f(200.f / 1195.f, 200.f / 1193.f));
  earth.setPosition((width - 500.f) / 4 + 500.f - 100.f, height / 4.f - 100.f);

  sf::CircleShape rocket2(1.f);
  rocket2.setFillColor(sf::Color::Red);
  rocket2.setOrigin(1.f, 1.f);

  float angle_total{};

  sf::Texture texture3;
  if (!texture3.loadFromFile("map.jpg"))
  {
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

  sf::Font tnr;
  if (!tnr.loadFromFile("times_new_roman.ttf"))
  {
    // error
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
  time.setPosition((width - 500.f) / 2 + 500.f + 50.f, 230.f);

  sf::Text countdown;
  countdown.setFont(tnr);
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

  std::vector<sf::Drawable *> drawables{
      &map, &ground, &inner_atm, &rocket1, &earth, &rocket2, &rocket3,
      &altitude, &angle, &speed, &stage, &fuel_left, &time};

  std::vector<sf::Vertex *> vertices{line, line1, line2};

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
  rocket::Rocket rocket;

  { // aggiungo uno scope in modo da cancellare poi tutte le variabili che non
    // servono
    char ans_eng;
    interface::rocket_data rocket_data;
    interface::ad_eng_data ad_eng_data;
    interface::base_eng_data base_eng_data;
    std::cout << "let's build the engine of your rocket"
              << "\n"
              << "do you prefer an advance (a) rocket or base rocket (b)"
              << "\n";
    std::cin >> ans_eng;
    rocket::Rocket::Ad_engine ad;
    rocket::Rocket::Base_engine base;
    switch (ans_eng)
    {
    case 'a':
      interface::select_ad_eng(ad_eng_data);
      switch (ad_eng_data.type)
      {
      case 'm':
        ad = rocket::Rocket::Ad_engine{
            ad_eng_data.burn_a, ad_eng_data.nozzle_as,
            ad_eng_data.t_0, ad_eng_data.grain_dim,
            ad_eng_data.grain_rho, ad_eng_data.a_coef,
            ad_eng_data.burn_rate_n};
        break;

      case 'f':
        ad = rocket::Rocket::Ad_engine{ad_eng_data.p_0, ad_eng_data.burn_a,
                                       ad_eng_data.nozzle_as,
                                       ad_eng_data.t_0};
        break;
      }
    case 'b':
      interface::select_base_eng(base_eng_data);
      base =
          rocket::Rocket::Base_engine{base_eng_data.isp, base_eng_data.cm,
                                      base_eng_data.p0, base_eng_data.burn_a};

      std::cout << "invalid value";
      break;
    }
    char ans_roc;
    std::cout << "how many details do you want to insert for create your"
              << "\n"
              << "rocket (many (m), some (s), few (f))?"
              << "\n";
    std::cin >> ans_roc;
    switch (ans_roc)
    {
    case 'm':
      interface::create_complete_roc(rocket_data);

      break;
    case 's':
      interface::create_med_roc(rocket_data);
      break;

    case 'f':
      interface::create_minim_roc(rocket_data);
      break;

    default:
      std::cout << "invalid value";
      break;
    }
    if (ans_eng = 'b')
    {
      std::unique_ptr<rocket::Rocket::Engine> eng_b =
          std::make_unique<rocket::Rocket::Base_engine>(base);
      std::vector<std::unique_ptr<rocket::Rocket::Engine>> vec_liq;
      // per il momento faccio così poi vedo di cambiare
      for (int i{0}; i <= rocket_data.stage_num; i++)
      {
        vec_liq.push_back(std::move(eng_b));
      }
      rocket = rocket::Rocket{
          rocket_data.name, rocket_data.mass_structure, rocket_data.up_ar,
          rocket_data.lat_ar, rocket_data.s_p_m, rocket_data.m_s_cont,
          rocket_data.l_p_m, rocket_data.l_c_m, eng_b, vec_liq, rocket_data.n_solid_eng,
          rocket_data.n_liq_eng};
    }
    else
    {
      std::unique_ptr<rocket::Rocket::Engine> eng_ad =
          std::make_unique<rocket::Rocket::Ad_engine>(ad);
      std::vector<std::unique_ptr<rocket::Rocket::Engine>> vec_liq;
      // per il momento faccio così poi vedo di cambiare
      for (int i{0}; i <= rocket_data.stage_num; i++)
      {
        vec_liq.push_back(std::move(eng_ad));
      }
      rocket = rocket::Rocket{
          rocket_data.name, rocket_data.mass_structure, rocket_data.up_ar,
          rocket_data.lat_ar, rocket_data.s_p_m, rocket_data.m_s_cont,
          rocket_data.l_p_m, rocket_data.l_c_m, eng_ad, vec_liq, rocket_data.n_solid_eng,
          rocket_data.n_liq_eng};
    }
  }
  sim::Air_var air;
  int i{0};
  double delta_time{1.7e-2}; // camiato per ragioni di usare un int per i millisec dopo
  std::streampos start_pos;
  Vec force;
  double delta_ms{0.};
  double delta_ml{0.};
  std::cout << "a che altezza vuoi orbitare?";
  double orbital_h;
  std::cin >> orbital_h;

  interface::run_countdown(countdown, drawables, vertices, window);

  // game loop starts

  while (window.isOpen())
  {
    sf::Event event;

    bool orbiting =
        rocket::is_orbiting(rocket.get_pos()[0], rocket.get_velocity()[1]);

    rocket.set_state(file_name, orbital_h, delta_time, orbiting, start_pos);

    air.set_state(rocket.get_pos()[0]);

    Vec eng_force = rocket.thrust(air.p_, delta_time, orbiting);

    force = rocket::total_force(
        air.rho_, rocket.get_theta(), rocket.get_mass(), rocket.get_pos()[0],
        delta_time, rocket.get_up_ar(), rocket.get_lat_ar(), orbiting,
        rocket.get_velocity(), eng_force);

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

    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
      {
        window.close();
      }
    }

    int out_time_min{out_time / 1000 / 60};
    int out_time_sec{out_time / 1000 - out_time_min * 60};
    int out_time_millisec{out_time - out_time_sec * 1000 - out_time_min * 1000 * 60};
    time.setString("Time: " + std::to_string(out_time_min) + " min " +
                   std::to_string(out_time_sec) + " s " +
                   std::to_string(out_time_millisec) + " ms ");

    rocket1.rotate(rocket.get_theta() * 360 / (2 * M_PI)); // forse è un set rotation in realtà
    outer_atm.setPosition(0,
                          rocket.get_pos()[0] + (height * 3 / 4) - 100'000);
    ground.setPosition(0.f, rocket.get_pos()[0] + (height * 3 / 4));
    inner_atm.setPosition(0.f,
                          rocket.get_pos()[0] + (height * 3 / 4) - 51'000);

    double const rocket_radius{rocket.get_pos()[0] + sim::cost::earth_radius_};
    angle_total += rocket.get_velocity()[1] * delta_time / rocket_radius;
    rocket2.setPosition(
        (width - 500.f) / 4 + 500.f - 100.f * rocket_radius / sim::cost::earth_radius_ * std::sin(rocket.get_theta()),
        height / 4.f - 200.f + 100.f * rocket_radius / sim::cost::earth_radius_ * std::cos(angle_total));

    rocket3.move(0.1f, 0.f);
    const sf::Vector2f a{rocket3.getPosition()};
    if (a.x > width)
    {
      rocket3.setPosition(a.x - (width - 500.f), a.y);
    }

    window.clear();

    std::for_each(drawables.begin(), drawables.end(),
                  [&](sf::Drawable *obj)
                  { window.draw(*obj); });

    std::for_each(vertices.begin(), vertices.end(),
                  [&](sf::Vertex *obj)
                  { window.draw(obj, 2, sf::Lines); });

    window.display();

    out_time += delta_time;
  }

  return 0;
}