#include <cassert>
#include <fstream>
#include <iostream>
#include <numbers>
#include <stdio.h>
#include "interface.h"
#include "simulation.h"

namespace {
constexpr double kPi = std::numbers::pi_v<double>;
}  // namespace

int main() {
  float const width{1200.f};
  float const height{600.f};
  sf::RenderWindow window(sf::VideoMode(width, height), "Rocket simulator");
  window.setPosition(sf::Vector2i(0, 0));

  window.setFramerateLimit(5);

  sf::Texture texture1;
  if (!texture1.loadFromFile("rocket.png")) {
    return -1;
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
    return -1;
  }
  sf::Sprite earth;
  earth.setTexture(texture2);
  earth.setScale(200.f / 1195.f, 200.f / 1193.f);
  earth.setPosition((width - 500.f) / 4 + 500.f - 100.f, height / 4.f - 100.f);

  sf::CircleShape rocket2(2.f);
  rocket2.setFillColor(sf::Color::Red);
  rocket2.setOrigin(2.f, 2.f);
  rocket2.setPosition((width - 500.f) / 4 + 500.f, height / 4.f - 100.f);

  float angle_total{0.f};

  sf::Texture texture3;
  if (!texture3.loadFromFile("map.jpg")) {
    return -1;
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
    return -1;
  }

  sf::Text altitude;
  interface::set_text_style(altitude, tnr);
  altitude.setString("Altitude: ");
  altitude.setPosition((width - 500.f) / 2 + 500.f + 50.f, 30.f);

  sf::Text angle;
  interface::set_text_style(angle, tnr);
  angle.setString("Angle: ");
  angle.setPosition((width - 500.f) / 2 + 500.f + 50.f, 70.f);

  int out_time{};

  sf::Text time;
  interface::set_text_style(time, tnr);
  time.setString("Time: ");
  time.setPosition((width - 500.f) / 2 + 500.f + 50.f, 230.f);

  sf::Vertex line[]{sf::Vertex(sf::Vector2f(500.f, 0.f)),
                    sf::Vertex(sf::Vector2f(500.f, height))};

  sf::Vertex line1[]{sf::Vertex(sf::Vector2f(500.f, height / 2)),
                     sf::Vertex(sf::Vector2f(width, height / 2))};

  sf::Vertex line2[]{
      sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, 0.f)),
      sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, height / 2))};

  std::vector<sf::Drawable *> drawables{&map,   &ground,  &inner_atm, &rocket1,
                                        &earth, &rocket2, &rocket3,   &altitude,
                                        &angle, &time};

  std::vector<sf::Vertex *> vertices{line, line1, line2};

  double delta_time{1};
  double x{};
  double y{};
  double a{};
  // game loop inizia
  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
      }
    }

    double const rocket_distance{y + sim::cost::earth_radius_};
    double angle_var{angle_total};
    angle_total = x / rocket_distance;
    angle_var = angle_total-angle_var;

    x += 10;
    y += x * x;
    a += 10 * 3.14 / 180;

    assert(y >= 0. && x >= 0.);

    // grafica inizia

    int const out_time_min{out_time / 60};
    int const out_time_sec{out_time - out_time_min * 60};
    time.setString("Time: " + std::to_string(out_time_min) + " min " +
                   std::to_string(out_time_sec) + " sec ");

    altitude.setString("Altitude: " + std::to_string(y) + " m");

    angle.setString("Angle: " + std::to_string(a * 360 / (2 * kPi)) + "°");

    rocket1.setRotation(90 - a * 360 / (2 * kPi));
    outer_atm.setPosition(0.f, y + (height * 3 / 4) - 100'000);
    ground.setPosition(0.f, y + (height * 3 / 4));
    inner_atm.setPosition(0.f, y + (height * 3 / 4) - 51'000);

    if (y / sim::cost::earth_radius_ * 100.f < 150.f) {
      earth.setScale(200.f / 1195.f, 200.f / 1193.f);
      earth.setPosition((width - 500.f) / 4 + 500.f - 100.f,
                        height / 4.f - 100.f);
      rocket2.setPosition((width - 500.f) / 4 + 500.f -
                              100.f * rocket_distance / sim::cost::earth_radius_ *
                                  std::sin(angle_total),
                          height / 4.f - 200.f +
                              100.f * rocket_distance / sim::cost::earth_radius_ *
                                  std::cos(angle_total));
    } else {
      earth.setScale(2.f / 1195.f, 2.f / 1193.f);
      earth.setPosition((width - 500.f) / 4 + 500.f - 10.f,
                        height / 4.f - 10.f);
      rocket2.setPosition((width - 500.f) / 4 + 500.f -
                              10.f * rocket_distance / sim::cost::earth_radius_ *
                                  std::sin(angle_total),
                          height / 4.f - 200.f +
                              10.f * rocket_distance / sim::cost::earth_radius_ *
                                  std::cos(angle_total));
    }
    rocket3.move(angle_var / 2 / kPi * 700.f, 0.f);
    sf::Vector2f const pos3{rocket3.getPosition()};
    if (pos3.x > width) {
      rocket3.setPosition(pos3.x - (width - 500.f), pos3.y);
    }

    window.clear();

    std::for_each(drawables.begin(), drawables.end(),
                  [&](sf::Drawable *obj) { window.draw(*obj); });

    std::for_each(vertices.begin(), vertices.end(),
                  [&](sf::Vertex *obj) { window.draw(obj, 2, sf::Lines); });

    window.display();

    out_time += delta_time;
  }
}
