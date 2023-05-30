#include <SFML/Window.hpp>
#include <chrono>

#include "array"
#include "menu.h"
#include "navigate.h"
#include "rocket.h"
#include "simulation.h"

int main() {
  // definisco le varie parti delle impostazioni
  rocket::Rocket rocket;
  simulation::Atmosphere atm;
  simulation::vec force;
  simulation::Gravity gravity;
  auto start = chrono::high_resolution_clock;
  auto end = chrono::high_resolution_clock;
  ;
  std::chrono::duration<double, std::nano>
      time;  // è un double di nanosecondi
             // se va a intaccare le performance mettiamo micro sec
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
        auto start = chrono::high_resolution_clock::now();
        force = simulation::force(rocket, atm, gravity);
        rocket.mass_lose();
        simulate::acelerate(rocket, force, time);
        rocket.move(time);
        rocket.move_theta();
        // stage relese, verifica fatta nella spinta

        end = chrono::high_resolution_clock();
        chrono::high_resolution_clock time = start - end;
      }
    } else
      navigation::Navigate(menu_pos, rocket);
  }
}
}
// tempo chrono o clock?
