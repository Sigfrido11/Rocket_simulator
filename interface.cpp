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

 
 void run_countdown(sf::Text& countdown, std::vector<sf::Drawable *> const &drawables,
                   std::vector<sf::Vertex *> const& vertices,
                   sf::RenderWindow& window) {
  sf::Clock clock;
  for (int i = 10; i >= 1; --i) {
    countdown.setString(std::to_string(i));

    // Disegna la scena una volta per il numero corrente
    window.clear();
    std::for_each(drawables.begin(), drawables.end(),
                  [&](sf::Drawable *obj) { window.draw(*obj); });
    std::for_each(vertices.begin(), vertices.end(),
                  [&](sf::Vertex *obj) { window.draw(obj, 2, sf::Lines); });
    window.draw(countdown);
    window.display();

    // Attendi per 1 secondo, mantenendo la finestra reattiva
    clock.restart();
    while (clock.getElapsedTime().asSeconds() < 1.0f) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
          window.close();
          return; // Esci immediatamente dal countdown
        }
      }
      sf::sleep(sf::milliseconds(10)); // Evita un alto utilizzo della CPU
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
  sf::sleep(sf::seconds(1.0f)); // Mostra "GO!" per un momento
}


 void handle_exception(const std::string& err, sf::Font const& tnr) {
  std::cerr << "known exception detected: " << err << '\n';

  sf::RenderWindow error_window(sf::VideoMode(800, 600), "error SFML");

  if (!error_window.isOpen()) {
    std::cerr << "error: Impossibile create the error windows " << '\n';
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
