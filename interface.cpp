#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "interface.h"

namespace interface {

void set_text_style(sf::Text &text, sf::Font &font) {
  text.setFont(font);
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::White);
}

void create_ad_eng_all(ad_eng_data &eng_data) {
  eng_data.type = 'm';
  std::cout << "give me the burn area (m^2) ≈ 2.2" << '\n';
  std::cin >> eng_data.burn_a;
  std::cout << "give me the throat area (m^2) ≈ 2.2" << '\n';
  std::cin >> eng_data.nozzle_as;
  std::cout << "give me the temperature of the engine 2'500 < x < 3'800"
            << '\n';
  std::cin >> eng_data.t_0;
  std::cout << "give me the dimension of the grain for the engine ≈0.02"
            << '\n';
  std::cin >> eng_data.grain_dim;
  std::cout << "give me the density of the grain for the engine ≈2800" << '\n';
  std::cin >> eng_data.grain_rho;
  std::cout << "give me the engine burn rate a coefficent ≈50" << '\n';
  std::cin >> eng_data.a_coef;
  std::cout << "give me the engine burn rate n coefficent ≈0.02" << '\n';
  std::cin >> eng_data.burn_rate_n;
  std::cout << "give me the molar mass of the propellant (g/mol) ≈180" << '\n';
  std::cin >> eng_data.prop_mm;
}

void create_ad_eng_med(ad_eng_data &eng_data) {
  eng_data.type = 's';
  std::cout << "give me the burn area (m^2) ≈ 2.2" << '\n';
  std::cin >> eng_data.burn_a;
  std::cout << "give me the throat area (m^2) ≈ 2.2" << '\n';
  std::cin >> eng_data.nozzle_as;
  std::cout << "give me the temperature inside the engine 2'500 < x < 3'800"
            << '\n';
  std::cin >> eng_data.t_0;
  std::cout << "give me the pressure inside the engine (Pa) ≈e6" << '\n';
  std::cin >> eng_data.p_0;
}

void create_ad_eng_minim(ad_eng_data &eng_data) {
  eng_data.type = 'f';
  std::cout << "give me the pressure inside the engine (Pa) ≈e6" << '\n';
  std::cin >> eng_data.p_0;
  std::cout << "give me the temperature inside the engine 2'500 < x < 3'800"
            << '\n';
  std::cin >> eng_data.t_0;
}

rocket_data create_complete_roc() {
  rocket_data rocket_data;
  std::cout << "give a name to your rocket"
            << '\n';
  std::cin >> rocket_data.name;
  std::cout << "how many liquid stages do you want (usually 1 < x < 4)"
            << '\n';
  std::cin >> rocket_data.stage_num;
  std::cout << "what's the upper area of your rocket (metres) ≈ 1000"
            << '\n';
  std::cin >> rocket_data.up_ar;
  std::cout << "what's the mass of the structure of your rocket (kg) ≈ 10'000"
            << '\n';
  std::cin >> rocket_data.mass_structure;
  std::cout << "what's the mass of each fuel container of your rocket (kg) ≈ 15'000"
            << '\n';
  std::cin >> rocket_data.m_s_cont;
  std::cout << "what's the mass of the propellant for each stage of your rocket (kg) ≈ 100'000"
            << '\n';
  std::cin >> rocket_data.s_p_m;
  std::cout << "how many engines does the solid stage have: < 3 "
            << '\n';
  std::cin >> rocket_data.n_solid_eng;
  return rocket_data;
}

rocket_data create_med_roc() {
  rocket_data rocket_data;
  std::cout << "give a name to your rocket"
            << '\n';
  std::cin >> rocket_data.name;
  std::cout << "how many liquid stages do you want (usually 1 < x < 4)"
            << '\n';
  std::cin >> rocket_data.stage_num;
  std::cout << "what's the mass of the propellant for each stage of your rocket (kg) "
               "≈ 100'000"
            << '\n';
  std::cin >> rocket_data.s_p_m;
  std::cout << "how many engines does the solid stage have: < 3 "
            << '\n';
  std::cin >> rocket_data.n_solid_eng;
  return rocket_data;
}

rocket_data create_minim_roc() {
  interface::rocket_data rocket_data;
  std::cout << "give a name to your rocket"
            << '\n';
  std::cin >> rocket_data.name;
  std::cout << "how many liquid stages do you want 1 < x < 4"
            << '\n';
  std::cin >> rocket_data.stage_num;
  std::cout << "how many engines does the solid stage have: < 3"
            << '\n';
  std::cin >> rocket_data.n_solid_eng;
  return rocket_data;
}

void create_base_eng_all(base_eng_data &eng_data) {
  eng_data.type = 'm';
  std::cout << "give me the specific impulse (s) 150 < x < 300" << '\n';
  std::cin >> eng_data.isp;
  std::cout << "give me the coefficient of losing mass 1.5 < x < 4. " << '\n';
  std::cin >> eng_data.cm;
  std::cout << "give me the pressure inside the engine (Pa) ≈e6" << '\n';
  std::cin >> eng_data.p0;
  std::cout << "give me the burn area for the engine (m^2) ≈ 2e-4" << '\n';
  std::cin >> eng_data.burn_a;
}

void create_base_eng_med(base_eng_data &eng_data) {
  eng_data.type = 's';
  std::cout << "give me the specific impulse (s) 150 < x < 300" << '\n';
  std::cin >> eng_data.isp;
  std::cout << "give me the coefficient of losing mass 1.5 < x < 4. " << '\n';
  std::cin >> eng_data.cm;
  std::cout << "give me the pressure inside the engine (Pa) ≈e6" << '\n';
  std::cin >> eng_data.p0;
}

void create_base_eng_minim(base_eng_data &eng_data) {
  eng_data.type = 'f';
  std::cout << "give me the specific impulse (s) 150 < x < 300" << '\n';
  std::cin >> eng_data.isp;
  std::cout << "give me the coefficient of losing mass 1.5 < x < 4. " << '\n';
  std::cin >> eng_data.cm;
}

void select_ad_eng(ad_eng_data &data) {
  char ans;
  std::cout << "how many parameters do you want to insert"
            << '\n';
  std::cout << "for this advanced engine: many(m), some (s), few(f)"
            << '\n';
  std::cin >> ans;
  switch (ans) {
    case 'm':
      create_ad_eng_all(data);
      break;

    case 's':
      create_ad_eng_med(data);
      break;

    case 'f':
      create_ad_eng_minim(data);
      break;

    default:
      std::cout << "invalid value for advanced engine"
                << '\n';
      break;
  }
}

void select_base_eng(base_eng_data &data) {
  char ans;
  std::cout << "how many parameters do you want to insert"
            << '\n';
  std::cout << "for this base engine: many(m), some (s), few(f)"
            << '\n';
  std::cin >> ans;
  switch (ans) {
    case 'm':
      create_base_eng_all(data);
      break;

    case 's':
      create_base_eng_med(data);
      break;

    case 'f':
      create_base_eng_minim(data);
      break;

    default:
      std::cout << "invalid value for base engine"
                << '\n';
      break;
  }
}

void run_countdown(sf::Text& countdown, std::vector<sf::Drawable *> const &drawables,
                   std::vector<sf::Vertex *> const& vertices,
                   sf::RenderWindow& window) {
  sf::Clock clock;
  for (int i = 10; i >= 1; --i) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
      }
    }

    countdown.setString(std::to_string(i));
    sf::Time dur = sf::Time::Zero;
    
    clock.restart();
    window.clear();
    while (dur.asSeconds() < 1.0f) {
      std::for_each(drawables.begin(), drawables.end(),
                    [&](sf::Drawable *obj) { window.draw(*obj); });
      std::for_each(vertices.begin(), vertices.end(),
                    [&](sf::Vertex *obj) { window.draw(obj, 2, sf::Lines); });
      window.draw(countdown);
      window.display();
      dur = clock.getElapsedTime();
    }
  }

  countdown.setString("GO!");
  window.clear();
  std::for_each(drawables.begin(), drawables.end(),
                [&](sf::Drawable *obj) { window.draw(*obj); });

  std::for_each(vertices.begin(), vertices.end(),
                [&](sf::Vertex *obj) { window.draw(obj, 2, sf::Lines); });
  window.draw(countdown);
  window.display();
}


void handle_exception(const std::string& err, sf::Font const& tnr) {
  std::cerr << "known exception detected: " << err << '\n';

  sf::RenderWindow error_window(sf::VideoMode(800, 600), "error SFML");

  if (!error_window.isOpen()) {
    std::cerr << "error: Impossibile create the error windows" << '\n';
    return;
  }
  sf::Text error_text(err, tnr, 30);
  error_text.setFillColor(sf::Color::Red);
  error_text.setStyle(sf::Text::Bold | sf::Text::Underlined); 
  // | operatore binario or

  sf::Text rem_time("Remaning time 10s", tnr, 15);
  error_text.setFillColor(sf::Color::White);
  error_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
  rem_time.setPosition(20, 20);

  // Posizionamento del testo al centro della finestra
  sf::FloatRect textBounds = error_text.getLocalBounds();
  error_text.setPosition((error_window.getSize().x - textBounds.width) / 2,
                         (error_window.getSize().y - textBounds.height) / 2);

  sf::Clock clock;
  // Durata desiderata del loop (10 secondi)
  sf::Time time_out = sf::seconds(10.0f);

  while (error_window.isOpen() && clock.getElapsedTime() < time_out) {
    sf::Time remaining_time = time_out - clock.getElapsedTime();
    sf::Event event;
    rem_time.setString(
        "Remaining time: " + std::to_string((int)remaining_time.asSeconds()) +
        " seconds");
    while (error_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        error_window.close();
      }
    }
    error_window.clear();
    error_window.draw(error_text);
    error_window.draw(rem_time);
    error_window.display();
    sf::sleep(sf::milliseconds(300));
  }
}

}  // namespace interface