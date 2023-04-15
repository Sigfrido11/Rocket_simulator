#ifndef BUTTON_H
#define BUTTON_H

/*
class Button {
public:
    Button (sf::Image* normal,sf::Image* clicked,std::string,sf::Vector2f location);
    void checkClick (sf::Vector2f);
    void setState(bool);
    void setText(std::string);
    bool getVar();
    sf::Sprite* getSprite();
    sf::String * getText();
private:
    sf::Sprite normal;
    sf::Sprite clicked;
    sf::Sprite* currentSpr;
    sf::String String;
    bool current;
};

Button::Button(sf::Image* normal,sf::Image* clicked,std::string words,sf::Vector2f location) {
    this->normal.SetImage(*normal);
    this->clicked.SetImage(*clicked);
    this->currentSpr=&this->normal;
    current =false;
    this->normal.SetPosition(location);
    this->clicked.SetPosition(location);
    String.SetText(words);
    String.SetPosition(location.x+3,location.y+3);
    String.SetSize(14);
}
void Button::checkClick (sf::Vector2f mousePos) {
    if (mousePos.x>currentSpr->GetPosition().x && mousePos.x<(currentSpr->GetPosition().x + currentSpr->GetSize().x)) {
        if(mousePos.y>currentSpr->GetPosition().y && mousePos.y<(currentSpr->GetPosition().y + currentSpr->GetSize().y)) {
            setState(!current);
        }
    }
}
void Button::setState(bool which) {
    current = which;
    if (current) {
        currentSpr=&clicked;
        return;
    }
    currentSpr=&normal;
}
void Button::setText(std::string words) {
    String.SetText(words);
}
bool Button::getVar() {
    return current;
}
sf::Sprite* Button::getSprite() {
    return currentSpr;
}

sf::String * Button::getText() {
    return &String;
}

*/
namespace button {

class Button {
sf::RectangleShape rectangle(sf::Vector2f(120, 50));

sf::fvoid sf::Transformable::setPosition	(	float 	x,float 	y)	

font* font_;
sf::text text_;

sf::Color idle_color;
sf::Color hover_color_
sf::Color active_color_;



public: 
~Button();

  sf::IntRect Box; // This could easily be protected or private

  const char Is_pressed(const sf:keybord keybord) const {

if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
{
    return 'left'
}
else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
{
    return 'right'
}
else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
{
    return 'down'
}

else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
{
    return 'up'
}
  }
};
class Insert_button {
  /* questa parte mi ha causato qualche problema non che qualche dubbio
  questi sono dei bottoni speciali che, appena cliccati, bloccano la il
  programma (lo fanno in automatico attendendo un input), sostanzialemte sono
  dei normali bottoni che però quando vengono cliccati (reagiscono alla funzione
  isclicked) leggono le stringhe digitate da tastiera e quando viene premuto
  spazio/invio proiettano sullo schermo il valore, inizialmente devono avere
  solo un bordo esterno ed essere dello stesso colore dello sfondo, una volta
  ricevuto l'input ed aver controllato che è corretto dentro al riguadro deve
  essere stampato il valore digitato
  */
  double value; // questo è il valore da stampare
public:
  sf::IntRect Box; // This could easily be protected or private

  const bool Isclicked(const sf::Mouse Mouse, const float X, const float Y) {
    // We check if it's clicked first because a direct value comparison is less
    // resource intensive than an area check

    if (!Mouse.isButtonPressed(sf::Mouse::Left)) {
      return false;
    }
    // It's pressed! Let's check it's actually in the box:
    return Box.contains(X, Y);
  }
};
} // namespace interface

#endif