#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Audio.hpp>
#include <cmath>

int main()
{

    float width{1200.f};
    float height{600.f};
    sf::RenderWindow window(sf::VideoMode(width, height), "Rocket simulator");
    window.setPosition(sf::Vector2i(0, 0));

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(66);

    sf::Music music;
    if (!music.openFromFile("01_A Better Beginning.wav"))
        return -1; // error
    music.play();
    music.setLoop(true);

    sf::Texture texture1;
    if (!texture1.loadFromFile("rocket.png"))
    {
        // error
    }
    sf::Sprite rocket1;
    rocket1.setTexture(texture1);
    rocket1.setOrigin(sf::Vector2f(1024.f, 1024.f));
    rocket1.setScale(sf::Vector2f(0.2f, 0.2f));
    rocket1.setPosition(sf::Vector2f(250.f, height / 4.f * 3.f - 1024.f / 5.f));

    sf::RectangleShape inner_atm({500.f, 51000.f});
    inner_atm.setFillColor(sf::Color::Cyan);
    inner_atm.setPosition(0.f, height / 4.f * 3.f - 51000.f);

    sf::RectangleShape outer_atm({500.f, 49000.f});
    outer_atm.setFillColor(sf::Color::Blue);
    outer_atm.setPosition(0.f, height / 4.f * 3.f - 100000.f);

    sf::RectangleShape ground({500.f, 1000.f});
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0.f, height / 4.f * 3.f);

    sf::Texture texture2;
    if (!texture2.loadFromFile("earth.jpeg"))
    {
        // error
    }
    sf::Sprite earth;
    earth.setTexture(texture2);
    earth.setScale(sf::Vector2f(200.f / 1195.f, 200.f / 1193.f));
    earth.setPosition((width - 500.f) / 4 + 500.f - 100.f, height / 4.f - 100.f);

    sf::CircleShape rocket2(1.f);
    rocket2.setFillColor(sf::Color::Red);
    rocket2.setOrigin(1.f, 1.f);

    sf::Texture texture3;
    if (!texture3.loadFromFile("map.jpg"))
    {
        // error
    }
    sf::Sprite map;
    map.setTexture(texture3);
    map.setScale(sf::Vector2f(700.f / 2058.f, 300.f / 1262.f));
    map.setPosition(500.f, height / 2);

    sf::CircleShape rocket3(2.f);
    rocket3.setFillColor(sf::Color::Red);
    rocket3.setOrigin(2.f, 2.f);
    rocket3.setPosition(750.f, height / 4 * 3);

    sf::Font font;
    if (!font.loadFromFile("times_new_roman.ttf"))
    {
        // error
    }

    sf::Text altitude;
    altitude.setFont(font);
    altitude.setString("Altitude: ");
    altitude.setCharacterSize(24);
    altitude.setFillColor(sf::Color::White);
    altitude.setPosition((width - 500.f) / 2 + 500.f + 50.f, 30.f);

    sf::Text angle;
    angle.setFont(font);
    angle.setString("Angle: ");
    angle.setCharacterSize(24);
    angle.setFillColor(sf::Color::White);
    angle.setPosition((width - 500.f) / 2 + 500.f + 50.f, 70.f);

    sf::Text speed;
    speed.setFont(font);
    speed.setString("Speed: ");
    speed.setCharacterSize(24);
    speed.setFillColor(sf::Color::White);
    speed.setPosition((width - 500.f) / 2 + 500.f + 50.f, 110.f);

    sf::Text stage;
    stage.setFont(font);
    stage.setString("Stage: ");
    stage.setCharacterSize(24);
    stage.setFillColor(sf::Color::White);
    stage.setPosition((width - 500.f) / 2 + 500.f + 50.f, 150.f);

    sf::Text fuel_left;
    fuel_left.setFont(font);
    fuel_left.setString("Fuel left: ");
    fuel_left.setCharacterSize(24);
    fuel_left.setFillColor(sf::Color::White);
    fuel_left.setPosition((width - 500.f) / 2 + 500.f + 50.f, 190.f);

    int out_time{};

    sf::Text time;
    time.setFont(font);
    time.setCharacterSize(24);
    time.setFillColor(sf::Color::White);
    time.setPosition((width - 500.f) / 2 + 500.f + 50.f, 230.f);

    sf::Text countdown;
    countdown.setFont(font);
    countdown.setCharacterSize(400);
    countdown.setFillColor(sf::Color::Yellow);
    countdown.setPosition((width - 500.f) / 2, 20.f);

    sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(500.f, 0.f)),
            sf::Vertex(sf::Vector2f(500.f, height))};

    sf::Vertex line1[] =
        {
            sf::Vertex(sf::Vector2f(500.f, height / 2)),
            sf::Vertex(sf::Vector2f(width, height / 2))};

    sf::Vertex line2[] =
        {
            sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, 0.f)),
            sf::Vertex(sf::Vector2f((width - 500.f) / 2 + 500.f, height / 2))};

    sf::Clock clock;

    //
    // game loop starts

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        sf::Time elapsed1 = clock.getElapsedTime();
        if (elapsed1.asSeconds() <= 1)
        {
            countdown.setString("3");
        }
        if (elapsed1.asSeconds() <= 2 && elapsed1.asSeconds() > 1)
        {
            countdown.setString("2");
        }
        if (elapsed1.asSeconds() <= 3 && elapsed1.asSeconds() > 2)
        {
            countdown.setString("1");
        }
        if (elapsed1.asSeconds() < 4 && elapsed1.asSeconds() > 3)
        {
            countdown.setString("GO!");
        }
        if (elapsed1.asSeconds() < 4)
        {
            out_time = 0;
        }

        int out_time_min{out_time / 60};
        int out_time_sec{out_time - out_time_min * 60};
        time.setString("Time: " + std::to_string(out_time_min) + " min " + std::to_string(out_time_sec) + " sec ");

        rocket1.rotate(0.1f);
        outer_atm.move(0.f, 0.1f);
        ground.move(0.f, 0.1f);
        inner_atm.move(0.f, 0.1f);

        float angle_total{};
        rocket2.setPosition((width - 500.f) / 4 + 500.f - 100.f * std::sin(angle_total), height / 4.f - 200.f + 100.f * std::cos(angle_total));

        rocket3.move(0.1f, 0.f);
        const sf::Vector2f a{rocket3.getPosition()};
        if (a.x > width)
        {
            rocket3.setPosition(a.x - (width - 500.f), a.y);
        }

        window.clear();
        if (elapsed1.asSeconds() < 4)
        {
            window.draw(countdown);
        }
        window.draw(map);
        window.draw(outer_atm);
        window.draw(ground);
        window.draw(inner_atm);
        window.draw(line, 2, sf::Lines);
        window.draw(line1, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
        window.draw(rocket1);
        window.draw(earth);
        window.draw(rocket2);
        window.draw(rocket3);
        window.draw(altitude);
        window.draw(angle);
        window.draw(speed);
        window.draw(stage);
        window.draw(fuel_left);
        window.draw(time);
        if (elapsed1.asSeconds() < 4)
        {
            window.draw(countdown);
        }
        window.display();

        out_time += 1;
    }

    return 0;
}