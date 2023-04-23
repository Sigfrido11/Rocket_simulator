#include "menu.h"
#include 'navigate.h'
#include <SFML/Window.hpp>
#include <chrono>
#include "array"
#include "rocket.h"
#include "simulation.h"
using vec = std::array<double,2>;
int main() {
  // definisco le varie parti delle impostazioni
  rocket::Rocket rocket;
  simulation::Atmosphere atm;
  simulation::vec force;
  simulation::Gravity gravity;
  double time {0.};
  interface::Main_menu main_menu;
  interface::Example_menu preset;
  interface::Setting_menu setting;
  interface::Menu menu;

  char menu_pos{'main'};  // ATTENZIONE IN REALTÀ È UNA STRINGA/INTERO

  sf::Window window(sf::VideoMode(800, 600), "My window");

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    sf::Event event;

    if (menu_pos == 'play') {
      // game loop
      bool keep{true};
      while (keep) {
        auto num_ticks = std::chrono::duration::rep();
        auto time = num_ticks * std::chrono::duration::period();        
        const auto now = std::chrono::system_clock::now();
        time = simulation::time(time, now);
        force = simulation::force(rocket, atm, gravity);
        rocket.mass_lose();
        simulate::acelerate(rocket, force, time);
        rocket.move(time);
        rocket.stage_relase();
         
      }
    } else 
      navigation::Navigate(menu_pos, rocket);
    }
  }
}
// tempo chrono o clock?