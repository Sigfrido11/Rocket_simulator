#include "interface.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

namespace interface
{
  void set_text_style(sf::Text &text, sf::Font &font)
  {
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
  }

  void create_complete_roc(rocket_data &rocket_data)
  {
    std::cout << "give a name to your rocket"
              << "\n";
    std::cin >> rocket_data.name;
    std::cout << "how many stages do you want"
              << "\n";
    std::cin >> rocket_data.stage_num;
    std::cout << "what's the lateral area of your rocket (metres)"
              << "\n";
    std::cin >> rocket_data.lat_ar;
    std::cout << "what's the upper area of your rocket (metres)"
              << "\n";
    std::cin >> rocket_data.up_ar;
    std::cout << "what's the mass structure of your rocket (kg)"
              << "\n";
    std::cin >> rocket_data.mass_structure;
    std::cout << "what's the mass of solid conteiner of your rocket (kg)"
              << "\n";
    std::cin >> rocket_data.m_s_cont;
    std::cout << "what's the mass of solid propellant of your rocket (kg)"
              << "\n";
    std::cin >> rocket_data.s_p_m;
    for (int i{0}; i <= rocket_data.stage_num; i++)
    {
      double ans;
      std::cout
          << "what's the mass of liquid conteiner of your rocket (kg) at stage: "
          << i << "\n";
      std::cin >> ans;
      rocket_data.l_c_m.push_back(ans);
      std::cout
          << "what's the mass of solid propellant of your rocket (kg) at stage: "
          << i << "\n";
      std::cin >> rocket_data.s_p_m;
      std::cin >> ans;
      rocket_data.l_c_m.push_back(ans);
    }
    std::cout << ""
              << "\n";
    std::cin >> rocket_data.n_solid_eng;
    for (int i{0}; i <= rocket_data.stage_num; i++)
    {
      double ans;
      std::cout << "how many engine has the solid state at stage: " << i << "\n";
      std::cin >> ans;
      rocket_data.l_c_m.push_back(ans);
    }
  }
  void create_med_roc(rocket_data &rocket_data)
  {
    std::cout << "give a name to your rocket"
              << "\n";
    std::cin >> rocket_data.name;
    std::cout << "what's the mass of solid propellant of your rocket (kg)"
              << "\n";
    std::cin >> rocket_data.stage_num;
    for (int i{0}; i <= rocket_data.stage_num; i++)
    {
      double ans;
      rocket_data.l_c_m.push_back(ans);
      std::cout
          << "what's the mass of liquid propellant of your rocket (kg) at stage: "
          << i << "\n";
      std::cin >> rocket_data.s_p_m;
      std::cin >> ans;
      rocket_data.l_c_m.push_back(ans);
    }
    std::cout << "how many engine has the solid propellant at stage:"
              << "\n";
    std::cin >> rocket_data.n_solid_eng;
    for (int i{0}; i <= rocket_data.stage_num; i++)
    {
      double ans;
      std::cout << "how many engine has the liquid propellant at stage: " << i
                << "\n";
      std::cin >> ans;
      rocket_data.l_c_m.push_back(ans);
    }
  }
  void create_minim_roc(rocket_data &rocket_data)
  {
    std::cout << "give a name to your rocket"
              << "\n";
    std::cin >> rocket_data.name;
    std::cout << "how many stages do you want"
              << "\n";
    std::cin >> rocket_data.stage_num;
    std::cout << "how many engine has the solid propellant at stage:"
              << "\n";
    std::cin >> rocket_data.n_solid_eng;
    for (int i{0}; i <= rocket_data.stage_num; i++)
    {
      double ans;
      std::cout << "how many engine has the solid state at stage: " << i << "\n";
      std::cin >> ans;
      rocket_data.l_c_m.push_back(ans);
    }
  }

  void create_ad_eng_all(ad_eng_data &eng_data)
  {
    eng_data.type = 'm';
    std::cout << "give me the burn area (m^2)" << '\n';
    std::cin >> eng_data.burn_a;
    std::cout << "give me the throat area (m^2)" << '\n';
    std::cin >> eng_data.nozzle_as;
    std::cout << "give me the temperature of the engine" << '\n';
    std::cin >> eng_data.t_0;
    std::cout << "give me the dimension of the grain for the engine" << '\n';
    std::cin >> eng_data.grain_dim;
    std::cout << "give me the density of the grain for the engine" << '\n';
    std::cin >> eng_data.grain_rho;
    std::cout << "give me the engine burn rate a coefficent" << '\n';
    std::cin >> eng_data.a_coef;
    std::cout << "give me the engine burn rate n coefficent" << '\n';
    std::cin >> eng_data.burn_rate_n;
  }

  void create_ad_eng_med(ad_eng_data &eng_data)
  {
    eng_data.type = 's';
    std::cout << "give me the burn area (m^2)" << '\n';
    std::cin >> eng_data.burn_a;
    std::cout << "give me the throat area (m^2)" << '\n';
    std::cin >> eng_data.p_0;
    std::cout << "give me the temperature of the engine" << '\n';
    std::cin >> eng_data.t_0;
    std::cout << "give me the dimension of the grain for the engine" << '\n';
    std::cin >> eng_data.nozzle_as;
  }
  void create_ad_eng_minim(ad_eng_data &eng_data)
  {
    eng_data.type = 'f';
    std::cout << "give me the throat area (m^2)" << '\n';
    std::cin >> eng_data.p_0;
    std::cout << "give me the temperature of the engine" << '\n';
    std::cin >> eng_data.t_0;
  }

  void create_base_eng_all(base_eng_data &eng_data)
  {
    eng_data.type = 'm';
    std::cout << "give me the specific impulse (s)" << '\n';
    std::cin >> eng_data.isp;
    std::cout << "give me the coefficient of losing mass" << '\n';
    std::cin >> eng_data.cm;
    std::cout << "give me the pression of the engine" << '\n';
    std::cin >> eng_data.p0;
  }

  void create_base_eng_med(base_eng_data &eng_data)
  {
    eng_data.type = 's';
    std::cout << "give me the specific impulse (s)" << '\n';
    std::cin >> eng_data.isp;
    std::cout << "give me the coefficient of losing mass" << '\n';
    std::cin >> eng_data.cm;
    std::cout << "give me the pression of the engine" << '\n';
    std::cin >> eng_data.p0;
    std::cout << "give me the burn area for the engine (m^2)" << '\n';
    std::cin >> eng_data.burn_a;
  }

  void create_base_eng_minim(base_eng_data &eng_data)
  {
    eng_data.type = 'f';
    std::cout << "give me the specific impulse (s)" << '\n';
    std::cin >> eng_data.isp;
    std::cout << "give me the coefficient of losing mass" << '\n';
    std::cin >> eng_data.cm;
  }

  void run_countdown(sf::Text &countdown, std::vector<sf::Drawable *> &drawables,
                     std::vector<sf::Vertex *> &vertices,
                     sf::RenderWindow &window)
  {
    sf::Clock clock;
    for (int i = 10; i >= 1; --i)
    {

      countdown.setString(std::to_string(i));
      sf::Time dur = sf::Time::Zero;
      clock.restart();
      window.clear();
      while (dur.asSeconds() < 1.0f)
      {
        std::for_each(drawables.begin(), drawables.end(),
                      [&](sf::Drawable *obj)
                      { window.draw(*obj); });
        std::for_each(vertices.begin(), vertices.end(),
                      [&](sf::Vertex *obj)
                      { window.draw(obj, 2, sf::Lines); });
        window.draw(countdown);
        window.display();
        dur = clock.getElapsedTime();
      }
    }

    countdown.setString("LIFT OFF!");
    window.clear(sf::Color::White);
    std::for_each(drawables.begin(), drawables.end(),
                  [&](sf::Drawable *obj)
                  { window.draw(*obj); });

    std::for_each(vertices.begin(), vertices.end(),
                  [&](sf::Vertex *obj)
                  { window.draw(obj, 2, sf::Lines); });
    window.draw(countdown);
    window.display();
  }

  void select_ad_eng(ad_eng_data &data)
  {
    char ans;
    std::cout << "how many parameter do you want to insert"
              << "\n";
    std::cout << "for advanced engine: many(m), some (s), few(f)"
              << "\n";
    std::cin >> ans;
    switch (ans)
    {
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
      std::cout << "invalid value"
                << "\n";
      break;
    }
  }

  void select_base_eng(base_eng_data &data)
  {
    char ans;
    std::cout << "how many parameter do you want to insert"
              << "\n";
    std::cout << "for base engine: many(m), some (s), few(f)"
              << "\n";
    std::cin >> ans;
    switch (ans)
    {
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
      std::cout << "invalid value"
                << "\n";
      break;
    }
  }
} // namespace interface