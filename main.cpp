#include "rocket.h"
#include "simulation.h"
//#include "menu.h"
//#include "navigate.h"
//#include <SFML/Window.hpp>
#include <chrono>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>



int main() {
  // definisco le varie parti delle impostazioni
  using Vec = std::array<double, 2>;
  namespace ch = std::chrono;
  rocket::Rocket rocket;
  rocket::Rocket::Engine eng;
  Vec force;
  std::chrono::high_resolution_clock dur;
  std::chrono::high_resolution_clock time;
  ;
  enum class sel_menu {Play , Setting, Preset, Main};  // ATTENZIONE la navigazione del menù adesso è fatta con switch case
  auto sel{sel_menu::Main};
  
  sf::Window window(sf::VideoMode(800, 600), "My window");

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    sf::Event event;

    if (menu.posizione() == 'play') {
      // game loop
      bool keep{true};
      while (keep) {
        auto start = ch::high_resolution_clock::now();
        force = rocket::force();
        rocket.mass_lose();
        double nanosec = std::chrono::duration_cast<ch::nanoseconds>(dur);
        rocket.change_vel();
        /* ho preferito creare funzioni super specifiche che prendano solo l'essenziale perchè prendere l'intero razzo by
        value secondo me poteva rallentare tanto l'esecuzione visto che contiene tante informazioni*/
        
       rocket.move(nanosec);
        rocket.improve_theta();
        // stage relese, verifica fatta nella spinta

        auto dur = ch::high_resolution_clock::now() - start;
      }
    } else
      navigation::Navigate(menu_pos, rocket);
  }
}
}
// tempo chrono o clock?
