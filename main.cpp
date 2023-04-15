#include "menu.h"
#include 'navigate.h'
#include "simulation.h"
#include <SFML/Window.hpp>


  menu::Setting_menu setting;
         // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {

            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
      


int main() {
  // definisco le varie parti delle impostazioni
  simulation::Rocket rocket;
  simulation::Atmosphere atm;
  interface::Main_menu main_menu;
  interface::Example_menu preset;
  interface::Setting_menu setting;
  interface::Menu menu;



  char menu_pos{'main'}; // ATTENZIONE IN REALTÀ È UNA STRINGA/INTERO

  
  sf::Window window(sf::VideoMode(800, 600), "My window");

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    sf::Event event;

    if (menu_pos=='play'){
      //codice di simulazione
    }
    else{
   navigation::Navigate(menu_pos, rocket);
    }
  
  
}}