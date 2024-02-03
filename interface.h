#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <algorithm> 

namespace interface {

struct rocket_data {
  std::string name{"my rocket"};
  double mass_structure {15'000};
  double up_ar{1000.};
  double s_p_m{100'000};
  double m_s_cont{15'000};
  std::vector<double> l_p_m;
  std::vector<double> l_c_m;
  int n_solid_eng{1};
  std::vector<int> n_liq_eng;
  int stage_num{1};
};

struct base_eng_data {
  double isp{250};
  double cm{2.5};
  double p0{8.e6};
  double burn_a{200e-6};
  char type;
};

struct ad_eng_data {
  double burn_a{200e-2};
  double nozzle_as{221e-2};
  double t_0{1710};
  double grain_dim{0.02};
  double grain_rho{1873};
  double a_coef{0.01};
  double burn_rate_n{0.02};
  double p_0{8e6};
  double prop_mm{178};
  char type;
};

void set_text_style(sf::Text&, sf::Font&);

rocket_data create_complete_roc(); 

rocket_data create_med_roc();

rocket_data create_minim_roc();

void create_ad_eng_all(ad_eng_data& eng_data);

void create_ad_eng_med(ad_eng_data& eng_data);

void create_ad_eng_minim(rocket_data& eng_data);

void create_base_eng_minim(base_eng_data& eng_data);

void create_base_eng_all(base_eng_data& eng_data);

void create_base_eng_med(base_eng_data& eng_data);

void select_ad_eng(ad_eng_data& eng_data);

void select_base_eng(base_eng_data& eng_data);

void run_countdown(sf::Text& countdown, std::vector<sf::Drawable *> const &drawables,
                   std::vector<sf::Vertex *> const& vertices,
                   sf::RenderWindow& window);

void handle_exception(const std::string& err, sf::Font const& tnr);

}  // namespace interface
#endif