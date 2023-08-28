#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>

namespace interface {

struct rocket_data {
  std::string name;
  double mass_structure;
  double up_ar;
  double lat_ar;
  double s_p_m;
  double m_s_cont;
  std::vector<double> l_p_m;
  std::vector<double> l_c_m;
  int n_solid_eng;
  std::vector<int> n_liq_eng;
  char type;
  int stage_num;
};

struct base_eng_data {
  double isp;
  double cm;
  double p0;
  double burn_a;
  char type;
};

struct ad_eng_data {
  double burn_a;
  double nozzle_as;
  double t_0;
  double grain_dim;
  double grain_rho;
  double a_coef;
  double burn_rate_n;
  double p_0;
  char type;
};

void set_text_style(sf::Text&, sf::Font&);

void create_complete_roc(rocket_data&); 

void create_med_roc(rocket_data&);

void create_ad_eng_all(rocket_data&);

void create_minim_roc(rocket_data&);

void create_ad_eng_med(ad_eng_data&);

void create_ad_eng_minim(ad_eng_data&);

void create_base_eng_minim(base_eng_data&);

void create_base_eng_all(base_eng_data&);

void create_base_eng_med(base_eng_data&);

void select_ad_eng(ad_eng_data&);

void select_base_eng(base_eng_data&);

void run_countdown(sf::Text&, std::vector<sf::Drawable*>&,
                   std::vector<sf::Vertex*>&,
                   sf::RenderWindow&);

}  // namespace interface
#endif